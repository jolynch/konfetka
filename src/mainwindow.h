#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "databackend.h"
#include "mainbar.h"
#include "rearpanel.h"
//#include "handle.h"
#include "panelcontroller.h"
#include "panel.h"
#include "playlistpanel.h"
#include "medialib.h"
#include "playlist.h"
#include "collectioneditor.h"
#include "minimode.h"
#include "filebrowser.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QShortcut>
#include <QRect>
#include <QWidgetAction>

namespace konfetka
	{
	enum windowState
		{rearPanel,playlistOnly,medialibOnly,playlist_medialib};
	}

class MainWindow:public QWidget
	{	Q_OBJECT
	private:
		bool locked;
		konfetka::windowState currentState;
		int screenW; int screenH; int height; int currentHeight; int step;
		int leftSide; int rightSide; int middle; int mlibHH; int playHH;
		int currentPlaylistPos; int currentMedialibPos;
		DataBackend * conn;
		QApplication * papa;
		MiniMode * minibar;
		RearPanel * rearpanel;
		MainBar * mainbar;
		//Handle * playlist_handle;
		//Panel * playlist;
		//Handle * medialib_handle;
		//Panel * medialib;
		MediaLib * medialibView;
		QGridLayout * layout;
		PlaylistPanel * playlistpanel;
		QTimer * animator;
		QSystemTrayIcon * icon;
		QWidgetAction * volumeLabel;
		QLabel * volLabel;
		QWidgetAction * volume;
		QSlider * slider;
	public:
		MainWindow(QApplication * a, QWidget * parent = 0, Qt::WindowFlags f = 0);
		//!Added by Joe to make it easier to get to the playlist and medialib.
		Playlist* thePlaylist();
		MediaLib* theMediaLib(); //! end
	public slots:
		void slotStayOnTop(bool);
		void slotHide();
		void slotQuit();

		//void slotPlaylistClicked();
		//void slotMedialibClicked();
		//void slotChangeState(konfetka::windowState,konfetka::windowState);
		
		void slotAnimationDone();

		void toggle();
		void slotResizeThis();
		void clickResponse(QSystemTrayIcon::ActivationReason reason);
		//void provideRefresh();
	
		void trayTool();
	signals:
		//void stateChanged(konfetka::windowState,konfetka::windowState);
		//void movePlaylist(int,int,int,int);
		//void resizePlaylist(int, int);
		//void resizeMedialib(int, int);
		//void moveMedialib(int,int,int,int);
	};

#endif
