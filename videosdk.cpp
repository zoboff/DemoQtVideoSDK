#include "videosdk.h"
#include <QRandomGenerator>

VideoSDK::VideoSDK(QObject *parent): QObject(parent)
{

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
        emit error("WebSocket already closed.");
}

void VideoSDK::auth()
{
    QString command = "{\"method\": \"auth\", \"type\": \"secured\", \"credentials\": \"" + m_pin + "\"}";
    send(command);
}

bool VideoSDK::started() const
{
    return m_started;
}

void VideoSDK::send(const QString &data)
{
    m_socket->sendTextMessage(data);
    qDebug() << "Send command: " << data << endl;
}

void VideoSDK::onTextReceived(QString data)
{
    qDebug() << "Message received:" << data << endl;

    QJsonParseError err;
    QJsonDocument json_doc = QJsonDocument::fromJson(data.toUtf8(), &err);
    QJsonObject json_obj = json_doc.object();

    if(json_obj.contains(OBJ_METHOD) && json_obj.contains(OBJ_RESULT))
    {
        if(json_obj[OBJ_METHOD] == V_AUTH && json_obj[OBJ_RESULT] == true)
        {
            m_started = true;

            /* Emit signal */
            emit opened();
        }
        else if(json_obj.contains(OBJ_ERROR))
        {
            QString err = QString(json_obj[OBJ_ERROR].toString());

            /* Emit signal */
            emit error(QString(err));
        }
    }

    // auto j = {"appState": None, "method": "getAppState", "result": None};

}

void VideoSDK::onSocketDestroyed(QObject *obj)
{
    qDebug() << "WebSocket '" + obj->objectName() + "' distroyed" << endl;
}

