#############################################################################
# Makefile for building: konfetka
# Generated by qmake (2.01a) (Qt 4.3.2) on: Sat Nov 3 20:12:04 2007
# Project:  konfetka.pro
# Template: app
# Command: /usr/bin/qmake-qt4 -unix -o Makefile konfetka.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT_SHARED -DQT_XML_LIB -DQT_GUI_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB
CFLAGS        = -pipe -g -I/usr/include/xmms2 -I/usr/include/xmms2 -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -pipe -g -I/usr/include/xmms2 -I/usr/include/xmms2 -Wall -W -Wno-unused-parameter -D_REENTRANT $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++ -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtNetwork -I/usr/include/qt4/QtNetwork -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtXml -I/usr/include/qt4/QtXml -I/usr/include/qt4 -I. -I.
LINK          = g++
LFLAGS        = 
LIBS          = $(SUBLIBS)  -L/usr/lib -lQtNetwork -lxmmsclient -lxmmsclient -lxmmsclient++ -lQtXml -lQtGui -lQtNetwork -lQtCore -lpthread
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/bin/qmake-qt4
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -sf
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = src/main.cpp \
		src/mainbar.cpp \
		src/databackend.cpp \
		src/databackend/mlibdata.cpp \
		src/databackend/plistdata.cpp \
		src/databackend/colldata.cpp \
		src/databackend/xmms2interface.cpp \
		src/databackend/xmmsqt4.cpp \
		src/rearpanel.cpp \
		src/equalizer.cpp \
		src/visualization.cpp \
		src/visband.cpp \
		src/mainwindow.cpp \
		src/options.cpp \
		src/wikibrowser.cpp \
		src/imagehttp.cpp \
		src/lyricbrowser.cpp \
		src/albumart.cpp \
		src/handle.cpp \
		src/panel.cpp \
		src/medialib.cpp \
		src/playlistpanel.cpp \
		src/collectionchooser.cpp \
		src/infoeditor.cpp \
		src/collectioneditor.cpp \
		src/complexeditor.cpp \
		src/playlist.cpp \
		src/basicvis.cpp \
		src/minimode.cpp \
		src/songpositionslider.cpp \
		src/panelcontroller.cpp moc_mainbar.cpp \
		moc_mlibdata.cpp \
		moc_plistdata.cpp \
		moc_colldata.cpp \
		moc_xmms2interface.cpp \
		moc_xmmsqt4.cpp \
		moc_rearpanel.cpp \
		moc_equalizer.cpp \
		moc_visualization.cpp \
		moc_visband.cpp \
		moc_mainwindow.cpp \
		moc_options.cpp \
		moc_wikibrowser.cpp \
		moc_imagehttp.cpp \
		moc_lyricbrowser.cpp \
		moc_albumart.cpp \
		moc_handle.cpp \
		moc_panel.cpp \
		moc_medialib.cpp \
		moc_minimode.cpp \
		moc_playlistpanel.cpp \
		moc_collectionchooser.cpp \
		moc_infoeditor.cpp \
		moc_collectioneditor.cpp \
		moc_playlist.cpp \
		moc_basicvis.cpp \
		moc_songpositionslider.cpp \
		moc_panelcontroller.cpp \
		qrc_konfetka.cpp
OBJECTS       = main.o \
		mainbar.o \
		databackend.o \
		mlibdata.o \
		plistdata.o \
		colldata.o \
		xmms2interface.o \
		xmmsqt4.o \
		rearpanel.o \
		equalizer.o \
		visualization.o \
		visband.o \
		mainwindow.o \
		options.o \
		wikibrowser.o \
		imagehttp.o \
		lyricbrowser.o \
		albumart.o \
		handle.o \
		panel.o \
		medialib.o \
		playlistpanel.o \
		collectionchooser.o \
		infoeditor.o \
		collectioneditor.o \
		complexeditor.o \
		playlist.o \
		basicvis.o \
		minimode.o \
		songpositionslider.o \
		panelcontroller.o \
		moc_mainbar.o \
		moc_mlibdata.o \
		moc_plistdata.o \
		moc_colldata.o \
		moc_xmms2interface.o \
		moc_xmmsqt4.o \
		moc_rearpanel.o \
		moc_equalizer.o \
		moc_visualization.o \
		moc_visband.o \
		moc_mainwindow.o \
		moc_options.o \
		moc_wikibrowser.o \
		moc_imagehttp.o \
		moc_lyricbrowser.o \
		moc_albumart.o \
		moc_handle.o \
		moc_panel.o \
		moc_medialib.o \
		moc_minimode.o \
		moc_playlistpanel.o \
		moc_collectionchooser.o \
		moc_infoeditor.o \
		moc_collectioneditor.o \
		moc_playlist.o \
		moc_basicvis.o \
		moc_songpositionslider.o \
		moc_panelcontroller.o \
		qrc_konfetka.o
DIST          = /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/debug.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/link_pkgconfig.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		konfetka.pro
QMAKE_TARGET  = konfetka
DESTDIR       = 
TARGET        = konfetka

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: konfetka.pro  /usr/share/qt4/mkspecs/linux-g++/qmake.conf /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/debug.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/link_pkgconfig.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf
	$(QMAKE) -unix -o Makefile konfetka.pro
/usr/share/qt4/mkspecs/common/g++.conf:
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
/usr/share/qt4/mkspecs/features/debug.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/link_pkgconfig.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/qt.prf:
/usr/share/qt4/mkspecs/features/unix/thread.prf:
/usr/share/qt4/mkspecs/features/moc.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
/usr/share/qt4/mkspecs/features/yacc.prf:
/usr/share/qt4/mkspecs/features/lex.prf:
qmake:  FORCE
	@$(QMAKE) -unix -o Makefile konfetka.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/konfetka1.0.0 || $(MKDIR) .tmp/konfetka1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/konfetka1.0.0/ && $(COPY_FILE) --parents src/mainbar.h src/databackend.h src/databackend/mlibdata.h src/databackend/plistdata.h src/databackend/colldata.h src/databackend/xmms2interface.h src/databackend/xmmsqt4.h src/rearpanel.h src/equalizer.h src/visualization.h src/visband.h src/mainwindow.h src/options.h src/wikibrowser.h src/imagehttp.h src/lyricbrowser.h src/albumart.h src/handle.h src/panel.h src/medialib.h src/minimode.h src/playlistpanel.h src/collectionchooser.h src/infoeditor.h src/collectioneditor.h src/complexeditor.h src/playlist.h src/basicvis.h src/songpositionslider.h src/panelcontroller.h src/layoutpanel.h .tmp/konfetka1.0.0/ && $(COPY_FILE) --parents konfetka.qrc .tmp/konfetka1.0.0/ && $(COPY_FILE) --parents src/main.cpp src/mainbar.cpp src/databackend.cpp src/databackend/mlibdata.cpp src/databackend/plistdata.cpp src/databackend/colldata.cpp src/databackend/xmms2interface.cpp src/databackend/xmmsqt4.cpp src/rearpanel.cpp src/equalizer.cpp src/visualization.cpp src/visband.cpp src/mainwindow.cpp src/options.cpp src/wikibrowser.cpp src/imagehttp.cpp src/lyricbrowser.cpp src/albumart.cpp src/handle.cpp src/panel.cpp src/medialib.cpp src/playlistpanel.cpp src/collectionchooser.cpp src/infoeditor.cpp src/collectioneditor.cpp src/complexeditor.cpp src/playlist.cpp src/basicvis.cpp src/minimode.cpp src/songpositionslider.cpp src/panelcontroller.cpp .tmp/konfetka1.0.0/ && (cd `dirname .tmp/konfetka1.0.0` && $(TAR) konfetka1.0.0.tar konfetka1.0.0 && $(COMPRESS) konfetka1.0.0.tar) && $(MOVE) `dirname .tmp/konfetka1.0.0`/konfetka1.0.0.tar.gz . && $(DEL_FILE) -r .tmp/konfetka1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all: moc_mainbar.cpp moc_mlibdata.cpp moc_plistdata.cpp moc_colldata.cpp moc_xmms2interface.cpp moc_xmmsqt4.cpp moc_rearpanel.cpp moc_equalizer.cpp moc_visualization.cpp moc_visband.cpp moc_mainwindow.cpp moc_options.cpp moc_wikibrowser.cpp moc_imagehttp.cpp moc_lyricbrowser.cpp moc_albumart.cpp moc_handle.cpp moc_panel.cpp moc_medialib.cpp moc_minimode.cpp moc_playlistpanel.cpp moc_collectionchooser.cpp moc_infoeditor.cpp moc_collectioneditor.cpp moc_playlist.cpp moc_basicvis.cpp moc_songpositionslider.cpp moc_panelcontroller.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_mainbar.cpp moc_mlibdata.cpp moc_plistdata.cpp moc_colldata.cpp moc_xmms2interface.cpp moc_xmmsqt4.cpp moc_rearpanel.cpp moc_equalizer.cpp moc_visualization.cpp moc_visband.cpp moc_mainwindow.cpp moc_options.cpp moc_wikibrowser.cpp moc_imagehttp.cpp moc_lyricbrowser.cpp moc_albumart.cpp moc_handle.cpp moc_panel.cpp moc_medialib.cpp moc_minimode.cpp moc_playlistpanel.cpp moc_collectionchooser.cpp moc_infoeditor.cpp moc_collectioneditor.cpp moc_playlist.cpp moc_basicvis.cpp moc_songpositionslider.cpp moc_panelcontroller.cpp
moc_mainbar.cpp: src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h \
		src/songpositionslider.h \
		src/mainbar.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/mainbar.h -o moc_mainbar.cpp

moc_mlibdata.cpp: src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h \
		src/databackend/mlibdata.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/databackend/mlibdata.h -o moc_mlibdata.cpp

moc_plistdata.cpp: src/databackend/plistdata.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/databackend/plistdata.h -o moc_plistdata.cpp

moc_colldata.cpp: src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h \
		src/databackend/colldata.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/databackend/colldata.h -o moc_colldata.cpp

moc_xmms2interface.cpp: src/databackend/xmmsqt4.h \
		src/databackend/xmms2interface.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/databackend/xmms2interface.h -o moc_xmms2interface.cpp

moc_xmmsqt4.cpp: src/databackend/xmmsqt4.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/databackend/xmmsqt4.h -o moc_xmmsqt4.cpp

moc_rearpanel.cpp: src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h \
		src/equalizer.h \
		src/visualization.h \
		src/visband.h \
		src/options.h \
		src/wikibrowser.h \
		src/imagehttp.h \
		src/lyricbrowser.h \
		src/albumart.h \
		src/infoeditor.h \
		src/basicvis.h \
		src/rearpanel.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/rearpanel.h -o moc_rearpanel.cpp

moc_equalizer.cpp: src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h \
		src/equalizer.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/equalizer.h -o moc_equalizer.cpp

moc_visualization.cpp: src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h \
		src/visband.h \
		src/visualization.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/visualization.h -o moc_visualization.cpp

moc_visband.cpp: src/visband.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/visband.h -o moc_visband.cpp

moc_mainwindow.cpp: src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h \
		src/mainbar.h \
		src/songpositionslider.h \
		src/rearpanel.h \
		src/equalizer.h \
		src/visualization.h \
		src/visband.h \
		src/options.h \
		src/wikibrowser.h \
		src/imagehttp.h \
		src/lyricbrowser.h \
		src/albumart.h \
		src/infoeditor.h \
		src/basicvis.h \
		src/panelcontroller.h \
		src/panel.h \
		src/layoutpanel.h \
		src/playlistpanel.h \
		src/playlist.h \
		src/collectionchooser.h \
		src/databackend/colldata.h \
		src/medialib.h \
		src/collectioneditor.h \
		src/complexeditor.h \
		src/minimode.h \
		src/mainwindow.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/mainwindow.h -o moc_mainwindow.cpp

moc_options.cpp: src/options.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/options.h -o moc_options.cpp

moc_wikibrowser.cpp: src/imagehttp.h \
		src/wikibrowser.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/wikibrowser.h -o moc_wikibrowser.cpp

moc_imagehttp.cpp: src/imagehttp.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/imagehttp.h -o moc_imagehttp.cpp

moc_lyricbrowser.cpp: src/lyricbrowser.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/lyricbrowser.h -o moc_lyricbrowser.cpp

moc_albumart.cpp: src/albumart.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/albumart.h -o moc_albumart.cpp

moc_handle.cpp: src/handle.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/handle.h -o moc_handle.cpp

moc_panel.cpp: src/panel.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/panel.h -o moc_panel.cpp

moc_medialib.cpp: src/collectioneditor.h \
		src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h \
		src/complexeditor.h \
		src/medialib.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/medialib.h -o moc_medialib.cpp

moc_minimode.cpp: src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h \
		src/basicvis.h \
		src/albumart.h \
		src/mainbar.h \
		src/songpositionslider.h \
		src/minimode.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/minimode.h -o moc_minimode.cpp

moc_playlistpanel.cpp: src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h \
		src/playlist.h \
		src/collectionchooser.h \
		src/layoutpanel.h \
		src/databackend/colldata.h \
		src/playlistpanel.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/playlistpanel.h -o moc_playlistpanel.cpp

moc_collectionchooser.cpp: src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h \
		src/collectionchooser.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/collectionchooser.h -o moc_collectionchooser.cpp

moc_infoeditor.cpp: src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h \
		src/infoeditor.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/infoeditor.h -o moc_infoeditor.cpp

moc_collectioneditor.cpp: src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h \
		src/complexeditor.h \
		src/collectioneditor.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/collectioneditor.h -o moc_collectioneditor.cpp

moc_playlist.cpp: src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h \
		src/playlist.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/playlist.h -o moc_playlist.cpp

moc_basicvis.cpp: src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h \
		src/basicvis.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/basicvis.h -o moc_basicvis.cpp

moc_songpositionslider.cpp: src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h \
		src/songpositionslider.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/songpositionslider.h -o moc_songpositionslider.cpp

moc_panelcontroller.cpp: src/panel.h \
		src/layoutpanel.h \
		src/panelcontroller.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/panelcontroller.h -o moc_panelcontroller.cpp

compiler_rcc_make_all: qrc_konfetka.cpp
compiler_rcc_clean:
	-$(DEL_FILE) qrc_konfetka.cpp
qrc_konfetka.cpp: konfetka.qrc \
		images/checkmark.png \
		images/groove.png \
		images/meter.png \
		images/song.png \
		images/tree_down.png \
		data/weirdline \
		images/checkback.png \
		images/tree_right.png \
		images/checkhover.png \
		images/xmms2Logo.png \
		images/handle.png \
		images/playlist.png \
		images/pause_button.png \
		images/dot.png \
		images/repeat_all.png \
		images/shuffle.png \
		images/open.png \
		images/sort.png \
		images/minimize.png \
		images/repeat_track.png \
		images/check.png \
		images/trashcan.png \
		images/save.png \
		images/un_minimize.png \
		images/no_album50.png \
		images/shuffledCards.png \
		images/minus.png \
		images/logoNoXmms2-secondTry.png \
		images/logomini.png \
		images/gradient_small.png \
		images/trashcan02.png \
		images/next_button.png \
		images/gradient.png \
		images/no_album150.png \
		images/play_button.png \
		images/medialib_handle.png \
		images/file_system.png \
		images/dot_small.png \
		images/toggle_button.png \
		images/cube1.png \
		images/shuffle2.png \
		images/plus.png \
		images/repeat_none.png \
		images/duplicate.png \
		images/person.png \
		images/back_button.png \
		images/stop_button.png \
		images/handle.png \
		images/quit_button.png \
		images/volume_button_sound.png \
		images/new.png \
		images/volume_button_NOsound.png \
		images/shuffleCube.png \
		images/playlist_handle.png
	/usr/bin/rcc -name konfetka konfetka.qrc -o qrc_konfetka.cpp

compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all:
compiler_uic_clean:
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean compiler_rcc_clean 

####### Compile

main.o: src/main.cpp src/mainwindow.h \
		src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h \
		src/mainbar.h \
		src/songpositionslider.h \
		src/rearpanel.h \
		src/equalizer.h \
		src/visualization.h \
		src/visband.h \
		src/options.h \
		src/wikibrowser.h \
		src/imagehttp.h \
		src/lyricbrowser.h \
		src/albumart.h \
		src/infoeditor.h \
		src/basicvis.h \
		src/panelcontroller.h \
		src/panel.h \
		src/layoutpanel.h \
		src/playlistpanel.h \
		src/playlist.h \
		src/collectionchooser.h \
		src/databackend/colldata.h \
		src/medialib.h \
		src/collectioneditor.h \
		src/complexeditor.h \
		src/minimode.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o src/main.cpp

mainbar.o: src/mainbar.cpp src/mainbar.h \
		src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h \
		src/songpositionslider.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o mainbar.o src/mainbar.cpp

databackend.o: src/databackend.cpp src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h \
		src/databackend/plistdata.h \
		src/databackend/mlibdata.h \
		src/databackend/colldata.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o databackend.o src/databackend.cpp

mlibdata.o: src/databackend/mlibdata.cpp src/databackend/mlibdata.h \
		src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o mlibdata.o src/databackend/mlibdata.cpp

plistdata.o: src/databackend/plistdata.cpp src/databackend/plistdata.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o plistdata.o src/databackend/plistdata.cpp

colldata.o: src/databackend/colldata.cpp src/databackend/colldata.h \
		src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o colldata.o src/databackend/colldata.cpp

xmms2interface.o: src/databackend/xmms2interface.cpp src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o xmms2interface.o src/databackend/xmms2interface.cpp

xmmsqt4.o: src/databackend/xmmsqt4.cpp src/databackend/xmmsqt4.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o xmmsqt4.o src/databackend/xmmsqt4.cpp

rearpanel.o: src/rearpanel.cpp src/rearpanel.h \
		src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h \
		src/equalizer.h \
		src/visualization.h \
		src/visband.h \
		src/options.h \
		src/wikibrowser.h \
		src/imagehttp.h \
		src/lyricbrowser.h \
		src/albumart.h \
		src/infoeditor.h \
		src/basicvis.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o rearpanel.o src/rearpanel.cpp

equalizer.o: src/equalizer.cpp src/equalizer.h \
		src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o equalizer.o src/equalizer.cpp

visualization.o: src/visualization.cpp src/visualization.h \
		src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h \
		src/visband.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o visualization.o src/visualization.cpp

visband.o: src/visband.cpp src/visband.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o visband.o src/visband.cpp

mainwindow.o: src/mainwindow.cpp src/mainwindow.h \
		src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h \
		src/mainbar.h \
		src/songpositionslider.h \
		src/rearpanel.h \
		src/equalizer.h \
		src/visualization.h \
		src/visband.h \
		src/options.h \
		src/wikibrowser.h \
		src/imagehttp.h \
		src/lyricbrowser.h \
		src/albumart.h \
		src/infoeditor.h \
		src/basicvis.h \
		src/panelcontroller.h \
		src/panel.h \
		src/layoutpanel.h \
		src/playlistpanel.h \
		src/playlist.h \
		src/collectionchooser.h \
		src/databackend/colldata.h \
		src/medialib.h \
		src/collectioneditor.h \
		src/complexeditor.h \
		src/minimode.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o mainwindow.o src/mainwindow.cpp

options.o: src/options.cpp src/options.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o options.o src/options.cpp

wikibrowser.o: src/wikibrowser.cpp src/wikibrowser.h \
		src/imagehttp.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o wikibrowser.o src/wikibrowser.cpp

imagehttp.o: src/imagehttp.cpp src/imagehttp.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o imagehttp.o src/imagehttp.cpp

lyricbrowser.o: src/lyricbrowser.cpp src/lyricbrowser.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o lyricbrowser.o src/lyricbrowser.cpp

albumart.o: src/albumart.cpp src/albumart.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o albumart.o src/albumart.cpp

handle.o: src/handle.cpp src/handle.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o handle.o src/handle.cpp

panel.o: src/panel.cpp src/panel.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o panel.o src/panel.cpp

medialib.o: src/medialib.cpp src/medialib.h \
		src/collectioneditor.h \
		src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h \
		src/complexeditor.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o medialib.o src/medialib.cpp

playlistpanel.o: src/playlistpanel.cpp src/playlistpanel.h \
		src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h \
		src/playlist.h \
		src/collectionchooser.h \
		src/layoutpanel.h \
		src/databackend/colldata.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o playlistpanel.o src/playlistpanel.cpp

collectionchooser.o: src/collectionchooser.cpp src/collectionchooser.h \
		src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o collectionchooser.o src/collectionchooser.cpp

infoeditor.o: src/infoeditor.cpp src/infoeditor.h \
		src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o infoeditor.o src/infoeditor.cpp

collectioneditor.o: src/collectioneditor.cpp src/collectioneditor.h \
		src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h \
		src/complexeditor.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o collectioneditor.o src/collectioneditor.cpp

complexeditor.o: src/complexeditor.cpp src/complexeditor.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o complexeditor.o src/complexeditor.cpp

playlist.o: src/playlist.cpp src/playlist.h \
		src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o playlist.o src/playlist.cpp

basicvis.o: src/basicvis.cpp src/basicvis.h \
		src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o basicvis.o src/basicvis.cpp

minimode.o: src/minimode.cpp src/minimode.h \
		src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h \
		src/basicvis.h \
		src/albumart.h \
		src/mainbar.h \
		src/songpositionslider.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o minimode.o src/minimode.cpp

songpositionslider.o: src/songpositionslider.cpp src/songpositionslider.h \
		src/databackend.h \
		src/databackend/xmms2interface.h \
		src/databackend/xmmsqt4.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o songpositionslider.o src/songpositionslider.cpp

panelcontroller.o: src/panelcontroller.cpp src/panelcontroller.h \
		src/panel.h \
		src/layoutpanel.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o panelcontroller.o src/panelcontroller.cpp

moc_mainbar.o: moc_mainbar.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_mainbar.o moc_mainbar.cpp

moc_mlibdata.o: moc_mlibdata.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_mlibdata.o moc_mlibdata.cpp

moc_plistdata.o: moc_plistdata.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_plistdata.o moc_plistdata.cpp

moc_colldata.o: moc_colldata.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_colldata.o moc_colldata.cpp

moc_xmms2interface.o: moc_xmms2interface.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_xmms2interface.o moc_xmms2interface.cpp

moc_xmmsqt4.o: moc_xmmsqt4.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_xmmsqt4.o moc_xmmsqt4.cpp

moc_rearpanel.o: moc_rearpanel.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_rearpanel.o moc_rearpanel.cpp

moc_equalizer.o: moc_equalizer.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_equalizer.o moc_equalizer.cpp

moc_visualization.o: moc_visualization.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_visualization.o moc_visualization.cpp

moc_visband.o: moc_visband.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_visband.o moc_visband.cpp

moc_mainwindow.o: moc_mainwindow.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_mainwindow.o moc_mainwindow.cpp

moc_options.o: moc_options.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_options.o moc_options.cpp

moc_wikibrowser.o: moc_wikibrowser.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_wikibrowser.o moc_wikibrowser.cpp

moc_imagehttp.o: moc_imagehttp.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_imagehttp.o moc_imagehttp.cpp

moc_lyricbrowser.o: moc_lyricbrowser.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_lyricbrowser.o moc_lyricbrowser.cpp

moc_albumart.o: moc_albumart.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_albumart.o moc_albumart.cpp

moc_handle.o: moc_handle.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_handle.o moc_handle.cpp

moc_panel.o: moc_panel.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_panel.o moc_panel.cpp

moc_medialib.o: moc_medialib.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_medialib.o moc_medialib.cpp

moc_minimode.o: moc_minimode.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_minimode.o moc_minimode.cpp

moc_playlistpanel.o: moc_playlistpanel.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_playlistpanel.o moc_playlistpanel.cpp

moc_collectionchooser.o: moc_collectionchooser.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_collectionchooser.o moc_collectionchooser.cpp

moc_infoeditor.o: moc_infoeditor.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_infoeditor.o moc_infoeditor.cpp

moc_collectioneditor.o: moc_collectioneditor.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_collectioneditor.o moc_collectioneditor.cpp

moc_playlist.o: moc_playlist.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_playlist.o moc_playlist.cpp

moc_basicvis.o: moc_basicvis.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_basicvis.o moc_basicvis.cpp

moc_songpositionslider.o: moc_songpositionslider.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_songpositionslider.o moc_songpositionslider.cpp

moc_panelcontroller.o: moc_panelcontroller.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_panelcontroller.o moc_panelcontroller.cpp

qrc_konfetka.o: qrc_konfetka.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o qrc_konfetka.o qrc_konfetka.cpp

####### Install

install_target: first FORCE
	@$(CHK_DIR_EXISTS) $(INSTALL_ROOT)/usr/local/bin/ || $(MKDIR) $(INSTALL_ROOT)/usr/local/bin/ 
	-$(INSTALL_PROGRAM) "$(QMAKE_TARGET)" "$(INSTALL_ROOT)/usr/local/bin/$(QMAKE_TARGET)"

uninstall_target:  FORCE
	-$(DEL_FILE) "$(INSTALL_ROOT)/usr/local/bin/$(QMAKE_TARGET)"
	-$(DEL_DIR) $(INSTALL_ROOT)/usr/local/bin/ 


install:  install_target  FORCE

uninstall: uninstall_target   FORCE

FORCE:

