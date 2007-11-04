#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#include <xmmsclient/xmmsclient.h>
#endif
#ifndef BOOST_BIND
#define BOOST_BIND
#include <boost/bind.hpp>
#endif
#ifndef EQUALIZER_H
#define EQUALIZER_H
#include "databackend.h"
#include <QWidget>
#include <QSlider>
#include <QCheckBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QString>
#include <QFileDialog>
#include <QIcon>
#include <QPushButton>
#include <string>
#include <fstream>
#include <sstream>
#include <QInputDialog>
#include <QLineEdit>
#include <QVariant>
#include <QComboBox>
#include <QTextStream>

class Equalizer:public QWidget
	{	Q_OBJECT
	private:
		DataBackend * conn;
		QGridLayout * layout;
		QVBoxLayout * buttons;
		QCheckBox * enable;
		QCheckBox * extra;
		QSlider * preamp;
		QSlider * s0;
		QSlider * s1;
		QSlider * s2;
		QSlider * s3;
		QSlider * s4;
		QSlider * s5;
		QSlider * s6;
		QSlider * s7;
		QSlider * s8;
		QSlider * s9;
		QComboBox * load;
		QPushButton * save;
	public:
		Equalizer(DataBackend * c,int width,QWidget * parent = 0, Qt::WindowFlags f = 0);
		~Equalizer();
		bool setVal(std::string key,std::string val);
	private slots:
		void setPreamp(int);
		void set0(int);
		void set1(int);
		void set2(int);
		void set3(int);
		void set4(int);
		void set5(int);
		void set6(int);
		void set7(int);
		void set8(int);
		void set9(int);
		void toggleEnable(int);
		void toggleExtra(int);
		void changesResponse(const Xmms::Dict& valChanged);
		void saveToFile();
		void loadFromFile(int);
		void newToolTip(QString);
	};
#endif
