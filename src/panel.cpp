#ifndef PANEL_CPP
#define PANEL_CPP
#include "panel.h"
#include <iostream>

Panel::Panel(QWidget * parent, QString text_, QWidget * attached_):QWidget(parent)
	{
	x=-1; y=-1;
	text=text_;
	setFixedHeight(PBG_HEIGHT);
	setFixedWidth(PBG_WIDTH);
	attached=attached_;
	f=new QFrame((QWidget *)(this->parent()));
	fl=new QGridLayout();
	fl->addWidget(attached);
	fl->setAlignment(Qt::AlignCenter);
	fl->setMargin(0);
	f->setLayout(fl);
	f->setAutoFillBackground(true);
	f->setBackgroundRole(QPalette::Window);
	f->setFrameStyle(QFrame::StyledPanel|QFrame::Sunken);
	locked=false; dragging=false; timerOn=false;
	animator=new QTimer();
	animator->setInterval(PT_INTERVAL);
	menu=new QMenu();
	clicktimer=new QTimer();
	clicktimer->setSingleShot(true);
	clicktimer->setInterval(100);
	QObject::connect(clicktimer,SIGNAL(timeout()),this,SLOT(click_timeout()));
	menu->addAction("Next panel",this,SLOT(up_press()));
	menu->addAction("Previous panel",this,SLOT(down_press()));
	menu->addAction("Toggle lock",this,SLOT(lock_press()));
	this->hide();
	clicked=false;
	}

LayoutPanel * Panel::getLayoutPanel()
	{return ((LayoutPanel *)attached);}

bool Panel::isLocked(){return locked;}

void Panel::setSide(bool right_side)
	{
	sideR=right_side;
	if(x==-1&&y==-1)
		{
		if(!sideR)
			{x=0;y=0;}
		else
			{x=((QWidget *)parent())->width()-PBG_WIDTH; y=PBG_HEIGHT;}
		move(x,y);
		resizeAttached();
		}
	}
	

void Panel::setSide_DOUBLESIDEPANEL(bool right_side)
	{
	if(!right_side)
		{y=0;}
	else
		{y=PBG_HEIGHT;}
	}

void Panel::mouseDoubleClickEvent ( QMouseEvent * event )
	{
	if(locked) return;
	QObject::connect(animator,SIGNAL(timeout()),this,SLOT(timer_ping()));
	animator->start();
	timerOn=true;clicktimer->stop();
	attached->hide();
	}

void Panel::mouseMoveEvent ( QMouseEvent * event )
	{
	if(locked) return;
	if(!dragging) return;
	QPoint newP=event->globalPos();
	emit(requestMove(false,newP.x(),y,sideR,this));
	this->show();
	}

void Panel::mousePressEvent ( QMouseEvent * event )
	{clicked=true;
	if(event->button()==Qt::RightButton)
		menu->exec(event->globalPos());
	if(locked) return;
	if(event->button()==Qt::LeftButton)
		{
		clicktimer->start();
		dragging=true;
		lastP=event->globalPos();
		}
	}

void Panel::mouseReleaseEvent ( QMouseEvent * event )
	{clicked=false;
	if(dragging)
		{dragging=false;}
	show();
	emit(released(this));
	}

void Panel::click_timeout()
	{
	if(clicked) return;
	const QPoint foo(0,0);
	mouseDoubleClickEvent(new QMouseEvent(QEvent::MouseButtonDblClick,foo,Qt::RightButton,Qt::RightButton,0));}

void Panel::wheelEvent ( QWheelEvent * event )
	{
	if(locked) return;
	if(event->delta()>0) up_press();
	if(event->delta()<0) down_press();
	}

void Panel::moveEvent ( QMoveEvent * event )
	{
	x=event->pos().x();
	resizeAttached();
	}

void Panel::up_press()
	{if(locked) return;
	emit(cycle(true));}

void Panel::down_press()
	{if(locked) return;
	emit(cycle(false));}

void Panel::lock_press()
	{locked = !locked; update();}

void Panel::timer_ping()
	{emit(requestMove(true,PSLIDE_DELTA,y,sideR,this));}

void Panel::stop()
	{
	//if(!timerOn) return;
	animator->disconnect();
	animator->stop();
	timerOn=false;
	attached->show();
	}

void Panel::resizeAttached()
	{
	int xPos=x;
	QWidget * pa=(QWidget *)parent();
	if(sideR)
		{xPos+=PBG_WIDTH;
		f->move(xPos,pa->y());}
	else 
		f->move(pa->x(),pa->y());
	int width=x;
	if(sideR)width=pa->width()-(x+PBG_WIDTH);
	f->resize(width,PBG_HEIGHT*2);
	f->show();
	}

void Panel::hide()
	{
	active=false;
	((QWidget *)(this))->hide();
	f->hide();
	}

void Panel::show()
	{
	active=true;
	f->show();
	((QWidget *)(this))->show();
	}

bool Panel::isActive()
	{return active;}

 //!here be hardcoding
void Panel::paintEvent ( QPaintEvent * event )
	{
	QPainter painter(this);
	painter.save();
	painter.setRenderHint(QPainter::Antialiasing);
	QLinearGradient gradient;

	//Rounded corners
	//curveFactor >=1
	int x, x2, curveFactor;
	curveFactor=2; x=0; x2=PBG_WIDTH/curveFactor;
		if(sideR) {
		x=PBG_WIDTH/curveFactor;
		x2=0;
		}
	QPainterPath path;
	path.setFillRule(Qt::WindingFill);
	path.moveTo(curveFactor*x,0);
	path.quadTo(curveFactor*x2,0,curveFactor*x2,PBG_WIDTH/curveFactor);
	path.lineTo(curveFactor*x2,(PBG_HEIGHT-PBG_WIDTH/curveFactor));
	path.quadTo(curveFactor*x2,PBG_HEIGHT,curveFactor*x,PBG_HEIGHT);
	path.lineTo(curveFactor*x,PBG_HEIGHT);
	path.lineTo(curveFactor*x,0);
	path.closeSubpath();
	//end rounded corners

	if(sideR)
		{
		gradient=QLinearGradient(PBG_WIDTH,0,0,0);
		gradient.setColorAt(0,QColor::fromRgb(157,178,190));
		gradient.setColorAt(1,QColor::fromRgb(157,178,190,127));
		}
	else
		{
		gradient=QLinearGradient(0,0,PBG_WIDTH,0);
		gradient.setColorAt(0,QColor::fromRgb(110,162,186));
		gradient.setColorAt(1,QColor::fromRgb(110,162,186,127));
		}
	painter.setPen(Qt::NoPen);
	QBrush b(gradient);
		if(locked) 
		b = QBrush(QColor(161,161,161,225));
	painter.setBrush(b);
	//Draw the path
	painter.fillPath(path,b);
	painter.setPen(Qt::black);
	painter.setFont(QFont("Monospace",PTEXT_SIZE,QFont::Bold));
	painter.drawText(QRectF((PBG_WIDTH-PTEXT_SIZE)/2.0,0.0,PTEXT_SIZE,PBG_HEIGHT),
			Qt::AlignCenter|Qt::TextWrapAnywhere,text);
	painter.restore();
	}

void Panel::allowXMove(int x__)
	{move(x__,y);}
#endif
