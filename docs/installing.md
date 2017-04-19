HOWTO installing prointegra-OPC
========

For the instance we have no created any package for installing prointegra-OPC, so you'll need to compile it by hand

##under Ubuntu 16.04 (Xenial) GNU/Linux

###pvbrowser

first of all you'll need to have installed pvbrowser SCADA suite, it's dependencies will be Prointegra-OPC dependencies basically.
You can see it's repository [here](https://github.com/pvbrowser/pvb)
Pay attention that pvbrowser accepts QT5 now, so our application does.

####Known dependencies in ubuntu 16.04(xenial)

packages:
```
qt5-defaults
libqt5sql5-mysql
libqt5sql5-sqlite
```

###prointegra-OPC
after that, simply doing:
```
qmake
```
and:
```
make
```
should give us the executable file: prointegra-OPC

you'll have also to compile for your platform every communication daemon, executing ```qmake``` and ```make``` in it's folder does it.

pugiXML is added to the project, so it will be automatically recompiled.

###gStools

gStools is already compiled, but if you encounter problems recompile it! See it's Makefile to change properties, as compilation for 32 or 64 bits

then we have made a launching script for our app ´´´./launch-script.sh´´´, to find libGstools.so, modify it with your preferences.

###Under MS Windows XP
work in progress...


