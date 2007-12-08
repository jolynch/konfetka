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
	QHttp *http;
	QBuffer imageBuffer;
	QBuffer xmlBuffer;
	int httpGetId;
	int numToGet;
	QString imageUrl;
	QDomNodeList allCovers;
	int id;
	bool toReflect;
	
	//Rendering Related
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
	bool gotInformation(const Xmms::bin& res); // Xmms2 bindata calls
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
	void makeRequest();
	
	signals:
	void newPixmap(QPixmap);
	};

#endif


