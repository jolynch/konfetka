#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#endif
#ifndef WIKIBROWSER_H
#define WIKIBROWSER_H
#include "databackend.h"
#include "databackend/mlibdata.h"
#include <QWebView>
#include <QByteArray>

#include <QUrl>
#include <QString>
#include <QDesktopServices>
#include <QVariant>
#include <QByteArray>
#include <QProgressBar>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStack>

class WikiView:public QWidget {
	Q_OBJECT
	private :
		QPushButton * backButton;
		QPushButton * forwardButton;
		QProgressBar * progressBar;
		QHBoxLayout * hlayout;
		QVBoxLayout * vlayout;
		QPushButton * goHome;
		DataBackend * conn;
		int lastLoaded;
		QString wikiPath;
		QWebView * myWiki;

	public :
		WikiView(DataBackend * c,QWidget * parent,Qt::WindowFlags f);
		~WikiView();
		
	public slots:
		void goToHome();
		void parseUrl(int id,bool force=false);
	
};

#endif
