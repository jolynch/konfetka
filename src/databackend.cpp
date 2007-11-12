#ifndef DATABACKEND_CPP
#define DATABACKEND_CPP
#include "databackend.h"


#include "databackend/plistdata.h"
#include "databackend/mlibdata.h"
#include "databackend/colldata.h"


DataBackend::DataBackend(QObject * parent, std::string name):XMMS2Interface(parent,name) {
mlibData=new MlibData(this);
collData=new CollData(this);
plistData=new PlistData(this);
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


