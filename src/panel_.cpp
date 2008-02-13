#ifndef PANEL__CPP
#define PANEL__CPP
#include "panel_.h"

Panel_::Panel_(QWidget * parent, QString text, QWidget * a):QWidget(parent)
	{
	this->setFixedHeight(PBG_HEIGHT);
	this->setFixedWidth(PBG_WIDTH);
	vposition=0; 
	position =PBG_WIDTH/2;
	rightSide=false;
	clicked=false;
	dragging=false;
	locked=false;
	name=text;
	attached=a;
	clickTimer=new QTimer();
	clickTimer->setSingleShot(true);
	clickTimer->setInterval(100);
	//frame setup. needs to hold the attached widget.
	aFrame=new QFrame((QWidget *)(this->parent()));
	QGridLayout * fl=new QGridLayout();
	fl->addWidget(attached);
	fl->setAlignment(Qt::AlignCenter);
	fl->setMargin(0);
	aFrame->setLayout(fl);
	aFrame->setAutoFillBackground(true);
	aFrame->setBackgroundRole(QPalette::Window);
	aFrame->setFrameStyle(QFrame::StyledPanel|QFrame::Sunken);

	connect(clickTimer,SIGNAL(timeout()),this,SLOT(clickTimeout()));
	}

void Panel_::init(int pos, bool side)
	{
	rightSide=side;
	position=pos;
	if(!rightSide){vposition=0;}
	else{vposition=PBG_HEIGHT;}
	this->move(position-PBG_WIDTH/2,vposition);
	}

int Panel_::getPosition()
	{return position;}
int Panel_::getVPosition()
	{return vposition;}
bool Panel_::isRightSide() 
	{return rightSide;}
bool Panel_::isLocked() 
	{return locked;}

void Panel_::mouseDoubleClickEvent ( QMouseEvent * event )
	{
	if(!locked)
		emit timerClicked(this);
	}

void Panel_::mouseMoveEvent ( QMouseEvent * event )
	{
	if(dragging&&!locked)
		emit handleDraggedTo(this,event->globalX());
	}

void Panel_::mousePressEvent ( QMouseEvent * event )
	{
	if(event->button()==Qt::RightButton)
		{
		emit rightClicked(this,event->globalX(),event->globalY());
		return;
		}
	if(locked) return;
	clicked=true;
	clickTimer->start();
	}

void Panel_::mouseReleaseEvent ( QMouseEvent * event )
	{
	if(locked) return;
	if(clicked)
		emit timerClicked(this);
	clicked=false;
	dragging=false;
	}

void Panel_::wheelEvent ( QWheelEvent * event )
	{
	if(locked) return;
	if(event->delta()>0) emit scrolledUp(this);
	if(event->delta()<0) emit scrolledDown(this);
	}

void Panel_::moveEvent ( QMoveEvent * event )
	{
	if(locked) return;
	position=event->pos().x()+PBG_WIDTH/2;
	update();
	resizeAttached();
	}

void Panel_::hide()
	{
	((QWidget *)(this))->hide();
	aFrame->hide();
	}

void Panel_::show()
	{
	aFrame->show();
	((QWidget *)(this))->show();
	}

void Panel_::lock()
	{locked=true; update();}

void Panel_::unlock()
	{locked=false; update();}

void Panel_::resizeAttached()
	{
	QWidget * pa=(QWidget *)parent();
	if(rightSide)
		aFrame->move(position+PBG_WIDTH/2,pa->y());
	else 
		aFrame->move(pa->x(),pa->y());
	int width=position-PBG_WIDTH/2;
	if(rightSide)width=pa->width()-(position+PBG_WIDTH/2);
	aFrame->resize(width,PBG_HEIGHT*2);
	}

void Panel_::clickTimeout()
	{
	if(locked) return;
	if(clicked)
		{
		clicked=false;
		dragging=true;
		}
	}

void Panel_::paintEvent ( QPaintEvent * event )
	{
	QPainter painter(this);
	painter.save();
	painter.setRenderHint(QPainter::Antialiasing);
	QLinearGradient gradient;

	//Rounded corners
	//curveFactor >=1
	int x, x2, curveFactor;
	curveFactor=2; x=0; x2=PBG_WIDTH/curveFactor;
		if(rightSide) {
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

	if(rightSide)
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
			Qt::AlignCenter|Qt::TextWrapAnywhere,name);
	painter.restore();
	}

#endif
