#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#endif
#ifndef LYRICBROWSER_H
#define LYRICBROWSER_H
#include "databackend.h"
#include "databackend/mlibdata.h"
#include <QTextBrowser>
#include <QByteArray>
#include <QHttp>
#include <QHttpResponseHeader>
#include <QUrl>
#include <QString>
#include <QDesktopServices>
#include <QList>

class LyricBrowser:public QTextBrowser
	{	Q_OBJECT
	private:
		QString * adress;
		int currentID;
		QHttp * browser;
		QString parse(QString in);
		DataBackend * conn;
	public:
		LyricBrowser(DataBackend * c);
		~LyricBrowser();
		void parseUrl(int id);
	public slots:
		void httpData(int,bool);
		void setSource(const QUrl &);
	};

#endif
