#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#include <xmmsclient/xmmsclient.h>
#endif
#ifndef VISUALIZATION_H
#define VISUALIZATION_H
#include "databackend.h"
#include "visband.h"
#include <QWidget>
#include <QHBoxLayout>

class Visualization:public QWidget
	{	Q_OBJECT
	private:
		unsigned int height;
		DataBackend * conn;
		QHBoxLayout * layout;
		VisBand * b0;
		VisBand * b1;
		VisBand * b2;
		VisBand * b3;
		VisBand * b4;
		VisBand * b5;
		VisBand * b6;
		VisBand * b7;
		VisBand * b8;
		VisBand * b9;
		VisBand * b10;
		VisBand * b11;
		VisBand * b12;
		VisBand * b13;
		VisBand * b14;
		VisBand * b15;
		VisBand * b16;
		VisBand * b17;
	public:
		Visualization(DataBackend * c,QWidget * parent = 0, Qt::WindowFlags f = 0);
		void mousePressEvent ( QMouseEvent * event );
		bool getSignal(const Xmms::List<unsigned int>& list);
	};

#endif
