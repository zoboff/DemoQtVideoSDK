#include "videosdk.h"

VideoSDK::VideoSDK(const QString &host, QObject *parent): QObject(parent)
{
    m_host = host;
    m_socket = new QWebSocket("socket 01", QWebSocketProtocol::VersionLatest, this);

    QObject::connect(m_socket, SIGNAL(connected()), this, SLOT(onConnected()));
    QObject::connect(m_socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));

    QUrl url = QUrl("ws://" + host + ":8765");
    m_socket->open(url);
}

void VideoSDK::onConnected()
{
    qDebug() << "WebSocket connected";
    QObject::connect(m_socket, SIGNAL(textMessageReceived(const QString&)), this, SLOT(onTextReceived(const QString&)));

    auth();
}

void VideoSDK::onDisconnected()
{
    //m_socket->deleteLater();
    qDebug() << "Disconnected";
}

void VideoSDK::auth()
{
    QString command = "{\"method\": \"auth\", \"type\": \"secured\", \"credentials\": \"123\"}";
    send(command);
}

ConnectionStatus VideoSDK::connectionStatus() const
{
    return m_connectionStatus;
}

void VideoSDK::send(const QString &data)
{
    if(true) //m_socket->connected())
    {
        m_socket->sendTextMessage(data);
        qDebug() << "Send: " << data;
    }
    else {
        qDebug() << "Sending error";
    }
}

void VideoSDK::onTextReceived(QString data)
{
    qDebug() << "Message received:" << data;

    QJsonParseError err;
    QJsonDocument json_doc = QJsonDocument::fromJson(data.toUtf8(), &err);
    QJsonObject json_obj = json_doc.object();

    if(json_obj.contains(OBJ_METHOD) && json_obj.contains(OBJ_RESULT)){
        if(json_obj[OBJ_METHOD] == V_AUTH && json_obj[OBJ_RESULT] == true)
        {
            m_connectionStatus = ConnectionStatus::normal;

            qDebug() << "Successful connection";
        }
        else {
            qDebug() << "Error connection";
        }
    }

}
