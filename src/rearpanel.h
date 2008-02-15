#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#endif
#ifndef REARPANEL_H
#define REARPANEL_H
#include "databackend.h"
#include "databackend/mlibdata.h"
#include "equalizer.h"
#include "visualization.h"
#include "options.h"
#include "wikibrowser.h"
#include "lyricbrowser.h"
#include "albumart.h"
#include "infoeditor.h"
#include "basicvis.h"
#include "contextinfo.h"
#include <string>
#include <cstdlib>
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>
#include <QTabWidget>
#include <QScrollArea>
#include <QString>
#include <QUrl>
#include <QProcess>
#include <QFile>
#include <QTextStream>

class RearPanel: public QWidget
	{	Q_OBJECT
	private:
		/*JOEY*/QRect deskRect;/*DONE*/
		QWidget * p;
		DataBackend * conn;
		ContextInfo * contextInfo;
		QString * finalString;
		QLabel * songInfo;
		AlbumArt * albumArt;
		QTabWidget * netInfo;
		QLabel * logo;
		QLabel * authors;
		QLabel * random;
		QWidget * info;
		QGridLayout * infoLayout;
		WikiView * wiki;
		LyricBrowser * lyrics;

		Visualization * visualization;
		Equalizer * equalizer;
		InfoEditor * infoeditor;
		QGridLayout * layout;
		BasicVis * newVis;

		QLabel * getRandom(bool fortune);
	public:
		RearPanel(DataBackend * c,QRect deskRect,QWidget * parent=0,Qt::WindowFlags f = 0);
		~RearPanel();
		AlbumArt* getAlbumArt();
	public slots:
		void updateStatus(Xmms::Playback::Status);
		void updateSong(int id);
		//ADDED BY JOEY FOR OPTIONS
		void createNewVis();
	signals:
		void sigUpdateHeaders(QList<QString>);
		void sigSearchWhat(int);
		void sigDoubleClick(bool);
		//DONE
	};
#endif
