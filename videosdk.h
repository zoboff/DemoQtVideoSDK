#ifndef VIDEOSDK_H
#define VIDEOSDK_H

#include <QObject>
#include <QWebSocket>
#include <QDebug>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringLiteral>

#define OBJ_METHOD "method"
#define OBJ_TYPE  "type"
#define OBJ_REQUEST_ID "requestId"
#define OBJ_ERROR "error"
#define OBJ_RESULT "result"
#define OBJ_CREDENTIALS "credentials"

#define V_AUTH "auth"
#define V_SECURED "secured"

enum ConnectionStatus
{
    unknown = 0,
    started = 1,
    connected = 2,
    normal = 3,
    close = 4,
};

class VideoSDK : public QObject
{
    Q_OBJECT

public:


public:
    explicit VideoSDK(const QString &host, QObject *parent);

    ConnectionStatus connectionStatus() const;

protected:
    void send(const QString &data);
    void auth();

signals:

private:
    QString m_host;
    QWebSocket* m_socket;
    QByteArray m_data;
    ConnectionStatus m_connectionStatus = ConnectionStatus::unknown;

public slots:
    void onConnected();
    void onDisconnected();
    void onTextReceived(QString data);

};

#endif // VIDEOSDK_H
