#ifndef WEBPANEL_CPP
#define WEBPANEL_CPP
#include "webpanel.h"


WebPanel::WebPanel(DataBackend * c,QWidget * parent,Qt::WindowFlags f):LayoutPanel(parent,f) {

	//these have to be initialized before registerService is used
	tab = new QTabWidget();
	progressBar = new QProgressBar();
	progressBar->setRange(0,100);

	shout = new Shoutcast(c,this);
	registerService(shout);

	tab->addTab(new QLabel("Last.fm"),"Last.fm");
	
	tab->addTab(new QLabel("Skidbladnir"),"Skidbladnir");
	tab->addTab(new QLabel("Amazon"),"Amazon Cover Manager");
	info = new QLabel("Plugin Info");
	
	layout = new QGridLayout();
	layout->addWidget(tab,0,0,1,4);
	layout->addWidget(info,1,0,1,3);
	layout->addWidget(progressBar,1,3,1,1);
	this->setLayout(layout);
	
	connect(tab,SIGNAL(currentChanged(int)),this,SLOT(updateInfo(int))); 
}

bool WebPanel::registerService(WebService * service) {
	tab->addTab(service,service->getServiceName());
	tab->setTabToolTip(tab->indexOf(service),service->getInfo());
	connect(service,SIGNAL(progress(int)),progressBar,SLOT(setValue(int)));
	connect(service,SIGNAL(progress(int,int)),this,SLOT(showProgress(int,int)));
	connect(service,SIGNAL(progress(qint64,qint64)),this,SLOT(showProgress(qint64,qint64)));


	return true;
}

void WebPanel::showProgress(qint64 in ,qint64 total) {
	showProgress((int)in,(int)total);
}


void WebPanel::showProgress(int in,int total) {
	double d_in = (double) in;
	double d_total = (double) total;
	progressBar->setValue((int)(100*(d_in/d_total)));
}

void WebPanel::setLayoutSide(bool right_side) { //true=right, false=left
}

void WebPanel::updateInfo(int index) {
	info->setText(QString(index));
	//info->setText(((WebService*)(tab->currentWidget()))->getInfo());
}

WebPanel::~WebPanel() {
	delete progressBar;
	delete tab;
}
#endif
