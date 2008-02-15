#ifndef ALBUMART_CPP
#define ALBUMART_CPP
#include "albumart.h"

AlbumArt::AlbumArt(DataBackend * c) {
	conn=c;
	mlib=((MlibData *)conn->getDataBackendObject(DataBackend::MLIB));
	numToGet=0;
	hasAlbum = 0;
	toReflect = 1;
	noAlbum = QPixmap(":images/no_album175");
	setAcceptDrops(true);
	http = new QHttp();
	connect(conn,SIGNAL(qsettingsValueChanged(QString, QVariant)),this,SLOT(processSettingsUpdate(QString,QVariant)));
}

AlbumArt::~AlbumArt() {
	delete http;
}

void AlbumArt::processSettingsUpdate(QString name,QVariant value) {
	if(name == "konfetka/albumArtReflection")
	toReflect = value.toBool();
	update();
}


void AlbumArt::fetchXML(int newId) {
	id = newId;
// 	if(mlib->getInfo("picture_front",id).toString()!="Unknown") {
// 	conn->bindata.retrieve(mlib->getInfo("picture_front",id).toString().toStdString())
// 						(Xmms::bind(&AlbumArt::gotInformation,this));
// 	}
	QString tmpQuery;
		if(mlib->getInfo("arist",id).toInt()!=-1)
		tmpQuery += mlib->getInfo("artist",id).toString();
		if(mlib->getInfo("album",id).toInt()!=-1)
		tmpQuery += " " +mlib->getInfo("album",id).toString();
	if(tmpQuery == "" || query == tmpQuery) {
	return;
	}
	if(tmpQuery!=query)
	query = tmpQuery;
	std::cout<<"Looking for:"<<query.toStdString()<<std::endl;
	allCovers = QDomNodeList();
	//Thanks amarok for the D1URM... code thing
	QString toUrl;
	toUrl.append("http://xml.amazon.com/onca/xml3?t=webservices-20&dev-t=D1URM11J3F2CEH&KeywordSearch=");
	toUrl.append(QUrl::toPercentEncoding(query));
	toUrl.append("&mode=music&type=lite&locale=us&page=1&f=xml");

	QUrl url(toUrl);

	http->setHost(url.host(), url.port() != -1 ? url.port() : 80);
	if (!url.userName().isEmpty())
	http->setUser(url.userName(), url.password());
	
		if(xmlBuffer.isOpen())
		xmlBuffer.close();

		if(!xmlBuffer.isOpen()) {
		xmlBuffer.open(QIODevice::ReadWrite);
		}
			
	httpGetId = http->get(conn->encodeUrl(toUrl), &xmlBuffer);
	connect(&timeout, SIGNAL(timeout()), this, SLOT(fetchImage()));
// 	timeout.start(5000); //TODO let people change the timeout time
	connect(http, SIGNAL(done(bool)), this, SLOT(fetchImage(bool)));
}

void AlbumArt::fetchImage(bool err,bool force) {
	QObject::disconnect(http, 0, this, 0);
	timeout.stop();
		if(err) {
		std::cout<<"Error looking up XML: "<<http->error()<<std::endl;
			if(http->error()!=2) {
			setImage(1);
			return;
			}
			else
			std::cout<<"Trying from cache"<<std::endl;
		}

		if(imageBuffer.isOpen())
		imageBuffer.close();
	QDomDocument doc("art");
	QDomNode details;
	if(!force && doc.setContent(xmlBuffer.data())) { // Hope that this succeeds
	//TODO debugging output from the xmlbuffer	
	std::cout<<"Read in XML document"<<std::endl;
	details = doc.documentElement().namedItem( "Details" );

	allCovers = doc.elementsByTagName("Details");
 	imageUrl=allCovers.item(numToGet).namedItem("ImageUrlLarge").firstChild().nodeValue();
	std::cout<<allCovers.size()<<std::endl;
	}
	else if(force) {
	imageUrl=allCovers.item(numToGet).namedItem("ImageUrlLarge").firstChild().nodeValue();
	}
	else
	err = true;
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
		}
		else {
		hasAlbum = false;
		update();
		}
}

void AlbumArt::setImage(bool err) {
	QObject::disconnect(http, 0, this, 0);
	if(err) {
	std::cout<<"Error looking up image: "<<http->error()<<std::endl;
	return;
	}
// 	std::cout<<"SETTING IMAGE"<<std::endl;
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
	QMessageBox::information(this, ("Konfetka"),"Sorry, no albums were found that matched your query",
				QMessageBox::Ok);
	return;
	}
	QMenu * menu = new QMenu();
	menu->setTitle("Album Art");
	menu->addAction("Next Cover",this,SLOT(getNextCover()));
	menu->addAction("Previous Cover",this,SLOT(getPrevCover()));
	menu->addAction("Best Match",this,SLOT(getOrigCover()));
	menu->popup(event->globalPos());
}

//Allow you to cycle through available albumart
void AlbumArt::getPrevCover() {
	if((numToGet-1)<0)
	numToGet=allCovers.size()-1; 
	else
	numToGet-=1;
	makeRequest();
}

void AlbumArt::getNextCover() {
	numToGet=(numToGet+1)%allCovers.size();
	makeRequest();
}

void AlbumArt::getOrigCover() {
	numToGet=0;
	makeRequest();
}

//Basically, forces us to use xml instead of the bindata(which otherwise is used if present)
void AlbumArt::makeRequest(){
	fetchImage(false,true);
}

//This widget has no children, technically I guess I could make 'im a QFrame instead of QWidget ... ah well
void AlbumArt::paintEvent(QPaintEvent * event) {
	QPainter painter(this);
	painter.save();
	painter.setRenderHint(QPainter::Antialiasing,true);
	painter.fillRect(0,0,width(),height(), painter.background());
	if(hasAlbum){
		//Scales the drawing, but idk if I like it at 0,0 always)
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
	painter.restore();
}

//Can change whether we do the nice little reflection or not
void AlbumArt::slotReflect(bool reflect) {
	toReflect = reflect;
	update();
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
// 	std::cout<<url.toStdString()<<std::endl;
	std::cout<<url.host().toStdString()<<" "<<url.path().toStdString()<<std::endl;
	
	
		if(imageBuffer.isOpen())
		imageBuffer.close();
		if(!imageBuffer.isOpen()) {
		imageBuffer.open(QIODevice::ReadWrite);
		}

	http->setHost(url.host(),80);
	http->get(url.path(),&imageBuffer);
	connect(http, SIGNAL(done(bool)), this, SLOT(setImage(bool)));

	event->acceptProposedAction();
}

#endif





