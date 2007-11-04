#ifndef VISBAND_H
#define VISBAND_H
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QColor>
#include <QPalette>

class VisBand:public QWidget
	{	Q_OBJECT
	private:
		unsigned int height;
		QVBoxLayout * layout;
		QLabel * l5;
		QLabel * l4;
		QLabel * l3;
		QLabel * l2;
		QLabel * l1;
		QLabel * l0;
	public:
		VisBand(QWidget * parent = 0, Qt::WindowFlags f = 0);
		~VisBand();
		void setHeight(unsigned int in);
		unsigned int getHeight();
	};

#endif
