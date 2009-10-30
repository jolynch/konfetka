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
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QList>

class AlbumArt: public QWidget {
	Q_OBJECT
	private:
	QNetworkAccessManager *manager;
	QMenu * menu;
	QBuffer imageBuffer;
	QBuffer xmlBuffer;
	int numToGet;
	QString imageUrl;
	QList<QString> allCovers;
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
	
	void setNoAlbum();

	public:
	AlbumArt(DataBackend * c);
	~AlbumArt();
	void paintEvent(QPaintEvent *);
	bool gotInformation(const Xmms::bin& res); // Xmms2 bindata calls
	bool sentInformation(const std::string& res);
	void dragEnterEvent(QDragEnterEvent * event);
	void dropEvent(QDropEvent *event);
	
	public slots:
	void fetchXML(int);
	void fetchImage(QNetworkReply * rep=NULL,bool force = 0);
	void setImage(QNetworkReply * rep=NULL);
	void processSettingsUpdate(QString,QVariant);

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


