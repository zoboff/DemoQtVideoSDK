#include "videosdk.h"
#include <QRandomGenerator>

VideoSDK::VideoSDK(QObject *parent): QObject(parent)
{
    QObject::connect(&m_timer, SIGNAL(timeout()), this, SLOT(queue_processing()));
}

VideoSDK::~VideoSDK()
{
    if(m_socket)
        m_socket->close();
}

void VideoSDK::open(const QString &host, const QString &pin)
{
    m_host = host;
    m_pin = pin;

    if(m_socket)
        m_socket->close();

    QString name = "VideoSDK to API websocket " + QString::number(QRandomGenerator::global()->generate());
    m_socket = new QWebSocket(name, QWebSocketProtocol::VersionLatest, this);
    m_socket->setObjectName(name);

    QObject::connect(m_socket, SIGNAL(connected()), this, SLOT(onSocketConnected()));
    QObject::connect(m_socket, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));
    QObject::connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onSocketError(QAbstractSocket::SocketError)));
    QObject::connect(m_socket, SIGNAL(destroyed(QObject*)), this, SLOT(onSocketDestroyed(QObject*)));

    QUrl url = QUrl("ws://" + host + ":8765");
    m_socket->open(url);

    /* Start a send texts processing */
    m_timer.start(QUEUE_INTERVAL);
}

void VideoSDK::onSocketConnected()
{
    qDebug() << "WebSocket '" + m_socket->origin() + "' connected" << endl;
    QObject::connect(m_socket, SIGNAL(textMessageReceived(QString)), this, SLOT(onTextReceived(QString)));

    auth();
}

void VideoSDK::onSocketDisconnected()
{
    qDebug() << "WebSocket '" + m_socket->origin() + "' disconnected" << endl;

    if(m_started)
        emit closed();

    /* Stop queue sending */
    m_timer.stop();

    m_started = false;
    m_socket->deleteLater();
    m_socket = nullptr;
}

void VideoSDK::onSocketError(QAbstractSocket::SocketError)
{
    /* Emit signal */
    if(m_socket)
        emit error(m_socket->errorString());
    else
        emit error("WebSocket already closed");
}

void VideoSDK::auth()
{
    QString command = "{\"method\": \"auth\", \"type\": \"secured\", \"credentials\": \"" + m_pin + "\"}";
    /* Only here send() */
    send(command);
}

void VideoSDK::processIncoming(const QString &data)
{
    QJsonParseError err;
    QJsonDocument json_doc = QJsonDocument::fromJson(data.toUtf8(), &err);
    QJsonObject json_obj = json_doc.object();

    /* { "method": ..., "result": ... }; */
    if(err.error == QJsonParseError::NoError && json_obj.contains(OBJ_METHOD) && json_obj.contains(OBJ_RESULT))
    {
        /* { "method": "auth", "result": true }; */
        if(json_obj[OBJ_METHOD] == V_AUTH && json_obj[OBJ_RESULT] == true)
        {
            m_started = true;

            /* Emit signal */
            emit opened();

            /* Now */
            now_ready();
        }
        /* {"event": "appStateChanged", "appState": None} */
        else if()
        {

        }
        /* === ERROR ============================================= */
        /* { "error": ... } */
        else if(json_obj.contains(OBJ_ERROR))
        {
            QString err = QString(json_obj[OBJ_ERROR].toString());

            /* Emit signal */
            emit error(QString(err));

            /* Now */
            now_error(err);
        }
    }
    /* === ERROR ============================================= */
    else if(err.error != QJsonParseError::NoError)
    {
        QString err = "processIncoming(): JSON parse error";

        /* Emit signal */
        emit error(QString(err));

        /* Now */
        now_error(err);
    }

    // auto j = {"appState": None, "method": "getAppState", "result": None};
}

/*
 * Request an application state
*/
void VideoSDK::requestAppState()
{
    QString command = "{\"method\": \"getAppState\"}";
    send_command(command);
}

/*
 * Request the settings list
*/
void VideoSDK::requestSettings()
{
    QString command = "{\"method\": \"getSettings\"}";
    send_command(command);
}

/*
 * Request the system information
*/
void VideoSDK::requestSystemInfo()
{
    QString command = "{\"method\": \"getSystemInfo\"}";
    send_command(command);
}

/*
 * Request the information about monitors
*/
void VideoSDK::requestMonitorsInfo()
{
    QString command = "{\"method\": \"getMonitorsInfo\"}";
    send_command(command);
}

/*
 * When ready
*/
void VideoSDK::now_ready()
{
    qDebug() << ":now_ready" << endl ;
    requestAppState();
    requestSettings();
    requestSystemInfo();
    requestMonitorsInfo();
}

/*
 * When error
*/
void VideoSDK::now_error(QString &error)
{
    qDebug() << ":now_error" << endl;
    qDebug() << " error: " << error << endl;
}

bool VideoSDK::started() const
{
    return m_started;
}

/*
 * Main method to send commands to websocket API
*/
void VideoSDK::send_command(const QString &data)
{
    m_mutex.lock();
    m_queue.append(new QString(data));
    qDebug() << "Queue length:" << m_queue.length() << endl;
    m_mutex.unlock();
}

int VideoSDK::send(const QString &data)
{
    int res = m_socket->sendTextMessage(data);
    if(res <= 0)
        qDebug() << "Send error! " << endl << "Data: " << data << endl;
    else
        qDebug() << "Send Ok: " << data << endl;

    return res;
}

/*
 * QTimer::timeout signal processing
*/
void VideoSDK::queue_processing()
{
    /* --- return --- */
    if(!started()) return;

    /* LOCK */
    m_mutex.lock();

    if(m_queue.length() > 0)
    {
        QString s {*m_queue[0]};

        /* Send data to Video SDK */
        if(send(s) > 0)
        {
            /* If the data was successfully sent  */
            m_queue.removeAt(0);
            qDebug() << "Queue length:" << m_queue.length() << endl;
        }
    }

    /* UNLOCK */
    m_mutex.unlock();
}

/*
 * Signal textMessageReceived() from QWebSocket* m_socket
*/
void VideoSDK::onTextReceived(const QString data)
{
    qDebug() << "Message received:" << data << endl;

    // Process
    processIncoming(data);
}

/*
 * Signal destroyed(QObject) from QWebSocket* m_socket
*/
void VideoSDK::onSocketDestroyed(QObject *obj)
{
    qDebug() << "WebSocket '" + obj->objectName() + "' distroyed" << endl;
}

