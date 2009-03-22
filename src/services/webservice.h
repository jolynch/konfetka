#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#endif
#ifndef WEBSERVICE
#define WEBSERVICE
#include "../databackend.h"
#include "../databackend/mlibdata.h"

#include <iostream>
#include <QWidget>
#include <QString>

class WebService:public QWidget
	{	Q_OBJECT
		
		public:
			WebService(DataBackend * c, QWidget * p=0, Qt::WindowFlags f=0):QWidget(p,f) {};
			
			virtual QString getInfo(){return QString("Default Web Service");}
			virtual QString getServiceName() {return QString("Default Web Service");}
			virtual ~WebService() {};
		
		signals:
			void progress(int); //These ints should really be between 0 and 100
			void progress(int,int); //For done/total signals
			void progress(qint64,qint64); //For done/total signals
	};
#endif
