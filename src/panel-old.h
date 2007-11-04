#ifndef PANEL_H
#define PANEL_H
#include <QFrame>
#include <QGridLayout>
#include <QTimer>
//#include <QTimerEvent>
#include <iostream>
class Panel:public QFrame
	{	Q_OBJECT
	private:
		bool mlib;
//		int animatorId;
		QTimer * animator;
		QGridLayout * layout;
		QWidget * child;
		int currentPos;
		int destination;
		int heightVal;
		int amount;
		int yVal;
		int xVal;
	public:
		Panel(int x,int y,int width,int height,bool mlib,
							QWidget* c=0, QWidget* parent=0, Qt::WindowFlags f=0);
//		void timerEvent ( QTimerEvent * event );
	private slots:
		void moveHandle();
	public slots:
		void moveThis(int,int,int,int);
	};
#endif
