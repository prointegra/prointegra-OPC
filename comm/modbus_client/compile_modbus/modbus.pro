#lessThan(QT_MAJOR_VERSION, 5) {
#  QT         += opengl svg webkit
#  CONFIG     += uitools warn_on release
#}else{
#  QT         += uitools webkitwidgets widgets opengl svg webkit
  CONFIG     += warn_on release
#}    
QMAKE_LFLAGS += -static-libgcc
INCLUDEPATH  += ../pvbrowser
#INCLUDEPATH  += ../qwt/include
#INCLUDEPATH  += ../qwt/src
#LIBS         += ../qwt/lib/libqwt.a
#DEFINES      += PVDEVELOP
#DEFINES     += NO_QWT
#macx:DEFINES += PVMAC
#macx:DEFINES += unix
#unix:LIBS    += -ldl

#HEADERS       = mainwindow.h \
#                opt.h \
#                editor.h \
#                designer.h \
#                syntax.h \
#                cutil.h \
#                dlgopt.h \
#                dlgnewprj.h \
#                dlgsearchreplace.h \
#                dlginsertFunction.h \
#                dlgpastewidget.h \
#                ../pvbrowser/dlgtextbrowser.h \
#                dlgdaemon.h \
#                dlgproperty.h \
#                dlginsertwidget.h \
#                dlgeditlayout.h \
#                widgetgenerator.h \
#                generatepython.h \
#                generatephp.h \
#                generateperl.h \
#                generatetcl.h \
#                generatelua.h \
#                ui_dlgnewprj.h \
#                ui_dlgsearchreplace.h \
#                ui_dlginsertfunction.h \
#                ui_dlgpastewidget.h \
#                ../pvbrowser/webkit_ui_dlgtextbrowser.h \
#                ui_dlgdaemon.h \
#                ui_dlgproperty.h \
#                ui_dlginsertwidget.h \
#                ui_dlgeditlayout.h \
#                ../pvbrowser/opt.h \
#                ../pvbrowser/MyWidgets.h \
#                ../pvbrowser/qwtwidgets.h \
#                ../pvbrowser/qwtplotwidget.h \
#                ../pvbrowser/qdrawwidget.h \
#                ../pvbrowser/qimagewidget.h \
#                ../pvbrowser/pvglwidget.h \
#                ../pvbrowser/interpreter.h \
#                ../pvbrowser/pvserver.h

SOURCES     = compile_modbus.cpp
TARGET = mkmodbus
