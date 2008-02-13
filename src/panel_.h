#ifndef PANEL__H
#define PANEL__H
#include "layoutpanel.h"
#include <QWidget>
#include <QTimer>
#include <QLinearGradient>
#include <QBrush>
#include <QFont>
#include <QRectF>
#include <QFrame>
#include <QGridLayout>
#include <QMouseEvent>
#include <QPainter>

///CONSTANTS
#define PBG_WIDTH 20
#define PBG_HEIGHT 188
#define PSLIDE_DELTA 100
#define PT_INTERVAL 20
#define PTEXT_SIZE 8
///END CONSTANTS

class Panel_:public QWidget
	{	Q_OBJECT
	private:
		bool rightSide,clicked,dragging,locked;
		int vposition;
		int position; //rightbound=PBG_WIDTH/2+position
		QString name;
		QWidget * attached;
		QTimer * clickTimer;
		QMouseEvent * lastClick;
		QFrame * aFrame;
	public:
		Panel_(QWidget * parent, QString text, QWidget * a);
		void init(int pos, bool side);
		int getPosition();
		int getVPosition();
		bool isRightSide();
		bool isLocked();
		void mouseDoubleClickEvent ( QMouseEvent * event );
		void mouseMoveEvent ( QMouseEvent * event );
		void mousePressEvent ( QMouseEvent * event );
		void mouseReleaseEvent ( QMouseEvent * event );
		void wheelEvent ( QWheelEvent * event );
		void moveEvent ( QMoveEvent * event );
		void paintEvent ( QPaintEvent * event );
	public slots:
		void hide();
		void show();
		void lock();
		void unlock();
	private slots:
		void resizeAttached();
		void clickTimeout();
	signals:
		void handleDraggedTo(Panel_ *,int);
		void timerClicked(Panel_ *);
		void rightClicked(Panel_ *,int,int);
		void scrolledUp(Panel_ *);
		void scrolledDown(Panel_ *);
	};

#endif
