#include <QCoreApplication>
#include "../videosdk/videosdk.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    VideoSDK sdk;

    sdk.open_session("127.0.0.1");
    sdk.call("echotest_es@trueconf.com");

    return a.exec();
}
