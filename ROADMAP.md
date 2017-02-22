
ROADMAP
===
#FEATURES
+ simple terminal gui to manage program
+ stress tests done
+ MORE databases support
    - TDS support
+ MORE tags type support
    - BOOL
    - WORD
    - REAL
    - STRING
...
#KNOWN BUGS
+ more safety checks, if no good xml files found no work!
+ only tested with modbus TCP/IP
+ not working properly the modbus TCP/IP daemon, it has to be launch standalone:
```
./comm/modbus_client/modbus_client ./comm/modbus_client/MBTCP.ini
```

