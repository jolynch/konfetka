#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#endif
#ifndef ALBUMART_H
#define ALBUMART_H
#include "databackend.h"
#include "databackend/mlibdata.h"
#include <QtNetwork>
#include <QtGui>
#include <QDomDocument>
#include <iostream>
#include <string>
#include <math.h>
#include <QSettings>
#include <QCryptographicHash>
class AlbumArt: public QWidget
	{
	Q_OBJECT
	private:
	QString curAlbumArtPath, artXmlPath, artHashStorePath;
	QHBoxLayout * layout;
	QProgressBar * progress;
	QIcon * currentArtIcon;
	QLabel * currentArtLabel;
	QHttp *http;
	QFile *xmlFile;
	QFile *imageFile;
	QString album;
	QString artist;
	QString combo;
	QString * albumPic;
	int httpGetId,numBad;
	int numToGet;
     	bool httpRequestAborted, paintFix;
	QString imageUrl;
	QDomNodeList allCovers;
	int id;
	QHash<int,QString> knownUrls;
	bool fromThis;
	bool toReflect;
	QPixmap left;
	QPixmap right;
	QPixmap center;
	QPixmap bottom;
	
	QTimer timeout;

	DataBackend * conn;

	public:
		AlbumArt(DataBackend * c);
		~AlbumArt();
		void paintEvent(QPaintEvent *);
	public slots:
	void fetchXML(int);
	void fetchImage(bool);
	void setImage(bool);
	void slotReflect(bool);

	//Menuing slots
	void mouseReleaseEvent(QMouseEvent*);
	void getPrevCover();
	void getNextCover();
	void getOrigCover();
	void saveCoverToFile();
	void makeRequest();
	
	signals:
	void newPixmap(QPixmap);
	//void mousePressEvent( QMouseEvent * event )
	};

#endif


