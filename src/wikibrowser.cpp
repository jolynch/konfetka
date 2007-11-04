#ifndef WIKIBROWSER_CPP
#define WIKIBROWSER_CPP
#include "wikibrowser.h"
WikiBrowser::WikiBrowser(QWidget * parent):QTextBrowser()
	{
	browser=new QHttp();
	wikiPath = new QString("/wiki/");
	browser->setHost("en.wikipedia.org");
	this->setOpenExternalLinks (false);
	
	pathProbFix = new QString();
	
	connect(browser,SIGNAL(requestFinished(int,bool)),this,SLOT(httpData(int,bool)));
	}

WikiBrowser::~WikiBrowser()
	{
	delete browser;
	delete wikiPath;
	}

void WikiBrowser::parseUrl(Xmms::PropDict info)
	{
	//std::cout<<browser->hasPendingRequests ()<<std::endl;
	wikiPath->clear(); wikiPath->append("/wiki/");
	std::string foo;
	try
		{
		foo=info.get<std::string>("artist");
		}
	catch(Xmms::no_such_key_error& err)
		{
		this->setText("Could not read tag");
		return;
		}
	while(foo.find(" ")!=std::string::npos)
		{foo.replace(foo.find(" "),1,"_");}
	wikiPath->append(foo.c_str());
	if(last!=wikiPath)
	this->setSource(QUrl(*wikiPath));
	last = *wikiPath;
	}

QVariant WikiBrowser::loadResource ( int type, const QUrl & name )
	{
		if(type==QTextDocument::ImageResource)
			{
			if(!name.host().isEmpty())
				http=new ImageHttp(name.host());
			else
				http=new ImageHttp("en.wikipedia.org");
			data = http->getStuff(name.path());
			QImage image;
				if(image.loadFromData(data)) {
				return data;
				} else {
				image.setText("error","Could Not Retrieve Image");
				return image;	
				}
			}
		else if(type==QTextDocument::StyleSheetResource) {
		QString empty="CSS not supported yet";
		return empty; 
		}
		return QTextBrowser::loadResource(type, name);
	}

void WikiBrowser::httpData(int id, bool error)
	{
	if(currentID!=id) return;
	QString str = browser->readAll();
	this->setHtml(str);
	}

void WikiBrowser::setSource(const QUrl & url)
	{
	pathProbFix->clear(); 

	if(!url.toString().contains("/wiki/"))
	pathProbFix->append("/wiki/");
	
	if(url.toString().contains("#")){
	scroll(url);
	return;
	}
	
	pathProbFix->append(url.toString());
	
	if(url.host().isEmpty()==false)
		{
		QDesktopServices::openUrl(url);
		return;
		}

	if(bStack.isEmpty() || bStack.top()!= *pathProbFix) {
	bStack.push(*pathProbFix);
	

        if (!fStack.isEmpty() && fStack.top() == *pathProbFix) {
		fStack.pop();
        } 
	else {	
		fStack.clear();
        }
	}
    	
	
	QHttpRequestHeader header("GET", *pathProbFix);
	header.setValue("Host", "en.wikipedia.org");
	header.setValue("User-Agent", "Qt-Integrated-Browser/0.01 Qt/4.2");
	currentID=browser->request(header);
	}

void WikiBrowser::slotBack() {
	if (bStack.count() <= 1)
	return;
	fStack.push(bStack.pop());
	this->setSource(QUrl(bStack.top()));
}

void WikiBrowser::slotForward() {
	if (fStack.isEmpty())
	return;
	bStack.push(fStack.pop());
	this->setSource(QUrl(bStack.top()));
}

void WikiBrowser::scroll(const QUrl & url) {
	pathProbFix->clear();
	pathProbFix->append(url.toString());
	pathProbFix->remove(0,1);
	this->scrollToAnchor(*pathProbFix);
}


WikiView::WikiView(QWidget * parent,Qt::WindowFlags f):QWidget(parent,f) {
	goHome = new QPushButton("To Artist");
	backButton = new QPushButton("Go Back");
	forwardButton = new QPushButton("Go Forward");
	
	artistHome=new QString();

	progressBar = new QProgressBar();

	myWiki = new WikiBrowser(this);
	connect(myWiki->browser,SIGNAL(dataReadProgress(int, int)),this, SLOT(updateProgress(int,int)));
	connect(backButton,SIGNAL(clicked()),myWiki,SLOT(slotBack()));
	connect(forwardButton,SIGNAL(clicked()),myWiki,SLOT(slotForward()));
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

void WikiView::updateProgress(int done, int total) {
progressBar->setValue ((int)(((double)done/(double)total)*100));
}

void WikiView::goToHome() {
if(!artistHome->isEmpty()) 
myWiki->setSource(QUrl(*artistHome));
}

void WikiView::setHome(Xmms::PropDict info) {
std::string foo;

try{
foo=info.get<std::string>("artist");
}
catch(Xmms::no_such_key_error& err){return;}

while(foo.find(" ")!=std::string::npos){
foo.replace(foo.find(" "),1,"_");
}

artistHome->clear();
artistHome->append(foo.c_str());
}

#endif
