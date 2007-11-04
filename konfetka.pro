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
system(cp data/stylesheet $$KONFPATH/stylesheet)
system(cp data/stylesheet.buttons $$KONFPATH/stylesheet.buttons)
system(cp -r data/eqPresets $$KONFPATH/eqPresets) 

# Input
HEADERS += src/mainbar.h src/databackend.h src/databackend/mlibdata.h src/databackend/plistdata.h src/databackend/colldata.h src/databackend/xmms2interface.h src/databackend/xmmsqt4.h src/rearpanel.h src/equalizer.h src/visualization.h src/visband.h src/mainwindow.h src/options.h src/wikibrowser.h src/imagehttp.h src/lyricbrowser.h src/albumart.h src/handle.h src/panel.h src/medialib.h src/minimode.h src/playlistpanel.h src/collectionchooser.h src/infoeditor.h src/collectioneditor.h src/complexeditor.h src/playlist.h src/basicvis.h src/songpositionslider.h src/panelcontroller.h src/layoutpanel.h 
SOURCES += src/main.cpp src/mainbar.cpp src/databackend.cpp src/databackend/mlibdata.cpp src/databackend/plistdata.cpp src/databackend/colldata.cpp src/databackend/xmms2interface.cpp src/databackend/xmmsqt4.cpp src/rearpanel.cpp src/equalizer.cpp  src/visualization.cpp src/visband.cpp src/mainwindow.cpp src/options.cpp src/wikibrowser.cpp src/imagehttp.cpp src/lyricbrowser.cpp src/albumart.cpp src/handle.cpp src/panel.cpp src/medialib.cpp src/playlistpanel.cpp  src/collectionchooser.cpp src/infoeditor.cpp src/collectioneditor.cpp src/complexeditor.cpp src/playlist.cpp src/basicvis.cpp src/minimode.cpp src/songpositionslider.cpp src/panelcontroller.cpp

RESOURCES = konfetka.qrc

