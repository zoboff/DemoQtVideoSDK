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

enum State
{
    none = 0,       // No connection to the server and TrueConf Room does nothing
    connect = 1,    // Tries to connect to the server
    login = 2,      // you need to login
    normal = 3,     // Connected to the server and logged in
    wait = 4,       // Pending: either it calls somebody or somebody calls it
    conference = 5, // In the conference
    close = 6       // Finishing the conference
};

class VideoSDK : public QObject
{
    Q_OBJECT

public:


public:
    explicit VideoSDK(QObject *parent);
    ~VideoSDK();
    void open(const QString &host, const QString &pin);

    bool started() const;

protected:
    void send(const QString &data);
    void auth();

signals:
    void opened();
    void closed();
    void error(QString text);
    //void change_state(State state);

private:
    bool m_started = false;
    QString m_host;
    QString m_pin;
    QWebSocket* m_socket = nullptr;

private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError(QAbstractSocket::SocketError);
    void onTextReceived(QString data);
    void onSocketDestroyed(QObject *obj = nullptr);

};

#endif // VIDEOSDK_H
