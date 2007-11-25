#ifndef LAYOUTPANEL
#define LAYOUTPANEL
#include <iostream>
#include <QWidget>
class LayoutPanel:public QWidget
	{	Q_OBJECT
	public:
		LayoutPanel(QWidget * p=0, Qt::WindowFlags f=0):QWidget(p,f){}
		virtual void setLayoutSide(bool right_side) {std::cout<<"base"<<std::endl;} //true=right, false=left
		virtual ~LayoutPanel() {}
	};
#endif
