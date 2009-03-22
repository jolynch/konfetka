#ifndef WIKIBROWSER_CPP
#define WIKIBROWSER_CPP
#include "wikibrowser.h"

WikiView::WikiView(DataBackend * c,QWidget * parent,Qt::WindowFlags f):QWidget(parent,f) {
	conn=c;
	goHome = new QPushButton("To Artist");
	backButton = new QPushButton("Go Back");
	forwardButton = new QPushButton("Go Forward");
	wikiPath = "http://en.wikipedia.org/wiki/";


	progressBar = new QProgressBar();
	progressBar->setRange(0,100);

	myWiki = new QWebView();
	connect(myWiki,SIGNAL(loadProgress(int)),progressBar, SLOT(setValue(int)));
	connect(backButton,SIGNAL(clicked()),myWiki,SLOT(back()));
	connect(forwardButton,SIGNAL(clicked()),myWiki,SLOT(forward()));
	connect(goHome,SIGNAL(clicked()),this,SLOT(goToHome()));
	
	hlayout = new QHBoxLayout();
	vlayout = new QVBoxLayout();

	hlayout->addWidget(backButton);
	hlayout->addWidget(progressBar);
	hlayout->addWidget(forwardButton);
	hlayout->addWidget(goHome);
	vlayout->addWidget(myWiki);
	vlayout->addLayout(hlayout);
	this->setLayout(vlayout);

}

WikiView::~WikiView() {
	delete backButton;
	delete forwardButton;
	delete goHome;
	delete progressBar;
	delete hlayout;
	delete vlayout;
	delete myWiki;
}

void WikiView::parseUrl(int id,bool force) {
	MlibData * mlib=((MlibData *)conn->getDataBackendObject(DataBackend::MLIB));
	QVariant tmp=mlib->getInfo(QString("artist"),id);
	if(tmp.toInt()==-1)
		{return;}
	if(tmp.toString()=="Unknown") return;
	QString path = wikiPath + tmp.toString();

	if(id!=lastLoaded || force) {
		myWiki->load(path);
		lastLoaded = id;
	}
}

void WikiView::goToHome() {
	parseUrl(lastLoaded,true);
}

#endif
