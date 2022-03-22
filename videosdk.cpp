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

/* Connect to TrueConf Server

Parameters
----------
    server: str
        Server address. For example, IP address;
    port: int
        Port. Default port is 4307.
*/
void VideoSDK::connectToServer(const QString &server, const int port)
{
    /* {"method": "connectToServer", "server": server, "port": port} */
    QString command = "{\"method\": \"connectToServer\", \"server\": \"" + server + "\", \"port\": " + QString::number(port) + "}";
    API_send(command);
}

/*
 * Connect to TrueConf Online service
*/
void VideoSDK::connectToService()
{
    /* command = {"method": "connectToService"} */
    QString command = "{\"method\": \"connectToService\"}";
    API_send(command);
}

/*
 * Login user to TrueConf Server
*/
void VideoSDK::login(const QString &callId, const QString &password)
{
    /* command = {"method": "login", "login": callId, "password": password, "encryptPassword": true} */
    QString command = "{\"method\": \"login\", \"login\": \"" + callId + "\", \"password\": \"" + password+ "\", \"encryptPassword\": true}";
    API_send(command);
}

/*
 * Logout current user
*/
void VideoSDK::logout()
{
    /* command = {"method": "logout"} */
    QString command = "{\"method\": \"logout\"}";
    API_send(command);
}

/*
 * Make a call
*/
void VideoSDK::call(const QString &peerId)
{
    /* command = {"method": "call", "peerId": peerId} */
    QString command = "{\"method\": \"call\", \"peerId\": \"" + peerId +"\"}";
    API_send(command);
}

/*
 * Reject an incoming call or invitation to the conference \
*/
void VideoSDK::reject()
{
    QString command = R"({"method": "accept"})";
    API_send(command);
}

/*
 * Accept an incoming call or invitation to the conference
*/
void VideoSDK::accept()
{
    QString command = R"({"method": "accept"})";
    API_send(command);
}

/*
 * End a call or a conference
*/
void VideoSDK::hangUp(const bool forAll)
{
    QString command;

    if(forAll)
        command = R"({"method": "hangUp", "forAll": true})";
    else
        command = R"({"method": "hangUp", "forAll": false})";

    API_send(command);
}

/*
 * Move a participant's video to an any other display.
 * Available in the conference only.
*/
void VideoSDK::moveVideoSlotToMonitor(const QString &callId, int monitorIndex)
{
    /* command = {"method": "moveVideoSlotToMonitor", "callId": callId, "monitorIndex": monitorIndex} */
    QString command = "{\"method\": \"moveVideoSlotToMonitor\", \"callId\": \"" + callId + "\", \"monitorIndex\": \"" + QString::number(monitorIndex) + "\"}";
    API_send(command);
}

/*
 * Send a text command to a remote client (application)
*/
void VideoSDK::sendCommand(QString &peerId, QString &cmd)
{
    /* {"method": "sendCommand", "peerId": peerId, "command": command} */
    QString command = "\{\"method\": \"sendCommand\", \"peerId\": \"" + peerId + "\", \"command\": \"" + cmd + "\"}";
    API_send(command);
}

/*
 * Set a new position to the active PTS camera
*/
void VideoSDK::setPanPos(int pos)
{
    /* {"method": "setPanPos", "pos": pos} */
    QString command = "{\"method\": \"setPanPos\", \"pos\":" + QString::number(pos) + "}";
    API_send(command);
}

/*
 * Set a new position to the active PTS camera
*/
void VideoSDK::setTiltPos(int pos)
{
    /* {"method": "setPanPos", "pos": pos} */
    QString command = "{\"method\": \"setTiltPos\", \"pos\":" + QString::number(pos) + "}";
    API_send(command);
}

/*
 * Set a new position to the active PTS camera
*/
void VideoSDK::setZoomPos(int pos)
{
    /* {"method": "setPanPos", "pos": pos} */
    QString command = "{\"method\": \"setZoomPos\", \"pos\":" + QString::number(pos) + "}";
    API_send(command);
}

void VideoSDK::onSocketConnected()
{
    qDebug() << "WebSocket '" + m_socket->origin() + "' connected" << Qt::endl;
    QObject::connect(m_socket, SIGNAL(textMessageReceived(QString)), this, SLOT(onSocketReceived(QString)));

    auth();
}

void VideoSDK::onSocketDisconnected()
{
    qDebug() << "WebSocket '" + m_socket->origin() + "' disconnected" << Qt::endl;

    if(m_started)
        emit closed();

    /* Stop queue sending */
    m_timer.stop();

    m_started = false;
    m_socket->deleteLater();
    m_socket = nullptr;
}

void VideoSDK::onSocketError(QAbstractSocket::SocketError err)
{
    /* Emit signal */
    if(m_socket)
        emit error(m_socket->errorString());
    else if(err == QAbstractSocket::SocketError::ConnectionRefusedError)
        emit error("Connection Refused Error");
    else
        emit error("WebSocket closed. Error: " + QString::number(err));
}

void VideoSDK::auth()
{
    QString command = "{\"method\": \"auth\", \"type\": \"secured\", \"credentials\": \"" + m_pin + "\"}";
    /* Only here send() */
    API_send_direct(command);
}

void VideoSDK::processIncoming(const QString &data)
{
    QJsonParseError err;
    QJsonDocument json_doc = QJsonDocument::fromJson(data.toUtf8(), &err);
    QJsonObject json_obj = json_doc.object();

    /* { "method": ..., "result": ... }; */
    if(err.error == QJsonParseError::NoError && json_obj.contains(OBJ_METHOD))
    {
        /* { "method": "auth", "result": true }; */
        if(json_obj[OBJ_METHOD] == V_AUTH && json_obj.contains(OBJ_RESULT) && json_obj[OBJ_RESULT].toBool() == true)
        {
            m_started = true;

            /* Emit signal */
            emit opened();

            /* Now */
            now_ready();
        }
        /* {"event": "appStateChanged", "appState": None} */
        /* "{"method":"event","event":"appStateChanged","embeddedHttpPort":8766,"appState":4,"desktopSharing":{"running":false},"broadcastPicture":{"running":false},"audioCaptureTest":false}" */
        else if(json_obj.contains(OBJ_EVENT) && json_obj[OBJ_EVENT].toString() == V_APP_STATE_CHANGED
                && json_obj.contains(OBJ_APP_STATE))
        {
            int m_state = json_obj[OBJ_APP_STATE].toInt(0);

            /* Emit signal */
            emit change_state(State(m_state));
        }
        /* {"method":"getAppState","requestId":"","embeddedHttpPort":8766,"appState":3,"desktopSharing":{"running":false},"broadcastPicture":{"running":false},"audioCaptureTest":false,"result":true} */
        else if(json_obj[OBJ_METHOD].toString() == V_GET_APP_STATE && json_obj.contains(OBJ_APP_STATE))
        {
            int m_state = json_obj[OBJ_APP_STATE].toInt(0);

            /* Emit signal */
            emit change_state(State(m_state));
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
        QString err = "processIncoming(): JSON parse 0error";

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
    API_send(command);
}

/*
 * Request the settings list
*/
void VideoSDK::requestSettings()
{
    QString command = "{\"method\": \"getSettings\"}";
    API_send(command);
}

/*
 * Request the system information
*/
void VideoSDK::requestSystemInfo()
{
    QString command = "{\"method\": \"getSystemInfo\"}";
    API_send(command);
}

/*
 * Request the information about monitors
*/
void VideoSDK::requestMonitorsInfo()
{
    QString command = "{\"method\": \"getMonitorsInfo\"}";
    API_send(command);
}

void VideoSDK::requestConferenceParticipants()
{
    QString command = R"({"method": "getConferenceParticipants"})";
    API_send(command);
}

/*
 * When ready
*/
void VideoSDK::now_ready()
{
    qDebug() << ":now_ready" << Qt::endl ;
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
    qDebug() << ":now_error" << Qt::endl;
    qDebug() << " error: " << error << Qt::endl;
}

bool VideoSDK::started() const
{
    return m_started;
}

QString VideoSDK::stateToText(const State state)
{
    switch(state)
    {
        case State::none: return "No connection to the server and TrueConf Room does nothing";
        case State::connect: return "Tries to connect to the server";
        case State::login: return "You need to login";
        case State::normal: return "Connected to the server and logged in";
        case State::wait: return "Pending: either it calls somebody or somebody calls it";
        case State::conference: return "In the conference";
        case State::close: return "Finishing the conference";
        default: return "Unknown state";
    }
}

/*
 * This is the main method to send commands to websocket API
*/
void VideoSDK::API_send(const QString &data)
{
    m_mutex.lock();
    m_queue.append(new QString(data));
    qDebug() << "Queue length:" << m_queue.length() << Qt::endl;
    m_mutex.unlock();
}

int VideoSDK::API_send_direct(const QString &data)
{
    int res = m_socket->sendTextMessage(data);
    if(res <= 0)
        qDebug() << "Send error! " << Qt::endl << "Data: " << data << Qt::endl;
    else
        qDebug() << "Send Ok: " << data << Qt::endl;

    return res;
}

/*
 * QTimer::timeout signal processing
*/
void VideoSDK::queue_processing()
{
    /* LOCK */
    m_mutex.lock();

    if(!started())
    {
        /* UNLOCK */
        m_mutex.unlock();

        return;
    }

    if(m_queue.length() > 0)
    {
        QString s {*m_queue[0]};

        /* Send data to Video SDK */
        if(API_send_direct(s) > 0)
        {
            /* If the data was successfully sent  */
            m_queue.removeAt(0);
            qDebug() << "Queue length:" << m_queue.length() << Qt::endl;
        }
    }

    /* UNLOCK */
    m_mutex.unlock();
}

/*
 * Signal textMessageReceived() from QWebSocket* m_socket
*/
void VideoSDK::onSocketReceived(const QString data)
{
    qDebug() << "Message received:" << data << Qt::endl;

    // Emit
    emit socketReceived(data);

    // Process
    processIncoming(data);
}

/*
 * Signal destroyed(QObject) from QWebSocket* m_socket
*/
void VideoSDK::onSocketDestroyed(QObject *obj)
{
    qDebug() << "WebSocket '" + obj->objectName() + "' distroyed" << Qt::endl;
}

