# DemoQtVideoSDK

## How to install TrueConf Room

[Download TrueConf Room](https://github.com/TrueConf/pyVideoSDK/blob/main/download.md)

## Before: Launch the *TrueConf Room* application with *-pin* parameter.

Windows:
```
"C:\Program Files\TrueConf\Room\TrueConfRoom.exe" -pin "123"
```

Linux:
```
$ trueconf-room -pin "123"
```

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
