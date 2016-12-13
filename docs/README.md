prointegra-OPC
========

prointegra-OPC is a C++ application to take/send tags from/to industrial automation stations (for now, only through modbus TCP/IP communications, but it could use all formats supported by the [pvbrowser](http://pvbrowser.de) suite) and store them in a database, with the aim to share it to others systems as a simple OPC.

##features

+ it's configurable via easy XML files, where edit Database type, topology, communications,tag, etc.
+ database support (trough QT)
    - sqlite3
    - MySQL
+ industrial communications
    - MODBUS TCP/IP
+ easy integration in pvbrowser SCADA code, or other custom projects for visualization

##Dependencies

+ it's based in [pvbrowser](http://pvbrowser.de) libs, so have it installed is required.
+ QT 5 and it's drivers for databases. (MySQL, SQLite3)
    - under ubuntu xenial (16.04): ``` sudo apt-get install libqt5sql-mysql libqt5sql-sqlite ```
+ use of [gStools](https://github.com/gentooza/gStools) for tags conversions (already included in /lib folder)
+ use of pugiXML for configurations files (already cinluded in /lib folder, v1.7)

##Compile and installation

###Under GNU/Linux
normally doing:
```
qmake
```
and:
```
make
```
should give us the executable file: prointegra-OPC

###Under MS Windows XP
work in progress...

##Contact

mail to: jcuellar (at) prointegra.com

##TODO/ROADMAP
[roadmap](./ROADMAP.md)


##Changelog

**v0.0.1: (2016/11/23)**
* ALPHA prototype, first commit
