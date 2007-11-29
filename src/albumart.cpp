#ifndef ALBUMART_CPP
#define ALBUMART_CPP
#include "albumart.h"

using namespace std;

AlbumArt::AlbumArt(DataBackend * c)
	{
	conn=c;
	numBad=0; paintFix=true;
	numToGet=0;
	QSettings s;
	if(s.contains("reflectImage") && s.value("reflectImage").toInt() == Qt::Checked )
	toReflect=1;
	else
	toReflect=0;
	httpRequestAborted=0;
	albumPic = new QString();
	layout=new QHBoxLayout();
	progress = new QProgressBar(this);
	progress->setValue(4);
	progress->hide();
	currentArtIcon = new QIcon(":images/no_album150");
	currentArtLabel = new QLabel();
	currentArtLabel->setPixmap(*(new QPixmap(":images/no_album150")));
	currentArtLabel->setAlignment ( Qt::AlignCenter );
	
	http = new QHttp();
	xmlFile = NULL;

	fromThis = 0;
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

	imageFile = new QFile("curAlbumArt.jpg");
	
	center = QPixmap(":images/no_album150");
	//layout->addWidget(currentArtReflection);
	layout->addWidget(currentArtLabel);
	//layout->addWidget(currentArtReflectionRight);
	layout->setSpacing(0);
	this->setLayout(layout);
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
	//delete currentArtLabel;
	delete progress;
	delete layout;
	//delete currentArtIcon;
	currentArtLabel->deleteLater();
	delete http;
	if(xmlFile != NULL)
	delete xmlFile;
	delete imageFile;
	delete albumPic;
	}

void AlbumArt::fetchXML(int id_) {

	

	paintFix=true;
	string a;numBad=0;
	MlibData * mlib=((MlibData *)conn->getDataBackendObject(DataBackend::MLIB));
	id = id_;
	try{
		a+=mlib->getInfo(QString("album"),id).toString().toStdString();
		a+=" "+mlib->getInfo(QString("artist"),id).toString().toStdString();
		album=mlib->getInfo(QString("album"),id).toString();
		artist=mlib->getInfo(QString("artist"),id).toString();
/*		try {
		a+=info.get<string>("album");
		album = QString(info.get<string>("album").c_str());
		}
		catch(Xmms::no_such_key_error& err ) {
		numBad+=1;
		}
		try {
		a+=" "+info.get<string>("artist");
		artist = QString(info.get<string>("artist").c_str());
		}
		catch(Xmms::no_such_key_error& err ) {
		numBad+=1;
		}
*/	combo.clear();
	combo.append(a.c_str());
	}
	catch(...){}

	//cout<<"Would be fetching: "<<album->toStdString()<<endl;
	
		//Thanks amarok for the D1URM... code thing
	QString toUrl;
	toUrl.append("http://xml.amazon.com/onca/xml3?t=webservices-20&dev-t=D1URM11J3F2CEH&KeywordSearch=");
	toUrl.append(combo);
	toUrl.append("&mode=music&type=lite&locale=us&page=1&f=xml");

	QUrl url(toUrl);
	xmlFile = new QFile(artXmlPath);
		if (!xmlFile->open(QIODevice::ReadWrite)) {
		QMessageBox::information(this, tr("HTTP"),
					tr("Unable to save the xmlFile %1: %2.")
					.arg(artXmlPath).arg(xmlFile->errorString()));
		delete xmlFile;
		xmlFile = 0;
		return;
	}	
	http->setHost(url.host(), url.port() != -1 ? url.port() : 80);
	if (!url.userName().isEmpty())
	http->setUser(url.userName(), url.password());

   	httpRequestAborted = false;
	
	a=toUrl.toStdString();
	//cout<<album->toStdString()<<endl;
	int pos;
	while(a.find(" ",0)!=string::npos)
	{
	pos=a.find(" ",0);
	a.replace(pos,1,"%20");
	}
	toUrl.clear();
	toUrl.append(a.c_str());
	httpGetId = http->get(toUrl, xmlFile);

	curAlbumArtPath=Xmms::getUserConfDir().c_str();
	curAlbumArtPath.append("/clients/konfetka/curAlbumArt.jpg");

	QObject::connect(http, SIGNAL(done(bool)), this, SLOT(fetchImage(bool)));
	std::cout<<http->state()<<std::endl;
}

void AlbumArt::fetchImage(bool err)
	{

	if(err) {
	std::cout<<http->error()<<std::endl;
	setImage(1);
	return;
	}

	QObject::disconnect(http, 0, this, 0);
	xmlFile->flush();
	QDomDocument doc("art");

	QFile file2(artXmlPath);

    	if (!file2.open(QIODevice::ReadOnly))
        cout<<"Problem"<<endl;
    	if (!doc.setContent(&file2)) {
        file2.close();
    	}
    	file2.close();
	
	const QDomNode details = doc.documentElement().namedItem( "Details" );

	allCovers = doc.elementsByTagName("Details");
 	imageUrl=allCovers.item(numToGet).namedItem("ImageUrlLarge").firstChild().nodeValue();
	
	if(numToGet==0 && knownUrls.contains(id) && !fromThis) {
	imageUrl = knownUrls.value(id);
	}
	else
	knownUrls.insert(id,imageUrl);


	QDomNode node = details;
	imageUrl.remove(0,imageUrl.indexOf("com")+3);

	QString file = QCryptographicHash::hash(artist.toLower().toAscii() +"-"+album.toLower().toAscii(),QCryptographicHash::Md5).toHex();
	file = file+"-large";
	std::cout<<file.toStdString()<<std::endl;
	QString dirName = Xmms::getUserConfDir().c_str();
	dirName.append("/bindata");
	QDir  dir(dirName);
	dir.setFilter(QDir::NoDotAndDotDot);
	std::cout<<dirName.toStdString()<<"/"<<file.toStdString()<<std::endl;
	if(QFile::exists(dirName+"/"+file) && numToGet==0 && !fromThis) {
		std::cout<<"Using Local File"<<std::endl;
		curAlbumArtPath = dirName+"/"+file;
		
		if(imageFile->isOpen())
		imageFile->close();

		imageFile->setFileName(curAlbumArtPath);
		imageFile->open(QIODevice::ReadWrite);

		setImage(false);
	
		return;
	}

	if(!imageFile->isOpen()) {
	imageFile->setFileName(curAlbumArtPath);
	imageFile->open(QIODevice::ReadWrite);
	}
	http->setHost("images.amazon.com",80);
	http->get(imageUrl,imageFile);
	QObject::connect(http, SIGNAL(done(bool)), this, SLOT(setImage(bool)));
	fromThis = false;
	std::cout<<"xml "<<http->state()<<std::endl;

	}

void AlbumArt::setImage(bool err)
	{
	QObject::disconnect(http, 0, this, 0);
	if(err) {
	std::cout<<http->error()<<std::endl;
	return;
	}
	imageFile->flush();
		if(numBad==2||imageUrl.toStdString()==""||imageFile->size()<1000)
		{
		center = QPixmap(":images/no_album150");
		emit newPixmap(center);
		currentArtLabel->setPixmap(center);
		currentArtLabel->show();
		}
		else {
		currentArtLabel->hide();
		center = QPixmap(imageFile->fileName()); 
		if(center.isNull()) {
		center = QPixmap(":images/no_album150");
		emit newPixmap(center);
		currentArtLabel->setPixmap(center);
		currentArtLabel->show();
		return;
		}
		emit newPixmap(center);
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
		update();
		}

	QString file = QCryptographicHash::hash(artist.toLower().toAscii() +"-"+album.toLower().toAscii(),QCryptographicHash::Md5).toHex();
	file = file+"-large";
	std::cout<<file.toStdString()<<std::endl;
	QString dirName = Xmms::getUserConfDir().c_str();
	dirName.append("/bindata");
	if(QFile::exists(dirName+"/"+file)&&imageFile->fileName()!=(dirName+"/"+file)) {
	QFile::remove(dirName+"/"+file);
	}
	imageFile->copy(dirName+"/"+file);
// 	QFile::remove("data/AlbumArt/"+artist+"/"+album+".jpg");
// 	imageFile->copy("data/AlbumArt/"+artist+"/"+album+".jpg");

	imageFile->close();
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
	QString fileName = QFileDialog::getSaveFileName(this, "Save File",QDir::homePath ());
	if(fileName=="") return;
	
	if(!fileName.endsWith(".jpg",Qt::CaseInsensitive))
	fileName.append(".jpg");
		if(imageFile->size()>1000 && !currentArtLabel->isVisible()) {
			if(QFile::exists(fileName)) {
			QFile::remove(fileName); 
			}

			if(imageFile->copy(fileName)) {
			QMessageBox::information (NULL,"File Saved","File Saved Successfully!", QMessageBox::Ok);
			int ret= QMessageBox::question(NULL,"Open File Now","Would you like to open the file now?",
						QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes);
				if(ret == QMessageBox::Yes) {
				////std::cout<<fileName.toStdString()<<std::endl;
				QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
				}
			}
			else {
			QMessageBox::critical(NULL,"File NOT Saved","File NOT Saved! Please check permissions and try again.", QMessageBox::Ok);
			return;
			}

		}

}

void AlbumArt::makeRequest(){
	fromThis = true;
	paintFix=true;
	fetchImage(0);
}

/* end */

void AlbumArt::paintEvent(QPaintEvent * event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::HighQualityAntialiasing,true);
	
	if(currentArtLabel->isVisible() || allCovers.isEmpty() ){
		if(!currentArtLabel->isVisible())
		currentArtLabel->show();

		if(paintFix) {
		paintFix=false;
		this->setBackgroundRole(QPalette::Window);
		}
	painter.drawText(rect(), Qt::AlignHCenter | Qt::AlignBottom, "Image Not Available");
	}
	else{
	if(toReflect)
	painter.drawPixmap(QRect(0,0,this->width()/4,this->width()/2),left,QRect(0,0,87,175));
	painter.drawPixmap(QRect(this->width()/4,0,this->width()/2,this->width()/2),center,QRect(0,0,175,175));
	if(toReflect)
	painter.drawPixmap(QRect(3*(this->width()/4),0,this->width()/4,this->width()/2),right,QRect(0,0,87,175));
	}	
}

void AlbumArt::slotReflect(bool reflect) {
toReflect = reflect;
update();
}

#endif





