#ifndef LYRICBROWSER_CPP
#define LYRICBROWSER_CPP
#include "lyricbrowser.h"
LyricBrowser::LyricBrowser(DataBackend * c):QTextBrowser()
	{
	conn=c;
	browser=new QHttp();
	connect(browser,SIGNAL(requestFinished(int,bool)),this,SLOT(httpData(int,bool)));
	browser->setHost("lyrc.com.ar");
	}

LyricBrowser::~LyricBrowser()
	{
	delete browser;
	}

QString LyricBrowser::parse(QString in)
	{
	QString out=in;
	while(out.indexOf("<script")!=-1)
		{
		int foo=out.indexOf("<script");
		int bar=out.indexOf("/script>",foo)+8;
		int foobar=bar-foo;
		out.remove(foo,foobar);
		}
	while(out.indexOf("<noscript")!=-1)
		{
		int foo=out.indexOf("<noscript");
		int bar=out.indexOf("/noscript>",foo)+10;
		int foobar=bar-foo;
		out.remove(foo,foobar);
		}
	while(out.indexOf("color=\"white\"")!=-1)
		{
		int foo=out.indexOf("color=\"white\"");
		out.replace(foo,13,"color=\"black\"");
		}
	if(out.indexOf("bgcolor=\"black\"")!=-1)
		{
		int foo=out.indexOf("bgcolor=\"black\"");
		out.replace(foo,15,"bgcolor=\"white\"");
		}
	while(out.indexOf("color='white'")!=-1)
		{
		int foo=out.indexOf("color='white'");
		out.replace(foo,13,"color='black'");
		}
	while(out.indexOf("size=\"1\"")!=-1)
		{
		int foo=out.indexOf("size=\"1\"");
		out.replace(foo,8,"size=\"3\"");
		}
	while(out.indexOf("size='2'")!=-1)
		{
		int foo=out.indexOf("size='2'");
		out.replace(foo,8,"size='4'");
		}
	return out;
	}

void LyricBrowser::parseUrl(int id)
	{
	adress=new QString("/en/tema1en.php?artist=");
	MlibData * mlib=((MlibData *)conn->getDataBackendObject(DataBackend::MLIB));
	QVariant tmp=mlib->getInfo(QString("artist"),id);
	if(tmp.toInt()==-1)
		{
		this->setText(" ");
		return;
		}
	else if(tmp.toString()=="Unknown")
		{
		this->setText("Unknown Artist");
		return;
		}
	std::string foo=tmp.toString().toStdString();
	while(foo.find(" ")!=std::string::npos)
		{foo.replace(foo.find(" "),1,"%20");}
	adress->append(foo.c_str());
	adress->append("&songname=");
	tmp=mlib->getInfo(QString("title"),id);
	if(tmp.toString()=="Unknown")
		{
		this->setText("Unknown Title");
		return;
		}
	foo=tmp.toString().toStdString();
	while(foo.find(" ")!=std::string::npos)
		{foo.replace(foo.find(" "),1,"%20");}
	adress->append(foo.c_str());
	this->setSource(*(new QUrl(*adress)));
	//delete adress;
	//adress= new QString();
	}

void LyricBrowser::httpData(int id, bool error)
	{
	if(id!=currentID) return;
	QString str = browser->readAll();
	this->setHtml(parse(str));
	}

void LyricBrowser::setSource(const QUrl & url)
	{
	if(url.host().isEmpty()==false)
		{
		QDesktopServices::openUrl(url);
		return;
		}
	QString path=url.path();
	path.prepend("/");
	path.append("?");
	QList <QPair<QString, QString> > queries=url.queryItems();
	for(int z=0;z<queries.size();z++)
		{
		path.append(queries.at(z).first);
		path.append("=");
		path.append(queries.at(z).second);
		if(z+1<queries.size())
			path.append("&");
		}
	std::string foo=path.toStdString();
	while(foo.find(" ")!=std::string::npos)
		{foo.replace(foo.find(" "),1,"%20");}
	path.clear();
	path.append(foo.c_str());
	QHttpRequestHeader header("GET", path);
	header.setValue("Host", "lyrc.com.ar");
	currentID=browser->request(header);
	}
#endif
