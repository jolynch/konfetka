#ifndef IMAGEHTTP_CPP
#define IMAGEHTTP_CPP
#include "imagehttp.h"
ImageHttp::ImageHttp(QString host,QObject * parent):QHttp(host,80,parent)
	{
	h=new QString(host);
	requestID=-1;
	}

ImageHttp::~ImageHttp()
	{
	delete h;
	delete &loop;
	}

QByteArray ImageHttp::getStuff(const QString & what)
	{
	connect(this,SIGNAL(requestFinished(int,bool)),SLOT(finished(int,bool)));
	QHttpRequestHeader header("GET", what);
	header.setValue("Host", *h);
	header.setValue("User-Agent", "Qt-Integrated-Browser/0.01 Qt/4.3");
	requestID=this->request(header);
	loop.exec();
	QByteArray out=this->readAll();
	return out;
	}

void ImageHttp::finished(int id, bool err)
	{
	if(id!=requestID)
		return;
	loop.exit();
	}
#endif
