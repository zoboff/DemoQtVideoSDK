# DemoQtVideoSDK

## How to install TrueConf Room

1. Download TrueConf Room for your OS:

* [Windows](https://trueconf.com/download/room/trueconf_room_setup.exe)

* [Debian 10](https://trueconf.com/download/room/linux/trueconf_room_debian10_amd64.deb)

* [Debian 11](https://trueconf.com/download/room/linux/trueconf_room_debian11_amd64.deb)

* [Ubuntu 20.04](https://trueconf.com/download/room/linux/trueconf_room_ubuntu2004_amd64.deb)

* [Ubuntu 21.10](https://trueconf.com/download/room/linux/trueconf_room_ubuntu2110_amd64.deb)

* [Ubuntu 21.10 for Raspberry Pi](https://trueconf.com/download/room/linux/trueconf_room_ubuntu2110_arm64.deb)

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