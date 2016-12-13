gStools : Gentooza SCADA general purpose tools
========

##1 Description

Gentooza SCADA tools is a library I've made to help me developing industrial SCADA software using the free as in freedom suite pvbrowser (https://github.com/pvbrowser/pvb).

Pvbrowser is a great piece of software, but it's modbus communications are so simple, you need to adapt tags therefore to a readable format (dint, float, etc.).

So here we can find these kind of functions with others I usually need in a common industrial application.

It's licensed with a GPLv2 or later license, I know it could be negative for lots of people :-D but what I look for is not to share a world wide tool, is to share an useful tool for only free as in freedom projects, to improve and help only collaborative, social friendly commercial projects. (as I try to develop!)

##2 Compile and installation

it's not tested under windows, but it will in near future!

###Linux

+ compiling is easy and don't need any special library:

```
make
```

+ there is no install option, but you can take your library .so file to a known folder and add it to system path when running your application with command

```
export LD_LIBRARY_PATH=(path-to-gStools):$LD_LIBRARY_PATH
```

##3 License

gStools is licensed under GPLv2 or later


##4 Contact

+ mail to: joa.cuellar (at) riseup.net