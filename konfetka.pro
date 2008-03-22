TEMPLATE = app
TARGET = konfetka
target.path = /usr/local/bin
DEPENDPATH += .
PKGCONFIG += xmms2-client xmms2-client-cpp
CONFIG += link_pkgconfig debug
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
INCLUDEPATH += /usr/include/qt4/QtNetwork
LIBS += -lQtNetwork
QT += xml
QT += network
INSTALLS += target

MOC_DIR = mocs
OBJECTS_DIR = build

KONFPATH=$$(XDG_CONFIG_HOME)
exists($$KONFPATH){
	KONFPATH=$$(XDG_CONFIG_HOME)/xmms2/clients/konfetka/
	}
else{
	message("$XDG_CONFIG_HOME not set. Using ~/.config")
	KONFPATH= ~/.config/xmms2/clients/konfetka/
	}


!exists($$KONFPATH){
	system(mkdir $$KONFPATH)
	}

system(cp data/art.xml $$KONFPATH/art.xml)
system(cp -r data/eqPresets $$KONFPATH/eqPresets)
system(cp -r data/helpFiles $$KONFPATH/helpFiles)

#Global Shortcut (from Psi ((http://psi-im.org) )

HEADERS += globalshortcut/globalshortcutmanager.h globalshortcut/globalshortcuttrigger.h
SOURCES += globalshortcut/globalshortcutmanager.cpp

unix:!mac {
	SOURCES += globalshortcut/globalshortcutmanager_x11.cpp
}
win32: {
	SOURCES += globalshortcut/globalshortcutmanager_win.cpp
}
mac: {
	SOURCES += globalshortcut/globalshortcutmanager_mac.cpp
}


# Konfetka

HEADERS += src/mainbar.h src/databackend.h src/databackend/mlibdata.h src/databackend/plistdata.h src/databackend/colldata.h src/databackend/xmms2interface.h src/databackend/xmmsqt4.h src/rearpanel.h src/equalizer.h src/mainwindow.h src/options.h src/wikibrowser.h src/imagehttp.h src/lyricbrowser.h src/albumart.h src/panel.h src/medialib.h src/minimode.h src/infoeditor.h src/basicvis.h src/songpositionslider.h src/databackend/panelcontroller.h src/layoutpanel.h src/filebrowser.h src/playlistpanel.h src/collectionbrowser.h src/nicelabel.h src/contextinfo.h src/panelview.h
SOURCES += src/main.cpp src/mainbar.cpp src/databackend.cpp src/databackend/mlibdata.cpp src/databackend/plistdata.cpp src/databackend/colldata.cpp src/databackend/xmms2interface.cpp src/databackend/xmmsqt4.cpp src/rearpanel.cpp src/equalizer.cpp src/mainwindow.cpp src/options.cpp src/wikibrowser.cpp src/imagehttp.cpp src/lyricbrowser.cpp src/albumart.cpp src/panel.cpp src/medialib.cpp src/infoeditor.cpp src/basicvis.cpp src/minimode.cpp src/songpositionslider.cpp src/databackend/panelcontroller.cpp src/filebrowser.cpp src/playlistpanel.cpp src/collectionbrowser.cpp src/nicelabel.cpp src/contextinfo.cpp src/panelview.cpp


RESOURCES = konfetka.qrc

