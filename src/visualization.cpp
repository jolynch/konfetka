#ifndef VISUALIZATION_CPP
#define VISUALIZATION_CPP
#include "visualization.h"

Visualization::Visualization(DataBackend* c,
						QWidget* parent, Qt::WindowFlags f):QWidget(parent, f)
	{
	height=0;
	conn=c;
	layout=new QHBoxLayout;
	b0=new VisBand(this,NULL);
	b1=new VisBand(this,NULL);
	b2=new VisBand(this,NULL);
	b3=new VisBand(this,NULL);
	b4=new VisBand(this,NULL);
	b5=new VisBand(this,NULL);
	b6=new VisBand(this,NULL);
	b7=new VisBand(this,NULL);
	b8=new VisBand(this,NULL);
	b9=new VisBand(this,NULL);
	b10=new VisBand(this,NULL);
	b11=new VisBand(this,NULL);
	b12=new VisBand(this,NULL);
	b13=new VisBand(this,NULL);
	b14=new VisBand(this,NULL);
	b15=new VisBand(this,NULL);
	b16=new VisBand(this,NULL);
	b17=new VisBand(this,NULL);
	layout->addWidget(b0);
	layout->addWidget(b1);
	layout->addWidget(b2);
	layout->addWidget(b3);
	layout->addWidget(b4);
	layout->addWidget(b5);
	layout->addWidget(b6);
	layout->addWidget(b7);
	layout->addWidget(b8);
	layout->addWidget(b9);
	layout->addWidget(b10);
	layout->addWidget(b11);
	layout->addWidget(b12);
	layout->addWidget(b13);
	layout->addWidget(b14);
	layout->addWidget(b15);
	layout->addWidget(b16);
	layout->addWidget(b17);
	layout->setSpacing(0);
	layout->setAlignment(Qt::AlignCenter);
	this->setLayout(layout);
//	conn->stats.signalVisualisationData()(Xmms::bind(&Visualization::getSignal, this));
	}

bool Visualization::getSignal(const Xmms::List<unsigned int>& list)
	{
	std::cout<<"in"<<std::endl;
	unsigned int foo=0;
	for(;list.isValid();++list)
		{foo=*list; std::cout<<foo<<std::endl;}
	std::cout<<"out"<<std::endl;
	return false;
	}

void Visualization::mousePressEvent( QMouseEvent * event )
	{
	height++;
	if(height>6)height=0;
	b0->setHeight(height);
	b1->setHeight(height);
	b2->setHeight(height);
	b3->setHeight(height);
	b4->setHeight(height);
	b5->setHeight(height);
	b6->setHeight(height);
	b7->setHeight(height);
	b8->setHeight(height);
	b9->setHeight(height);
	b10->setHeight(height);
	b11->setHeight(height);
	b12->setHeight(height);
	b13->setHeight(height);
	b14->setHeight(height);
	b15->setHeight(height);
	b16->setHeight(height);
	b17->setHeight(height);
	}
#endif
