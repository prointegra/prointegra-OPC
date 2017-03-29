
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
+ check if database table has changed before update it via SQL. (speed it up)
+ better tables in database creation, we could check not only if it already exists, also it's schema for looking for differences
+ safe table writting model, system doesn't overwrite a modified table by a cycle parameter set in config.h
+ set a debug flag to hide show debug messages, add a debug function system
+ better for loops, improved to finish when aim is reached
...
#KNOWN BUGS
+ more safety checks, if no good xml files found no work!
+ only tested with modbus TCP/IP
+ log files for daemons no working
+ garbage recollecting after exiting


