#ifndef EQUALIZER_CPP
#define EQUALIZER_CPP
#include "equalizer.h"
using namespace std;
Equalizer::Equalizer(DataBackend * c,int width,QWidget * parent, Qt::WindowFlags f):QWidget(parent, f)
	{
	conn=c;
	layout=new QGridLayout();
	enable=new QCheckBox("Enable Equalizer");
	enable->setCheckState(Qt::Unchecked);
	extra=new QCheckBox("Extra Filter");
	extra->setCheckState(Qt::Unchecked);
	preamp=new QSlider(Qt::Vertical);
	preamp->setRange(-20, 20);
	preamp->setValue(0);
	s0=new QSlider(Qt::Vertical);
	s0->setRange(-20, 20);
	s0->setValue(0);
	s0->setTracking(false);
	s1=new QSlider(Qt::Vertical);
	s1->setRange(-20, 20);
	s1->setValue(0);
	s1->setTracking(false);
	s2=new QSlider(Qt::Vertical);
	s2->setRange(-20, 20);
	s2->setValue(0);
	s2->setTracking(false);
	s3=new QSlider(Qt::Vertical);
	s3->setRange(-20, 20);
	s3->setValue(0);
	s3->setTracking(false);
	s4=new QSlider(Qt::Vertical);
	s4->setRange(-20, 20);
	s4->setValue(0);
	s4->setTracking(false);
	s5=new QSlider(Qt::Vertical);
	s5->setRange(-20, 20);
	s5->setValue(0);
	s5->setTracking(false);
	s6=new QSlider(Qt::Vertical);
	s6->setRange(-20, 20);
	s6->setValue(0);
	s6->setTracking(false);
	s7=new QSlider(Qt::Vertical);
	s7->setRange(-20, 20);
	s7->setValue(0);
	s7->setTracking(false);
	s8=new QSlider(Qt::Vertical);
	s8->setRange(-20, 20);
	s8->setValue(0);
	s8->setTracking(false);
	s9=new QSlider(Qt::Vertical);
	s9->setRange(-20, 20);
	s9->setValue(0);
	s9->setTracking(false);
	QIcon *loadIcon=new QIcon(":images/open.png");
	
	load=new QComboBox();
	//load->setIcon(*loadIcon);
 	load->setMaximumSize(75,25);
	load->setToolTip("Load Presets");
	
	QString dirName;
	dirName.prepend("/clients/konfetka/eqPresets/");
	dirName.prepend(Xmms::getUserConfDir().c_str());

	QDir dir(dirName);	
	dir.setFilter(QDir::Files|QDir::NoDotAndDotDot);
	QStringList list = dir.entryList();
	load->addItem(*loadIcon,"Load",QVariant("Default"));
	load->addItem("Manual",QVariant("Manual"));
	for(int i=0;i<list.size();i++) {
	load->addItem(list.value(i).left(list.value(i).length()-7),QVariant(list.value(i).left(list.value(i).length()-7)));
	}

	QIcon *saveIcon=new QIcon(":images/save.png");
	save=new QPushButton();
	save->setIcon(*saveIcon);
 	save->setMaximumSize(75,25);
	save->setToolTip("Save preset");
	
	buttons = new QVBoxLayout();
	buttons->addWidget(load);
	buttons->addWidget(save);

	layout->addWidget(enable,0,0,1,5);
	layout->addWidget(extra,0,7,1,5);
	layout->addWidget(preamp,1,0,3,1);
	layout->addLayout(buttons,1,1,3,2);
	//layout->addWidget(load,1,1,1,1);
	//layout->addWidget(save,2,1,1,1);
	layout->addWidget(s0,1,3,3,1);
	layout->addWidget(s1,1,4,3,1);					
	layout->addWidget(s2,1,5,3,1);
	layout->addWidget(s3,1,6,3,1);
	layout->addWidget(s4,1,7,3,1);
	layout->addWidget(s5,1,8,3,1);
	layout->addWidget(s6,1,9,3,1);
	layout->addWidget(s7,1,10,3,1);
	layout->addWidget(s8,1,11,3,1);
	layout->addWidget(s9,1,12,3,1);
	for(int i=0;i<layout->columnCount();i++) {
	layout->setColumnStretch(i,10);
	}
	layout->setColumnStretch(1,1);layout->setColumnStretch(2,1);
// 	layout->setHorizontalSpacing((2*width)/35-10);
 	layout->setRowMinimumHeight(0,50);
	this->setLayout(layout);
	conn->config.valueSet("equalizer.use_legacy","1")
			(Xmms::bind(&DataBackend::scrapResult, conn));
	conn->config.valueGet("equalizer.enabled")
			((boost::bind(&Equalizer::setVal, this,"equalizer.enabled", _1)));
	conn->config.valueGet("equalizer.extra_filtering")
			(boost::bind(&Equalizer::setVal, this,"equalizer.extra_filtering", _1 ));
	conn->config.valueGet("equalizer.preamp")
			(boost::bind(&Equalizer::setVal, this,"equalizer.preamp", _1 ));
	conn->config.valueGet("equalizer.legacy0")
			(boost::bind(&Equalizer::setVal, this,"equalizer.legacy0", _1 ));
	conn->config.valueGet("equalizer.legacy1")
			(boost::bind(&Equalizer::setVal, this,"equalizer.legacy1", _1 ));
	conn->config.valueGet("equalizer.legacy2")
			(boost::bind(&Equalizer::setVal, this,"equalizer.legacy2", _1 ));
	conn->config.valueGet("equalizer.legacy3")
			(boost::bind(&Equalizer::setVal, this,"equalizer.legacy3", _1 ));
	conn->config.valueGet("equalizer.legacy4")
			(boost::bind(&Equalizer::setVal, this,"equalizer.legacy4", _1 ));
	conn->config.valueGet("equalizer.legacy5")
			(boost::bind(&Equalizer::setVal, this,"equalizer.legacy5", _1 ));
	conn->config.valueGet("equalizer.legacy6")
			(boost::bind(&Equalizer::setVal, this,"equalizer.legacy6", _1 ));
	conn->config.valueGet("equalizer.legacy7")
			(boost::bind(&Equalizer::setVal, this,"equalizer.legacy7", _1 ));
	conn->config.valueGet("equalizer.legacy8")
			(boost::bind(&Equalizer::setVal, this,"equalizer.legacy8", _1 ));
	conn->config.valueGet("equalizer.legacy9")
			(boost::bind(&Equalizer::setVal, this,"equalizer.legacy9", _1 ));

	QObject::connect(preamp,SIGNAL(valueChanged(int)),
			this,SLOT(setPreamp(int)));
	QObject::connect(s0,SIGNAL(valueChanged(int)),
			this,SLOT(set0(int)));
	QObject::connect(s1,SIGNAL(valueChanged(int)),
			this,SLOT(set1(int)));
	QObject::connect(s2,SIGNAL(valueChanged(int)),
			this,SLOT(set2(int)));
	QObject::connect(s3,SIGNAL(valueChanged(int)),
			this,SLOT(set3(int)));
	QObject::connect(s4,SIGNAL(valueChanged(int)),
			this,SLOT(set4(int)));
	QObject::connect(s5,SIGNAL(valueChanged(int)),
			this,SLOT(set5(int)));
	QObject::connect(s6,SIGNAL(valueChanged(int)),
			this,SLOT(set6(int)));
	QObject::connect(s7,SIGNAL(valueChanged(int)),
			this,SLOT(set7(int)));
	QObject::connect(s8,SIGNAL(valueChanged(int)),
			this,SLOT(set8(int)));
	QObject::connect(s9,SIGNAL(valueChanged(int)),
			this,SLOT(set9(int)));
	QObject::connect(enable,SIGNAL(stateChanged(int)),
			this,SLOT(toggleEnable(int)));
	QObject::connect(extra,SIGNAL(stateChanged(int)),
			this,SLOT(toggleExtra(int)));
	QObject::connect(conn,SIGNAL(configValChanged(const Xmms::Dict&)),
			this,SLOT(changesResponse(const Xmms::Dict&)));
	QObject::connect(load,SIGNAL(activated(int)),this,SLOT(loadFromFile(int)));
	QObject::connect(save,SIGNAL(clicked()),this,SLOT(saveToFile()));
	QObject::connect(load,SIGNAL(currentIndexChanged(QString)),this,SLOT(newToolTip(QString)));
	}

Equalizer::~Equalizer()
	{
	delete layout;
	delete enable;
	delete extra;
	delete preamp;
	delete s0;
	delete s1;
	delete s2;
	delete s3;
	delete s4;
	delete s5;
	delete s6;
	delete s7;
	delete s8;
	delete s9;
	}

void Equalizer::newToolTip(QString val) {
	load->setToolTip(val);
}

bool Equalizer::setVal(std::string key,std::string val)
	{
	if(key=="equalizer.enabled")
		{if(val=="0") enable->setCheckState(Qt::Unchecked);
		else enable->setCheckState(Qt::Checked); return false;}
	if(key=="equalizer.extra_filtering")
		{if(val=="0") extra->setCheckState(Qt::Unchecked);
		else extra->setCheckState(Qt::Checked); return false;}
	if(key=="equalizer.use_legacy")
		{if(val=="0")conn->config.valueSet("equalizer.use_legacy","1")
				(Xmms::bind(&DataBackend::scrapResult, conn));return false;}
	if(key=="equalizer.preamp") preamp->setValue(atoi(val.c_str()));
	if(key=="equalizer.legacy0") s0->setValue(atoi(val.c_str()));
	if(key=="equalizer.legacy1") s1->setValue(atoi(val.c_str()));
	if(key=="equalizer.legacy2") s2->setValue(atoi(val.c_str()));
	if(key=="equalizer.legacy3") s3->setValue(atoi(val.c_str()));
	if(key=="equalizer.legacy4") s4->setValue(atoi(val.c_str()));
	if(key=="equalizer.legacy5") s5->setValue(atoi(val.c_str()));
	if(key=="equalizer.legacy6") s6->setValue(atoi(val.c_str()));
	if(key=="equalizer.legacy7") s7->setValue(atoi(val.c_str()));
	if(key=="equalizer.legacy8") s8->setValue(atoi(val.c_str()));
	if(key=="equalizer.legacy9") s9->setValue(atoi(val.c_str()));
	return false;
	}

void Equalizer::toggleEnable(int state)
	{
	switch (state)
		{
		case Qt::Unchecked:
		conn->config.valueSet("equalizer.enabled","0")
				(Xmms::bind(&DataBackend::scrapResult, conn));
		break;
		case Qt::Checked:
		conn->config.valueSet("equalizer.enabled","1")
				(Xmms::bind(&DataBackend::scrapResult, conn));
		break;
		}
	}

void Equalizer::toggleExtra(int state)
	{
	switch (state)
		{
		case Qt::Unchecked:
		conn->config.valueSet("equalizer.extra_filtering","0")
				(Xmms::bind(&DataBackend::scrapResult, conn));
		break;
		case Qt::Checked:
		conn->config.valueSet("equalizer.extra_filtering","1")
				(Xmms::bind(&DataBackend::scrapResult, conn));
		break;
		}
	}

void Equalizer::changesResponse(const Xmms::Dict& valChanged)
	{
	if(valChanged.contains("equalizer.enabled"))
		{
		setVal("equalizer.enabled", valChanged.get<std::string>("equalizer.enabled"));
		}
	if(valChanged.contains("equalizer.extra_filtering"))
		setVal("equalizer.extra_filtering",
					 valChanged.get<std::string>("equalizer.extra_filtering"));
	if(valChanged.contains("equalizer.use_legacy"))
		setVal("equalizer.use_legacy",valChanged.get<std::string>("equalizer.use_legacy"));
	if(valChanged.contains("equalizer.preamp"))
		setVal("equalizer.preamp",valChanged.get<std::string>("equalizer.preamp"));
	if(valChanged.contains("equalizer.legacy0"))
		setVal("equalizer.legacy0",valChanged.get<std::string>("equalizer.legacy0"));
	if(valChanged.contains("equalizer.legacy1"))
		setVal("equalizer.legacy1",valChanged.get<std::string>("equalizer.legacy1"));
	if(valChanged.contains("equalizer.legacy2"))
		setVal("equalizer.legacy2",valChanged.get<std::string>("equalizer.legacy2"));
	if(valChanged.contains("equalizer.legacy3"))
		setVal("equalizer.legacy3",valChanged.get<std::string>("equalizer.legacy3"));
	if(valChanged.contains("equalizer.legacy4"))
		setVal("equalizer.legacy4",valChanged.get<std::string>("equalizer.legacy4"));
	if(valChanged.contains("equalizer.legacy5"))
		setVal("equalizer.legacy5",valChanged.get<std::string>("equalizer.legacy5"));
	if(valChanged.contains("equalizer.legacy6"))
		setVal("equalizer.legacy6",valChanged.get<std::string>("equalizer.legacy6"));
	if(valChanged.contains("equalizer.legacy7"))
		setVal("equalizer.legacy7",valChanged.get<std::string>("equalizer.legacy7"));
	if(valChanged.contains("equalizer.legacy8"))
		setVal("equalizer.legacy8",valChanged.get<std::string>("equalizer.legacy8"));
	if(valChanged.contains("equalizer.legacy9"))
		setVal("equalizer.legacy9",valChanged.get<std::string>("equalizer.legacy9"));
	}

void Equalizer::setPreamp(int val)
	{
	std::stringstream ss;
	std::string str;
	ss << val; ss >> str;
	conn->config.valueSet("equalizer.preamp",str)
			(Xmms::bind(&DataBackend::scrapResult, conn));
	}

void Equalizer::set0(int val)
	{
	std::stringstream ss;
	std::string str;
	ss << val; ss >> str;
	conn->config.valueSet("equalizer.legacy0",str)
			(Xmms::bind(&DataBackend::scrapResult, conn));
	}

void Equalizer::set1(int val)
	{
	std::stringstream ss;
	std::string str;
	ss << val; ss >> str;
	conn->config.valueSet("equalizer.legacy1",str)
			(Xmms::bind(&DataBackend::scrapResult, conn));
	}

void Equalizer::set2(int val)
	{
	std::stringstream ss;
	std::string str;
	ss << val; ss >> str;
	conn->config.valueSet("equalizer.legacy2",str)
			(Xmms::bind(&DataBackend::scrapResult, conn));
	}

void Equalizer::set3(int val)
	{
	std::stringstream ss;
	std::string str;
	ss << val; ss >> str;
	conn->config.valueSet("equalizer.legacy3",str)
			(Xmms::bind(&DataBackend::scrapResult, conn));
	}

void Equalizer::set4(int val)
	{
	std::stringstream ss;
	std::string str;
	ss << val; ss >> str;
	conn->config.valueSet("equalizer.legacy4",str)
			(Xmms::bind(&DataBackend::scrapResult, conn));
	}

void Equalizer::set5(int val)
	{
	std::stringstream ss;
	std::string str;
	ss << val; ss >> str;
	conn->config.valueSet("equalizer.legacy5",str)
			(Xmms::bind(&DataBackend::scrapResult, conn));
	}

void Equalizer::set6(int val)
	{
	std::stringstream ss;
	std::string str;
	ss << val; ss >> str;
	conn->config.valueSet("equalizer.legacy6",str)
			(Xmms::bind(&DataBackend::scrapResult, conn));
	}

void Equalizer::set7(int val)
	{
	std::stringstream ss;
	std::string str;
	ss << val; ss >> str;
	conn->config.valueSet("equalizer.legacy7",str)
			(Xmms::bind(&DataBackend::scrapResult, conn));
	}

void Equalizer::set8(int val)
	{
	std::stringstream ss;
	std::string str;
	ss << val; ss >> str;
	conn->config.valueSet("equalizer.legacy8",str)
			(Xmms::bind(&DataBackend::scrapResult, conn));
	}

void Equalizer::set9(int val)
	{
	std::stringstream ss;
	std::string str;
	ss << val; ss >> str;
	conn->config.valueSet("equalizer.legacy9",str)
			(Xmms::bind(&DataBackend::scrapResult, conn));
	}

void Equalizer::saveToFile()
	{
	QString fileName = QInputDialog::getText(this, tr("Save Equalizer Preset"),tr("Preset Name: "),
						 QLineEdit::Normal,load->currentText());
	if(fileName.isNull()) return;
	if(!fileName.contains(".preset"))
	fileName.append(".preset");
	fileName.prepend("/clients/konfetka/eqPresets/");
	fileName.prepend(Xmms::getUserConfDir().c_str());
	QFile file (fileName);
		if(!file.open(QIODevice::WriteOnly)) {
		std::cout<<"Write Error: #"<<file.error()<<std::endl;
		return;
		}
	QTextStream out(&file);
	out<<"[Equalizer preset]\n";
	out<<"Preamp="<<preamp->value()<<".0e0\n";
	out<<"Band0="<<s0->value()<<".0\n";
	out<<"Band1="<<s1->value()<<".0\n";
	out<<"Band2="<<s2->value()<<".0\n";
	out<<"Band3="<<s3->value()<<".0\n";
	out<<"Band4="<<s4->value()<<".0\n";
	out<<"Band5="<<s5->value()<<".0\n";
	out<<"Band6="<<s6->value()<<".0\n";
	out<<"Band7="<<s7->value()<<".0\n";
	out<<"Band8="<<s8->value()<<".0\n";
	out<<"Band9="<<s9->value()<<".0\n";
	file.close();
	
	QString dirName;
	dirName.prepend("/clients/konfetka/eqPresets/");
	dirName.prepend(Xmms::getUserConfDir().c_str());
	QDir dir(dirName);
	
	dir.setFilter(QDir::Files|QDir::NoDotAndDotDot);
	QStringList list = dir.entryList();
	load->clear();
	load->addItem(QIcon(":images/open.png"),"Load",QVariant("Default"));
	load->addItem("Manual",QVariant("Manual"));
	for(int i=0;i<list.size();i++) {
	load->addItem(list.value(i).left(list.value(i).length()-7),QVariant(list.value(i).left(list.value(i).length()-7)));
	}

	}

void Equalizer::loadFromFile(int numToLoad)
	{
	if(numToLoad<=1) return;
	QString fileName;
	fileName.prepend("/clients/konfetka/eqPresets/");
	fileName.prepend(Xmms::getUserConfDir().c_str());

	QDir dir(fileName);	
	dir.setFilter(QDir::Files|QDir::NoDotAndDotDot);
	QStringList list = dir.entryList();
		if(numToLoad-2 <0 || numToLoad-2 > list.size()) return;
	fileName.append(list.value(numToLoad-2));

	if(fileName.isNull()) return;
	
	QFile file(fileName);
		if(!file.open(QIODevice::ReadOnly)) {
		std::cout<<"Read Error: #"<<file.error()<<std::endl;
		return;
		}
	QString input;
	input = file.readLine();
	if(input.trimmed()!="[Equalizer preset]") return;
	input = file.readLine();
	input.remove(0,input.indexOf("=")+1);
	setPreamp((int)(input.toDouble()));
	input = file.readLine();
	input.remove(0,input.indexOf("=")+1);
	set0((int)(int)(input.toDouble()));
	input = file.readLine();
	input.remove(0,input.indexOf("=")+1);
	set1((int)(int)(input.toDouble()));
	input = file.readLine();
	input.remove(0,input.indexOf("=")+1);
	set2((int)(int)(input.toDouble()));
	input = file.readLine();
	input.remove(0,input.indexOf("=")+1);
	set3((int)(int)(input.toDouble()));
	input = file.readLine();
	input.remove(0,input.indexOf("=")+1);
	set4((int)(int)(input.toDouble()));
	input = file.readLine();
	input.remove(0,input.indexOf("=")+1);
	set5((int)(int)(input.toDouble()));
	input = file.readLine();
	input.remove(0,input.indexOf("=")+1);
	set6((int)(int)(input.toDouble()));
	input = file.readLine();
	input.remove(0,input.indexOf("=")+1);
	set7((int)(int)(input.toDouble()));
	input = file.readLine();
	input.remove(0,input.indexOf("=")+1);
	set8((int)(int)(input.toDouble()));
	input = file.readLine();
	input.remove(0,input.indexOf("=")+1);
	set9((int)(int)(input.toDouble()));
	file.close();
	}

#endif
