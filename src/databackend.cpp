#ifndef DATABACKEND_CPP
#define DATABACKEND_CPP
#include "databackend.h"


DataBackend::DataBackend(QObject * parent, std::string name):XMMS2Interface(parent,name) {
mlibData=new MlibData(this);
plistData=new PlistData(this);
collData=new CollData(this);
}

QObject * DataBackend::getDataBackendObject(DataBackendType type)
	{
	switch(type)
		{
		case COLL:return collData;
		case PLIST:return plistData;
		case MLIB:return mlibData;
		}
	return NULL;
	}

#endif


