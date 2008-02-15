#ifndef NICELABEL_H
#define NICELABEL_H
#include "databackend.h"
#include <QLabel>
#include <QMouseEvent>

class NiceLabel:public QLabel{	
Q_OBJECT
	private:
	int x; int pressPos; int id; int ms;
	bool pressed;
	
	public:
	NiceLabel(int speed=50,QWidget * parent = 0, Qt::WindowFlags f = 0);
	void mousePressEvent(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent * event);
	void mouseMoveEvent(QMouseEvent * event);
	void paintEvent(QPaintEvent *);
	void resizeEvent(QResizeEvent *);
	void timerEvent(QTimerEvent* event);
	void showEvent(QShowEvent * event);
	void hideEvent(QHideEvent * event);
	
	public slots:
	void slotScroll(int amount = -1);
};
#endif


