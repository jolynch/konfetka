#ifndef MLIBDATA_CPP
#define MLIBDATA_CPP
#include "mlibdata.h"

MlibData::MlibData(DataBackend * conn,QObject * parent):QObject(parent) {
this->conn = conn;
}

QVariant MlibData::getInfo(QString property, uint id) {
	return QVariant(-1);
}
	
QVariant MlibData::getInfo(std::string property, uint id) {
	return getInfo(QString(property.c_str()),id);
}
#endif



