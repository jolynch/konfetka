#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#endif
#ifndef WEBPANEL_H
#define WEBPANEL_H
#include "databackend.h"
#include "databackend/mlibdata.h"
#include "layoutpanel.h"

#include <QWebView>
#include <QWebPage>
#include <QWebFrame>

#include <QTabWidget>

#include <QByteArray>
#include <QUrl>
#include <QString>
#include <QVariant>
#include <QProgressBar>
#include <QPushButton>
#include <QGridLayout>
#include <QShortcut>
#include <QLineEdit>
#include <QLabel>
#include <QIcon>

#include "services/shoutcast.h"

class WebPanel:public LayoutPanel {
	Q_OBJECT
	private :
 		QProgressBar * progressBar;
		QLabel * info;
		QGridLayout * layout;
		QTabWidget * tab;
		
		//Services
		Shoutcast * shout;

	public :
		WebPanel(DataBackend * c,QWidget * parent,Qt::WindowFlags f=0);
		~WebPanel();
		void setLayoutSide(bool right_side);
		
		bool registerService(WebService * service);
	public slots:
		void showProgress(int,int);
		void showProgress(qint64,qint64);
		void updateInfo(int);


};

#endif
