#include <QCoreApplication>
#include "../videosdk/videosdk.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    VideoSDK sdk;

    /* Run before:
       "C:\Program Files\TrueConf\Room\TrueConfRoom.exe" -pin "123"
    */

    sdk.open_session("127.0.0.1", "123");
    sdk.call("echotest_es@trueconf.com");

    return a.exec();
}
