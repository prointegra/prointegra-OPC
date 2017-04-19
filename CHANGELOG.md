make cleanCHANGELOG
===

**NEXT: (XXXX/XX/XX)**

**v0.0.4: (2017/04/19)**
+ new features
    - time triggered read tables implemented
    - new modbus_client2 integrated
    - locking tables when being edited, number of lock cycles in config.h
+ polished
    - some info messages improved
    - some debug code removed at piDatabase.cpp
    - some debug code removed at piDataTable.cpp
    - some debug code removed at piTriggersTable.cpp
+ bugfixes
    - some memory leaks fixed at piDatabase.cpp
    - some memory leaks fixed at piComm.cpp
    - at config.cpp (int checkSlaveName function) returned always failed with more than 1 slave
    - delTable error in piDatabase, solved programming more C++ like style.
    - reset of write trigers was not properly working!
    - temporal fix, removing info writted by modbus_client2 in it's thread, it could cause segment fault fprintf in some circunstances
    - storemysql returned nothing in updatemysql fucntion inside pidatatable class!
    - modbus_client2 fixed for several slaves writting

**v0.0.3: (2017/02/07)**
+ BETHA, app can now store INT tags values from MODBUS TCP/IP slave, and write to them.
    - LASTVALUE and LOG tables
    - triggers table for reading and writting data
    - NEW FEATURES:LASTVALUE tables added, where only last value of a tag is saved
    - BUGFIX: not trying to store data when communications were completely wrong
    - CLEANING AND POLISHING: some old code not needed removed

**v0.0.2: (2016/12/13)**

+ BETHA, app can now store INT tags values from MODBUS TCP/IP slave, to LOG tables of SQLITE3,MYSQL databases.

**v0.0.1: (2016/11/23)**

+ ALPHA prototype, first commit

