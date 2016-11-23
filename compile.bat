@echo off
set myroot=%CD%
set mymake=%MYROOT%\win\bat\make_module.bat

echo setting environment

rem set MYPAUSE=TRUE



:MINGW
set PVBDIR=c:\pvb
set MINGWDIR=C:\Qt\Qt5.6.0\Tools\mingw492_32
set QTDIR=C:\Qt\Qt5.6.0\5.6\mingw49_32
rem set MINGWDIR=c:\mingw-old
rem set QTDIR=c:\Qt\4.8.2
set PATH=%PVBDIR%\win-mingw\bin;%MINGWDIR%\bin;%QTDIR%\bin;c:\windows;c:\windows\system32
set PYTHONPATH=%PVBDIR%\win-mingw\bin;%PYTHONPATH%

:END

echo executing qmake
qmake
echo compiling using mingw32-make
mingw32-make
