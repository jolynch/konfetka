#ifndef ALBUMART_CPP
#define ALBUMART_CPP
#include "albumart.h"

AlbumArt::AlbumArt(DataBackend * c)
	{
	conn=c;
	mlib=((MlibData *)conn->getDataBackendObject(DataBackend::MLIB));
	numToGet=0;
	hasAlbum = 0;
	QSettings s;
	if(s.contains("reflectImage") && s.value("reflectImage").toInt() == Qt::Checked )
	toReflect=1;
	else
	toReflect=0;
	httpRequestAborted=0;

	noAlbum = QPixmap(":images/no_album175");
	
	http = new QHttp();
	xmlFile = NULL;

	artHashStorePath=Xmms::getUserConfDir().c_str();
	artHashStorePath.append("/clients/konfetka/artHashStore");
	artXmlPath=Xmms::getUserConfDir().c_str();
	artXmlPath.append("/clients/konfetka/art.xml");
	curAlbumArtPath=Xmms::getUserConfDir().c_str();
	curAlbumArtPath.append("/clients/konfetka/curAlbumArt.jpg");
	QFile file(artHashStorePath);
	file.open(QIODevice::ReadOnly);
	QDataStream in(&file);
	in >> knownUrls; //read the hash
	file.flush();
	file.close();

	center = QPixmap(":images/no_album150");
	}

AlbumArt::~AlbumArt()
	{
	QFile file(artHashStorePath);
	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);
	out<<knownUrls; // write the hash to file
	file.flush();
	file.close();

	//delete currentArtIcon;
	delete http;
	if(xmlFile != NULL)
	delete xmlFile;
	}

void AlbumArt::fetchXML(int newId) {
// 	std::cout<<"Asked to fetch XML"<<std::endl;
	id = newId;
	QString tmpQuery;
		if(mlib->getInfo("album",id).toInt()!=-1)
		tmpQuery += mlib->getInfo("album",id).toString();
		if(mlib->getInfo("arist",id).toInt()!=-1)
		tmpQuery += " " + mlib->getInfo("artist",id).toString();
	if(tmpQuery == "" || query == tmpQuery)
	return;
	if(tmpQuery!=query)
	query = tmpQuery;
		//Thanks amarok for the D1URM... code thing
	QString toUrl;
	toUrl.append("http://xml.amazon.com/onca/xml3?t=webservices-20&dev-t=D1URM11J3F2CEH&KeywordSearch=");
	toUrl.append(query);
	toUrl.append("&mode=music&type=lite&locale=us&page=1&f=xml");

	QUrl url(toUrl);
	xmlFile = new QFile(artXmlPath);
		if(!xmlFile->open(QIODevice::ReadWrite)) {
		QMessageBox::information(this, tr("HTTP"),
					tr("Unable to save the xmlFile %1: %2.")
					.arg(artXmlPath).arg(xmlFile->errorString()));
		delete xmlFile;
		xmlFile = NULL;
		return;
	}	

	http->setHost(url.host(), url.port() != -1 ? url.port() : 80);
	if (!url.userName().isEmpty())
	http->setUser(url.userName(), url.password());
	
   	httpRequestAborted = false;
	httpGetId = http->get(conn->encodeUrl(toUrl), xmlFile);

	connect(http, SIGNAL(done(bool)), this, SLOT(fetchImage(bool)));
}

void AlbumArt::fetchImage(bool err,bool force) {
// 	std::cout<<"Fetching Image "<<http->state()<<std::endl;
	if(err) {
		std::cout<<"Error looking up XML: "<<http->error()<<std::endl;
		setImage(1);
		return;
	}
	QObject::disconnect(http, 0, this, 0);
	std::cout<<"Fetching Image "<<http->state()<<std::endl;
	xmlFile->flush();
		if(imageBuffer.isOpen())
		imageBuffer.close();
	QDomDocument doc("art");

	QFile file2(artXmlPath);

    	if (!file2.open(QIODevice::ReadOnly))
        std::cout<<"Problem"<<std::endl;
    	if (!doc.setContent(&file2)) {
        file2.close();
    	}
    	file2.close();
	
	const QDomNode details = doc.documentElement().namedItem( "Details" );

	allCovers = doc.elementsByTagName("Details");
 	imageUrl=allCovers.item(numToGet).namedItem("ImageUrlLarge").firstChild().nodeValue();
	
	if(mlib->getInfo("picture_front",id).toString()!="Unknown" && !force) {
	conn->bindata.retrieve(mlib->getInfo("picture_front",id).toString().toStdString())
					(Xmms::bind(&AlbumArt::gotInformation,this));
	}
	else {
	QDomNode node = details;
	imageUrl.remove(0,imageUrl.indexOf("com")+3);

		if(!imageBuffer.isOpen()) {
		imageBuffer.open(QIODevice::ReadWrite);
		}
	http->setHost("images.amazon.com",80);
	http->get(imageUrl,&imageBuffer);
	connect(http, SIGNAL(done(bool)), this, SLOT(setImage(bool)));
	std::cout<<"xml "<<http->state()<<std::endl;
	}

	}

void AlbumArt::setImage(bool err) {
	QObject::disconnect(http, 0, this, 0);
// 	std::cout<<"Setting image"<<std::endl;
	if(err) {
	return;
	}
		QImage tmp = QImage::fromData(imageBuffer.data());
			if(tmp.isNull()) {
			center = QPixmap(":images/no_album150");
			emit newPixmap(center);
			hasAlbum = false;
			update();
			return;
			}
			else
			center = QPixmap::fromImage(tmp); 
		emit newPixmap(center);
		hasAlbum = true;
		center = center.scaled(175,175,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
		QPixmap reverseLabel(center.transformed(QMatrix(-1,0,0,1,0,0),Qt::SmoothTransformation));
		reverseLabel.setAlphaChannel(QPixmap(":images/gradient.png"));

		QImage rightImage = reverseLabel.toImage().copy(QRect(0,0,87,175));
		QImage leftImage = reverseLabel.toImage().copy(QRect(87,0,87,175));

		left = QPixmap::fromImage(leftImage);
		right = QPixmap::fromImage(rightImage);

		reverseLabel = reverseLabel.fromImage(leftImage,Qt::AutoColor); 
		reverseLabel= reverseLabel.transformed(QMatrix(1,-.1,0,1,0,0),Qt::SmoothTransformation);
		
/*		//std::cout<<reverseLabel.toImage().numBytes()<<std::endl;*/
		
		left = left.transformed(QMatrix(1,-.1,0,1,0,0),Qt::SmoothTransformation);
		right = right.transformed(QMatrix(1,.1,0,1,0,0),Qt::SmoothTransformation);
	conn->bindata.add(Xmms::bin((unsigned char*) imageBuffer.data().data(),imageBuffer.size()))
					(Xmms::bind(&AlbumArt::sentInformation,this));
	imageBuffer.close();
	update();
	}

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
	menu->addAction("Original Cover",this,SLOT(getOrigCover()));
	menu->addAction("Save Cover to File",this,SLOT(saveCoverToFile()));
	menu->popup(event->globalPos());
}

/* Functions that take care of the menu */
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

void AlbumArt::saveCoverToFile() {
// 	QString fileName = QFileDialog::getSaveFileName(this, "Save File",QDir::homePath ());
// 	if(fileName=="") return;
// 	
// 	if(!fileName.endsWith(".jpg",Qt::CaseInsensitive))
// 	fileName.append(".jpg");
// 		if(imageFile.size()>1000 && hasAlbum) {
// 			if(QFile::exists(fileName)) {
// 			QFile::remove(fileName); 
// 			}
// 
// 			if(imageFile.copy(fileName)) {
// 			QMessageBox::information (NULL,"File Saved","File Saved Successfully!", QMessageBox::Ok);
// 			int ret= QMessageBox::question(NULL,"Open File Now","Would you like to open the file now?",
// 						QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes);
// 				if(ret == QMessageBox::Yes) {
// 				////std::cout<<fileName.toStdString()<<std::endl;
// 				QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
// 				}
// 			}
// 			else {
// 			QMessageBox::critical(NULL,"File NOT Saved","File NOT Saved! Please check permissions and try again.", QMessageBox::Ok);
// 			return;
// 			}
// 
// 		}

}

void AlbumArt::makeRequest(){
	std::cout<<"making request"<<std::endl;
	fetchImage(0,true);
}

/* end */

void AlbumArt::paintEvent(QPaintEvent * event) {
	QPainter painter(this);
	painter.save();
	painter.setRenderHint(QPainter::Antialiasing,true);
	painter.fillRect(0,0,width(),height(), painter.background());
	if(hasAlbum){
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

void AlbumArt::slotReflect(bool reflect) {
toReflect = reflect;
update();
}

bool AlbumArt::gotInformation(const Xmms::bin& res) {
	if(imageBuffer.isOpen())
	imageBuffer.close();
	
	imageBuffer.setData((const char*)(res.c_str()), res.size());
	setImage(0);

	return true;
}

bool AlbumArt::sentInformation(const std::string& res) {
	conn->medialib.entryPropertySet(id,"picture_front",res,"client/konfetka")();
	return true;
}

#endif





