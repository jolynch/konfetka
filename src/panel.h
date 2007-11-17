#ifndef PANEL_H
#define PANEL_H
#include <QWidget>
#include <QLabel>
#include <QString>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPixmap>
#include <QTimer>
#include <QMenu>
#include <QPoint>
#include <QPainter>
#include <QPaintEngine>
#include <QRegion>
#include <QFrame>
#include <QGridLayout>

#include <QLinearGradient>
#include <QBrush>
#include <QFont>
#include <QRectF>

///CONSTANTS
#define PBG_WIDTH 20
#define PBG_HEIGHT 188
#define PSLIDE_DELTA 100
#define PT_INTERVAL 20
///END CONSTANTS

class Panel:public QWidget//public QLabel
	{	Q_OBJECT
	private:
		///DIFFERENT TEXT SIZES
		int PTEXT_SIZE;
		///END
		int y, x; //xR=x+PBG_WIDTH
		bool sideR; //false=left, true=right
		bool locked, dragging, timerOn, active, clicked;
		QWidget * attached;
		QTimer * animator;
		QTimer * clicktimer;
		QMenu * menu;
		QPoint lastP;
		QFrame * f;
		QGridLayout * fl;
		QString text;
	public:
		Panel(QWidget * parent, QString text_, QWidget * attached_);
		void setSide(bool right_side);
		bool isActive();
		bool isLocked();
		void allowXMove(int x__);
		void setSide_DOUBLESIDEPANEL(bool right_side);
	private:
		void mouseDoubleClickEvent ( QMouseEvent * event );
		void mouseMoveEvent ( QMouseEvent * event );
		void mousePressEvent ( QMouseEvent * event );
		void mouseReleaseEvent ( QMouseEvent * event );
		void wheelEvent ( QWheelEvent * event );
		void moveEvent ( QMoveEvent * event );
		void paintEvent ( QPaintEvent * event );
	private slots:
		void up_press();
		void down_press();
		void lock_press();
		void timer_ping();
		void click_timeout();
	private:
		int handleMaskPos;
		int attachedMaskPos;
		bool moveUp;
	public slots:
		void stop();
		void resizeAttached();
		void hide();
		void show();
	signals:
		void cycle(bool);
		void requestMove(bool,int,int,bool,Panel *);
		void released(Panel *);
	};
#endif
