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
class AlbumArt: public QWidget {
	Q_OBJECT
	private:
	QString curAlbumArtPath, artXmlPath, artHashStorePath;
	QHttp *http;
	QFile *xmlFile;
	QBuffer imageBuffer;
	QString album;
	QString artist;
	int httpGetId,numBad;
	int numToGet;
     	bool httpRequestAborted, paintFix;
	QString imageUrl;
	QDomNodeList allCovers;
	int id;
	QHash<int,QString> knownUrls;
	bool fromThis;
	bool toReflect;
	
	//NEW STUFF
	bool hasAlbum;
	QPixmap left;
	QPixmap right;
	QPixmap center;
	QPixmap noAlbum;
	QString query;
	
	QTimer timeout;

	DataBackend * conn;
	MlibData * mlib;

	public:
	AlbumArt(DataBackend * c);
	~AlbumArt();
	void paintEvent(QPaintEvent *);
	bool gotInformation(const Xmms::bin& res);
	bool sentInformation(const std::string& res);
	public slots:
	void fetchXML(int);
	void fetchImage(bool,bool force = 0);
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


