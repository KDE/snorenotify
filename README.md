Snorenotify
===========

Snorenotify is a multi platform Qt notification framework. 
Using a plugin system it is possible to create notifications with many different notification systems on Windows, Unix and Mac.


## Supported Backends ##
- [Windows 8 toast notifications](https://github.com/Snorenotify/Snoretoast) 

[ ![Windows 8 Notification](http://winkde.org/~pvonreth/other/snore/img/Thumbnail%20Pictures/win8_high.jpg) ](http://winkde.org/~pvonreth/other/snore/img/win8.png)

- OSX Notification Center

![OSX Notification Center](http://winkde.org/~pvonreth/other/snore/img/Thumbnail%20Pictures/mac2.png)
![OSX Notification Center](http://winkde.org/~pvonreth/other/snore/img/Thumbnail%20Pictures/mac.png)

- Free Desktop Notifications

[![Free Desktop Notification](http://winkde.org/~pvonreth/other/snore/img/Thumbnail%20Pictures/fd_high.jpg)](http://winkde.org/~pvonreth/other/snore/img/fd.png)

- [Growl for Windows](http://www.growlforwindows.com/)

[![Growl for Windows Notification](http://winkde.org/~pvonreth/other/snore/img/Thumbnail%20Pictures/growl_high.jpg)](http://winkde.org/~pvonreth/other/snore/img/growl.png)

- [Snarl](http://snarl.fullphat.net/)

[![Snarl Notification](http://winkde.org/~pvonreth/other/snore/img/Thumbnail%20Pictures/snarl_high.jpg)](http://winkde.org/~pvonreth/other/snore/img/snarl.png)


- Integrated notifications

[![Snore Notification](http://winkde.org/~pvonreth/other/snore/img/Thumbnail%20Pictures/snore_high.png
)](http://winkde.org/~pvonreth/other/snore/img/snore.png)

- System Tray

[![System Tray Notification](http://winkde.org/~pvonreth/other/snore/img/Thumbnail%20Pictures/tray_high.jpg)](http://winkde.org/~pvonreth/other/snore/img/tray.png)

## Supported secondary Backends ##

- [Windows Phone - Supertoasty](http://supertoasty.com/)
- [Android - Notify my Android](https://www.notifymyandroid.com/) 


## Projects using Snore ##
- [Quassel IRC](http://www.quassel-irc.org/)
- [Tomahawk](http://www.tomahawk-player.org/)

## API Doc ##
[A documentation of the API can be found here](http://winkde.org/~pvonreth/other/snore/latest/doc/html/index.html)

## Dependencies ##
Required dependencies:

- [CMake](http://www.cmake.org/)
- [Extra CMake Modules](https://projects.kde.org/projects/kdesupport/extra-cmake-modules) (read how to install ecm)
- [Qt5](http://qt-project.org/)

### How to install Extra CMake Modules (ecm) ###

git clone http://anongit.kde.org/extra-cmake-modules.git

cd extra-cmake-modules; cmake .; make ; sudo make install

## Third party libraries that we ship with our source ##
- [Snarl C++](http://sourceforge.net/p/snarlwin/code/HEAD/tree/trunk/hdr/C++/SnarlInterface_v42/) used for Snarl

