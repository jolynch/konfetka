#ifndef ALBUMART_CPP
#define ALBUMART_CPP
#include "albumart.h"

AlbumArt::AlbumArt(DataBackend * c) {
	conn=c;
	mlib=((MlibData *)conn->getDataBackendObject(DataBackend::MLIB));
	numToGet=0;
	hasAlbum = 0;
	toReflect = 1;
	noAlbum = QPixmap(":images/no_album180");
	setAcceptDrops(true);
	http = new QHttp();
	menu = new QMenu();
	menu->setTitle("Album Art");
	menu->addAction("Next Cover",this,SLOT(getNextCover()));
	menu->addAction("Previous Cover",this,SLOT(getPrevCover()));
	menu->addAction("Best Match",this,SLOT(getOrigCover()));
	connect(conn,SIGNAL(qsettingsValueChanged(QString, QVariant)),this,SLOT(processSettingsUpdate(QString,QVariant)));
}

AlbumArt::~AlbumArt() {
	delete http;
	delete menu;
}

void AlbumArt::processSettingsUpdate(QString name,QVariant value) {
	if(name == "konfetka/albumArtReflection")
		toReflect = value.toBool();
	
	update();
}


void AlbumArt::fetchXML(int newId) {
	id = newId;
	QString tmpQuery;
		if(mlib->getInfo("arist",id).toInt()!=-1)
		tmpQuery += mlib->getInfo("artist",id).toString();
		if(mlib->getInfo("album",id).toInt()!=-1)
		tmpQuery += " " +mlib->getInfo("album",id).toString();
	if(tmpQuery == "" || query == tmpQuery) { //if it's the same don't do it again
	return;
	}
	if(tmpQuery!=query)
	query = tmpQuery;

	allCovers = QDomNodeList();
	//This very simple really ... TODO different locals, etc.. etc..
	QString toUrl="http://xml.amazon.com/onca/xml3?t=webservices-20&dev-t=12VHZKVBHQEAV5JTTQ02&KeywordSearch="+
		      (QUrl::toPercentEncoding(query))+
		      "&mode=music&type=lite&locale=us&page=1&f=xml";

	QUrl url(toUrl);
	//Just to make sure that we don't get anything wrong, use QUrl 
	http->setHost(url.host(), url.port() != -1 ? url.port() : 80);
	if (!url.userName().isEmpty())
	http->setUser(url.userName(), url.password());
	
	if(xmlBuffer.isOpen()) 
		xmlBuffer.close();
	if(!xmlBuffer.isOpen()) 
		xmlBuffer.open(QIODevice::ReadWrite);
			
	httpGetId = http->get(conn->encodeUrl(toUrl), &xmlBuffer);//Get the xml from amazon and store in a buffer
	connect(&timeout, SIGNAL(timeout()), this, SLOT(fetchImage()));
	timeout.start(5000); //TODO let people change the timeout time
	connect(http, SIGNAL(done(bool)), this, SLOT(fetchImage(bool)));
}

void AlbumArt::fetchImage(bool err,bool force) {
	QObject::disconnect(http, 0, this, 0);
	timeout.stop();
	
	if(err) {
			qDebug()<<"Error looking up XML:"<<http->error();
			if(http->error()!=2) {
				setImage(1);
				return;
			}
			else
				std::cout<<"Trying from cache"<<std::endl;
	}

	if(imageBuffer.isOpen()) imageBuffer.close(); //We have to close this if we're going to use it again
	
	QDomDocument doc("art");
	QDomNode details;
	if(!force && doc.setContent(xmlBuffer.data())) {
		//TODO debugging output from the xmlbuffer	
		details = doc.documentElement().namedItem( "Details" );
		allCovers = doc.elementsByTagName("Details");
		imageUrl = allCovers.item(numToGet).namedItem("ImageUrlLarge").firstChild().nodeValue();
	}
	else if(force) {
		imageUrl=allCovers.item(numToGet).namedItem("ImageUrlLarge").firstChild().nodeValue();
	}
	else {
		err = true;
	}
	//Once you have the list of covers, go look to see if this artist is already caches in bindata
		if(mlib->getInfo("picture_front",id).toString()!="Unknown" && !force) {
			conn->bindata.retrieve(mlib->getInfo("picture_front",id).toString().toStdString())
					      (Xmms::bind(&AlbumArt::gotInformation,this));
		}
		else if(!err) {
			QUrl url(imageUrl);
		
				if(!imageBuffer.isOpen()) {
				imageBuffer.open(QIODevice::ReadWrite);
				}
			std::cout<<"Fetching image from server: ["<<url.host().toStdString()<<"] ["<<url.path().toStdString()<<"]"<<std::endl;
			http->setHost(url.host(),80);
			http->get(url.path(),&imageBuffer);
			connect(http, SIGNAL(done(bool)), this, SLOT(setImage(bool)));
		} //at this point we can't fetch it and it's not in bindata
		else {
			hasAlbum = false;
			emit newPixmap(noAlbum);
			update();
		}
}

void AlbumArt::setImage(bool err) {
	QObject::disconnect(http, 0, this, 0);
	if(err) {
		qDebug()<<"Error looking up album cover:"<<imageUrl<<http->error();
	return;
	}
		QImage tmp = QImage::fromData(imageBuffer.data());
			if(tmp.isNull()) {
				emit newPixmap(noAlbum);
				hasAlbum = false;
				update();
				return;
			}
			else
				center = QPixmap::fromImage(tmp); 
		emit newPixmap(center);
		hasAlbum = true;
		//Take the image, flip it, apply a gradient, and then cut it in half, which should reveal a nice little reflection
		center = center.scaled(175,175,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
		QPixmap reverseLabel(center.transformed(QMatrix(-1,0,0,1,0,0),Qt::SmoothTransformation));
		reverseLabel.setAlphaChannel(QPixmap(":images/gradient.png"));

		QImage rightImage = reverseLabel.toImage().copy(QRect(0,0,87,175));
		QImage leftImage = reverseLabel.toImage().copy(QRect(87,0,87,175));

		left = QPixmap::fromImage(leftImage).transformed(QMatrix(1,-.1,0,1,0,0),Qt::SmoothTransformation);
		right = QPixmap::fromImage(rightImage).transformed(QMatrix(1,.1,0,1,0,0),Qt::SmoothTransformation);

		reverseLabel = reverseLabel.fromImage(leftImage,Qt::AutoColor); 
		reverseLabel= reverseLabel.transformed(QMatrix(1,-.1,0,1,0,0),Qt::SmoothTransformation);
	//This is the new cover for this song
	conn->bindata.add(Xmms::bin((unsigned char*) imageBuffer.data().data(),imageBuffer.size()))
					(Xmms::bind(&AlbumArt::sentInformation,this));
	imageBuffer.close();
	update();
	}

//What you wanna do foo?
void AlbumArt::mouseReleaseEvent(QMouseEvent * event) {
	if(allCovers.isEmpty()) {
	QMessageBox::information(this, ("Konfetka Album Search"),"Sorry, no albums were found that matched your query",
				QMessageBox::Ok);
	return;
	}
	
	menu->popup(event->globalPos());
}

//Allow you to cycle through available albumart
//Gos one back
void AlbumArt::getPrevCover() {
	if((numToGet-1)<0)
		numToGet=allCovers.size()-1; 
	else
		numToGet-=1;
	makeRequest();
}

//Gos one more
void AlbumArt::getNextCover() {
	numToGet=(numToGet+1)%allCovers.size();
	makeRequest();
}

//Gets the "best match" from amazon's list
void AlbumArt::getOrigCover() {
	numToGet=0;
	makeRequest();
}

//Basically, forces us to use xml instead of the bindata (which is otherwise used if present)
void AlbumArt::makeRequest(){
	fetchImage(false,true);
}

//Paint that album art on there
void AlbumArt::paintEvent(QPaintEvent * event) {
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing,true);
	painter.fillRect(0,0,width(),height(), painter.background());
	if(hasAlbum){
		//Scales the drawing, but idk if I like it at 0,0 always) TODO center in both directions
		if(toReflect)
		painter.drawPixmap(QRect(0,0,this->width()/4,this->width()/2),left,QRect(0,0,87,175));
		painter.drawPixmap(QRect(this->width()/4,0,this->width()/2,this->width()/2),center,QRect(0,0,175,175));
		if(toReflect)
		painter.drawPixmap(QRect(3*(this->width()/4),0,this->width()/4,this->width()/2),right,QRect(0,0,87,175));
	}
	else {
	painter.drawPixmap(QRect(width()/4, (height()-noAlbum.height())/2 ,
		noAlbum.width(),noAlbum.height()),noAlbum,noAlbum.rect());
	}
}

//Recieved bindata -- xmms2 callback
bool AlbumArt::gotInformation(const Xmms::bin& res) {
	if(imageBuffer.isOpen())
	imageBuffer.close();
	
	imageBuffer.setData((const char*)(res.c_str()), res.size());
	setImage(0);

	return true;
}

//Recieved notification of successful sent bindata -- xmms2 callback
bool AlbumArt::sentInformation(const std::string& res) {
	conn->medialib.entryPropertySet(id,"picture_front",res,"client/konfetka")();
	return true;
}

void AlbumArt::dragEnterEvent(QDragEnterEvent * event) {
	if(event->mimeData()->hasFormat("text/plain"))
	event->acceptProposedAction();
}

void AlbumArt::dropEvent(QDropEvent * event) {
	QUrl url(event->mimeData()->text());	
		if(imageBuffer.isOpen())
			imageBuffer.close();
		if(!imageBuffer.isOpen()) 
			imageBuffer.open(QIODevice::ReadWrite);
	http->setHost(url.host(),80);
	http->get(url.path().replace(" ","%20"),&imageBuffer);
	connect(http, SIGNAL(done(bool)), this, SLOT(setImage(bool)));

	event->acceptProposedAction();
}

#endif





