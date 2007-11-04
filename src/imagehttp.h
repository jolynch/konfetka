#ifndef IMAGEHTTP_H
#define IMAGEHTTP_H
#include <QHttp>
#include <QEventLoop>
#include <QBuffer>
#include <QByteArray>
#include <iostream>

class ImageHttp: public QHttp
{
	Q_OBJECT
	public:
		ImageHttp(QString host,QObject * parent = 0);
		QByteArray getStuff(const QString & what);
		~ImageHttp();
	private slots:
		void finished(int idx, bool err);
	private:
		QString * h;
		int requestID;
		QEventLoop loop;
};
#endif
