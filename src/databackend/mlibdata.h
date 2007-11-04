#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#endif
#ifndef MLIBDATA_H
#define MLIBDATA_H
#include "../databackend.h"
#include <QObject>
#include <QString>
#include <QVariant>
#include <string>


class MlibData:public QObject
	{	Q_OBJECT
	///MLIB DATA
	///WHATEVER IS NEEDED
	private:
	DataBackend * conn;
	public:
		MlibData(DataBackend *,QObject * parent = 0);
		QVariant getInfo(std::string property, uint id);
		QVariant getInfo(QString property, uint id);
	};

#endif



