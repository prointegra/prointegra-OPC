prointegra-OPC
========

prointegra-OPC is a C++ application to take/send tags from/to industrial automation stations (for now, only through modbus TCP/IP communications, but it could use all formats supported by the [pvbrowser](http://pvbrowser.de) suite) and store them in a database, with the aim to share it to others systems as a simple OPC.

##features

+ it's configurable via easy XML files, where edit Database type, topology, communications,tag, etc.
+ database support (trough QT)
    - mysql
    - sqlite3
+ industrial communications
    - MODBUS TCP/IP
    - MODBUS RTU
    - all protocols supported by pvbrowser rlib
+ easy integration in pvbrowser SCADA code, or other custom projects for visualization

##Dependencies

+ it's based in [pvbrowser](http://pvbrowser.de) libs, so have it installed is required
+ QT 5 and it's drivers for databases
+ use of [gStools](https://github.com/gentooza/gStools) for tags conversions
+ use of pugiXML (v1.7) for configurations files

##Compile and installation

..In progress

##Contact

mail to: jcuellar (at) prointegra.com


##Changelog

**v0.0.1: (2016/11/23)**
* ALPHA prototype, first commit

