#-------------------------------------------------
#
# Project created by QtCreator 2016-08-25T19:49:56
#
#-------------------------------------------------

QT       += core gui sql xml

CONFIG+=qaxcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LoginDlg
TEMPLATE = app

SOURCES += main.cpp\
        logindlg.cpp \
    mainform.cpp \
    showinforlabel.cpp \
    goodsnumdelegate.cpp \
    collectdlg.cpp \
    choicegoodsdlg.cpp \
    maskwidget.cpp \
    addpurchasedlg.cpp \
    supplierdlg.cpp \
    addsupplierdlg.cpp \
    navmodel.cpp \
    navdelegate.cpp \
    histogramview.cpp \
    adduserdlg.cpp \
    modifyuserdlg.cpp \
    addgoodtypedlg.cpp

HEADERS  += logindlg.h \
    mainform.h \
    connection.h \
    showinforlabel.h \
    goodsnumdelegate.h \
    collectdlg.h \
    choicegoodsdlg.h \
    maskwidget.h \
    addpurchasedlg.h \
    supplierdlg.h \
    addsupplierdlg.h \
    navmodel.h \
    navdelegate.h \
    histogramview.h \
    adduserdlg.h \
    modifyuserdlg.h \
    addgoodtypedlg.h

FORMS    += logindlg.ui \
    mainform.ui \
    collectdlg.ui \
    choicegoodsdlg.ui \
    addpurchasedlg.ui \
    supplierdlg.ui \
    addsupplierdlg.ui \
    adduserdlg.ui \
    modifyuserdlg.ui \
    addgoodtypedlg.ui

RESOURCES += \
    MyImages.qrc

RC_FILE += ico.rc
