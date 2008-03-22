#ifndef PANEL_CPP
#define PANEL_CPP
#include "panel.h"

Panel::Panel(QWidget * parent, QString text, QWidget * a):QWidget(parent)
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

void Panel::init(int pos, bool side)
	{
	rightSide=side;
	position=pos;
	if(!rightSide){vposition=0;}
	else{vposition=PBG_HEIGHT;}
	this->move(position-PBG_WIDTH/2,vposition);
	resizeAttached();
	}

void Panel::mv(int pos)
	{this->move(pos-PBG_WIDTH/2,vposition);}

void Panel::showAttached()
	{attached->show();}
void Panel::hideAttached()
	{attached->hide();}

int Panel::getPosition()
	{return position;}
int Panel::getVPosition()
	{return vposition;}
bool Panel::isRightSide() 
	{return rightSide;}
bool Panel::isLocked() 
	{return locked;}
QWidget * Panel::attachedWidget()
	{return attached;}

void Panel::mouseDoubleClickEvent ( QMouseEvent * event )
	{
	if(!locked)
		emit timerClicked(this);
	}

void Panel::mouseMoveEvent ( QMouseEvent * event )
	{
	if(dragging&&!locked)
		emit handleDraggedTo(this,event->globalX());
	}

void Panel::mousePressEvent ( QMouseEvent * event )
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

void Panel::mouseReleaseEvent ( QMouseEvent * event )
	{
	if(locked) return;
	if(clicked)
		emit timerClicked(this);
	else
		emit released();
	clicked=false;
	dragging=false;
	}

void Panel::wheelEvent ( QWheelEvent * event )
	{
	if(locked) return;
	if(event->delta()>0) emit scrolledUp(this);
	if(event->delta()<0) emit scrolledDown(this);
	}

void Panel::moveEvent ( QMoveEvent * event )
	{
	if(locked) return;
	position=event->pos().x()+PBG_WIDTH/2;
	update();
	resizeAttached();
	}

void Panel::hide()
	{
	((QWidget *)(this))->hide();
	aFrame->hide();
	}

void Panel::show()
	{
	aFrame->show();
	((QWidget *)(this))->show();
	}

void Panel::lock()
	{locked=true; update();}

void Panel::unlock()
	{locked=false; update();}

void Panel::resizeAttached()
	{
	QWidget * pa=(QWidget *)parent();
	if(rightSide)
		aFrame->move(position+PBG_WIDTH/2,0);
	else 
		aFrame->move(0,0);
	int width=position-PBG_WIDTH/2;
	if(rightSide)width=pa->width()-(position+PBG_WIDTH/2);
	aFrame->resize(width,PBG_HEIGHT*2);
	}

void Panel::clickTimeout()
	{
	if(locked) return;
	if(clicked)
		{
		clicked=false;
		dragging=true;
		}
	}

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
