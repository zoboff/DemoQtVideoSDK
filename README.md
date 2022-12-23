# DemoQtVideoSDK

C++(Qt) code examples that use TrueConf VideoSDK API and TrueConf Room API

## How to use

### Download and install TrueConf VideoSDK or TrueConf Room

   1. [Download TrueConf VideoSDK or TrueConf Room](https://github.com/TrueConf/pyVideoSDK/blob/main/download.md)
   1. Install with default settigs (Port = 80)
   1. Launch application c **-pin** parameter in the command line. 
   
   Windows:
   ```
   "C:\Program Files\TrueConf\VideoSDK\VideoSDK.exe" -pin "pin123"
   ```
   Linux:
   ```
   $ trueconf-video-sdk -pin "123"
   ```

### Login

   1. Get your free accounts in https://t.me/TrueConfSDKPromoBot
   1. Click at the link on the application main screen and open the **web-manager**
   1. Connect to the server and login in

**Now the application is ready to use**   

## just_call

This is the simplest console application that calls the user by _TrueConf ID_

```cpp
#include <QCoreApplication>
#include "../videosdk/videosdk.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    VideoSDK sdk;

    sdk.open_session("127.0.0.1", "123");  // open a new sesion with Room or VideoSDK
    sdk.call("echotest_es@trueconf.com"); // call

    return a.exec();
}
```

## SignalsAndState
![screenshot of the DemoQtVideoSDK main screen](screenshot.png)

## EmergencyCall

Just an animated calling button on the bottom right corner of the screen
