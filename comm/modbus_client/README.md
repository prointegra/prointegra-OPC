modbus client2
========

The original modbus client program is free software, and has been developed by the [pvbrowser](pvbrowser.de) project, and is provided by it's package pvaddon: [pvaddon](https://github.com/pvbrowser/pvbaddon)

This modification is motivated by this [issue](https://github.com/pvbrowser/pvbaddon/issues/2)
And latest source code is available [here](https://github.com/gentooza/modbus_client2)

I hope it will be merged by the so beautiful project pvbrowser, if not, I hope it will be at least as useful as it's for me.

##improves
+ Original modbus_client, configured for Modbus TCP/IP only allowed one slave connection at a time, making imposible to have several instances as they were competing by one socket (it's my interpretation). Now, we change socket address and reseting it every time we change the slave id being interrogated.
+ C++ styled program, I like to program C more than C++, but as it's in the middle I prefer to code it more C++ style than C for speeding up developing.
+ More functions, I don't like loooong functions, I prefer to cut them.
+ logging possibility, as we have the debug option, it could be nice to have a logging to file option. It's also important to see the real time we spend in every slave, etc.
+ garbage collecting, it seems program doesn't free anything by default when we stop it, or kill it, now we capture SIGTERM and SIGINT to clean and close.

##compile
as pvbrowser you need rllib installed, the full pvbrowser package is recommended
```
qmake
```
and
```
make
```
##usage
./modbus_client2 *configuration file*
