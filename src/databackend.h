#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#include <xmmsclient/xmmsclient.h>
#endif
#ifndef DATABACKEND_H
#define DATABACKEND_H
#include "databackend/xmms2interface.h"
#include "databackend/xmmsqt4.h"
#include "databackend/plistdata.h"
#include "databackend/mlibdata.h"
#include "databackend/colldata.h"

typedef uint DataBackendType;

class DataBackend:public XMMS2Interface {
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
};

#endif

