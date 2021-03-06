#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "databackend.h"
#include "databackend/panelcontroller.h"
#include "databackend/plistdata.h"
#include <QTreeView>

#include "mainbar.h"
#include "rearpanel.h"
#include "panel.h"
#include "collectionbrowser.h"
#include "options.h"
#include "webpanel.h"

#include "playlistpanel.h"

#include "medialib.h"
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
#include <QKeySequence>

namespace konfetka
	{
	enum windowState
		{rearPanel,playlistOnly,medialibOnly,Playlistmedialib};
	}

class MainWindow:public QWidget
	{	Q_OBJECT
	private:
		bool locked;
		konfetka::windowState currentState;
		int screenW; int screenH; int height; int currentHeight; int step;
		int leftSide; int rightSide; int middle; int mlibHH; int playHH;
		int currentPlaylistPos; int currentMedialibPos;
		QKeySequence minMaxGlobSC;
		DataBackend * conn;
		QApplication * papa;
		MiniMode * minibar;
		RearPanel * rearpanel;
		MainBar * mainbar;

		QTimer * animator;
		QSystemTrayIcon * icon;
		QWidgetAction * volumeLabel;
		QLabel * volLabel;
		QWidgetAction * volume;
		QSlider * slider;
	public:
		MainWindow(QApplication * a, QWidget * parent = 0, Qt::WindowFlags f = 0);

	public slots:
		void slotHide();
		void slotQuit();

		
		void slotAnimationDone();

		void toggle();
		void slotResizeThis();
		void clickResponse(QSystemTrayIcon::ActivationReason reason);

	
		void respondToConfigChange(QString name,QVariant value);
		void trayTool();
		
		void show();
		void hide();
	};

#endif
