#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#endif
#ifndef WIKIBROWSER_H
#define WIKIBROWSER_H
#include "imagehttp.h"
#include <QTextBrowser>
#include <QByteArray>
#include <QHttp>
#include <QHttpResponseHeader>
#include <QUrl>
#include <QString>
#include <QDesktopServices>
#include <QVariant>
#include <QByteArray>
#include <QImage>
#include <QProgressBar>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStack>

class WikiBrowser:public QTextBrowser
	{	Q_OBJECT
	private:
		int currentID;
		QString * wikiPath;
		ImageHttp * http;
		QString *pathProbFix;
		QString last;
		QStack<QString> bStack;
		QStack<QString> fStack;
	public:
		WikiBrowser(QWidget * parent);
		~WikiBrowser();
		void parseUrl(Xmms::PropDict info);
		QVariant loadResource ( int type, const QUrl & name );
		QByteArray data;
		void scroll(const QUrl & url);
		QHttp * browser;
	public slots:
		void httpData(int,bool);
		void setSource(const QUrl &);
		void slotBack();
		void slotForward();
	};

class WikiView:public QWidget {
	Q_OBJECT
	private :
		QPushButton * backButton;
		QPushButton * forwardButton;
		QProgressBar * progressBar;
		QHBoxLayout * hlayout;
		QVBoxLayout * vlayout;
		QString * artistHome;
		QPushButton * goHome;
	public :
		WikiView(QWidget * parent,Qt::WindowFlags f);
		~WikiView();
		WikiBrowser * myWiki;
		void setHome(Xmms::PropDict info);
	public slots:
		void updateProgress(int,int);
		void goToHome();
	
};

#endif
