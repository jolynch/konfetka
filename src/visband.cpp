#ifndef VISBAND_CPP
#define VISBAND_CPP
#include "visband.h"

VisBand::VisBand(QWidget * parent, Qt::WindowFlags f):QWidget(parent,f)
	{
	height=0;
	layout=new QVBoxLayout();
	l5=new QLabel();
	l5->setAutoFillBackground(true);
	l5->setFrameStyle(QFrame::StyledPanel);
	l5->setMaximumSize(10,10);
	l5->setMinimumSize(10,10);
	l4=new QLabel();
	l4->setAutoFillBackground(true);
	l4->setFrameStyle(QFrame::StyledPanel);
	l4->setMaximumSize(10,10);
	l4->setMinimumSize(10,10);
	l3=new QLabel();
	l3->setAutoFillBackground(true);
	l3->setFrameStyle(QFrame::StyledPanel);
	l3->setMaximumSize(10,10);
	l3->setMinimumSize(10,10);
	l2=new QLabel();
	l2->setAutoFillBackground(true);
	l2->setFrameStyle(QFrame::StyledPanel);
	l2->setMaximumSize(10,10);
	l2->setMinimumSize(10,10);
	l1=new QLabel();
	l1->setAutoFillBackground(true);
	l1->setFrameStyle(QFrame::StyledPanel);
	l1->setMaximumSize(10,10);
	l1->setMinimumSize(10,10);
	l0=new QLabel();
	l0->setAutoFillBackground(true);
	l0->setFrameStyle(QFrame::StyledPanel);
	l0->setMaximumSize(10,10);
	l0->setMinimumSize(10,10);
	layout->addWidget(l5);
	layout->addWidget(l4);
	layout->addWidget(l3);
	layout->addWidget(l2);
	layout->addWidget(l1);
	layout->addWidget(l0);
	layout->setMargin(3);
	layout->setAlignment(Qt::AlignCenter);
	this->setLayout(layout);
	}

VisBand::~VisBand()
	{
	delete l5;
	delete l4;
	delete l3;
	delete l2;
	delete l1;
	delete l0;
	delete layout;
	}

void VisBand::setHeight(unsigned int in)
	{
	QPalette blue (this->palette());
	blue.setColor(QPalette::Window, QColor(181, 181, 213, 127));
	QPalette grey (this->palette());
	switch(in)
	{
	case 6:
		l0->setPalette(blue);
		l1->setPalette(blue);
		l2->setPalette(blue);
		l3->setPalette(blue);
		l4->setPalette(blue);
		l5->setPalette(blue);
		break;
	case 5:
		l5->setPalette(grey);
		l0->setPalette(blue);
		l1->setPalette(blue);
		l2->setPalette(blue);
		l3->setPalette(blue);
		l4->setPalette(blue);
		break;
	case 4:
		l5->setPalette(grey);
		l4->setPalette(grey);
		l0->setPalette(blue);
		l1->setPalette(blue);
		l2->setPalette(blue);
		l3->setPalette(blue);
		break;
	case 3:
		l5->setPalette(grey);
		l4->setPalette(grey);
		l3->setPalette(grey);
		l0->setPalette(blue);
		l1->setPalette(blue);
		l2->setPalette(blue);
		break;
	case 2:
		l5->setPalette(grey);
		l4->setPalette(grey);
		l3->setPalette(grey);
		l2->setPalette(grey);
		l0->setPalette(blue);
		l1->setPalette(blue);
		break;
	case 1:
		l5->setPalette(grey);
		l4->setPalette(grey);
		l3->setPalette(grey);
		l2->setPalette(grey);
		l1->setPalette(grey);
		l0->setPalette(blue);
		break;
	case 0:
		l5->setPalette(grey);
		l4->setPalette(grey);
		l3->setPalette(grey);
		l2->setPalette(grey);
		l1->setPalette(grey);
		l0->setPalette(grey);
		break;
	}
	}

unsigned int VisBand::getHeight()
	{return height;}

#endif
