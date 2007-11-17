#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#include <xmmsclient/xmmsclient.h>
#endif
#ifndef DATABACKEND_H
#define DATABACKEND_H
#include "databackend/xmms2interface.h"
#include "databackend/xmmsqt4.h"
#include <QObject>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QVariant>

typedef uint DataBackendType;

class DataBackend:public XMMS2Interface//, public QObject
	{	Q_OBJECT
	private:
	QObject * mlibData;
	QObject * plistData;
	QObject * collData;
	
	public:
	static const DataBackendType COLL	=0x001;
	static const DataBackendType PLIST	=0x010;
	static const DataBackendType MLIB	=0x100;
	
	DataBackend(QObject * parent, std::string name);
	QObject * getDataBackendObject(DataBackendType type);

	/**
	***All QSettings keys starting with "konfetka"
	***have their inital values sent out in qsettingsValueChanged
	**/
	void emitInitialQSettings();

	public slots:
	//Only call this from the options GUI, please.
	void changeAndSaveQSettings(QString name, QVariant newValue);

	signals:
	void qsettingsValueChanged(QString /*name*/, QVariant /*newValue*/);
	};

#endif

