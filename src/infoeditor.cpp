#ifndef INFOEDITOR_CPP
#define INFOEDITOR_CPP
#include "infoeditor.h"

InfoEditor::InfoEditor(DataBackend * c,QWidget* parent,Qt::WindowFlags f):QWidget(parent,f)
	{
	stopped=false;
	curId=0;
	conn=c;
	layout=new QGridLayout();
	title=new QLabel("Title:");
	layout->addWidget(title,0,0);
	artist=new QLabel("Artist:");
	layout->addWidget(artist,1,0);
	album=new QLabel("Album");
	layout->addWidget(album,2,0);
	year=new QLabel("Year:");
	layout->addWidget(year,3,0);
	track=new QLabel("Track:");
	layout->addWidget(track,3,3);
	genre=new QLabel("Genre:");
	layout->addWidget(genre,4,0);
	title_=new QLineEdit();
	layout->addWidget(title_,0,1,1,5);
	artist_=new QLineEdit();
	layout->addWidget(artist_,1,1,1,5);
	album_=new QLineEdit();
	layout->addWidget(album_,2,1,1,5);
	year_=new QLineEdit();
	year_->setInputMask("0000;_");
	layout->addWidget(year_,3,1,1,2);
	track_=new QLineEdit();
	track_->setInputMask("000;_");
	layout->addWidget(track_,3,4,1,2);
	genre_=new QLineEdit();
	layout->addWidget(genre_,4,1,1,5);
	uneditable=new QFrame();
	uneditable->setFrameStyle(QFrame::StyledPanel|QFrame::Sunken);
	uneditableLayout=new QGridLayout();
	notPlaying=new QLabel("Nothing playing");
	notPlaying->setAlignment(Qt::AlignCenter);
	uneditableLayout->addWidget(notPlaying,0,0,3,2);
	url=new QLabel("URL:");
	uneditableLayout->addWidget(url,0,0,1,2);
	bitrate=new QLabel("Bitrate:");
	uneditableLayout->addWidget(bitrate,1,0);
	channels=new QLabel("Channels:");
	uneditableLayout->addWidget(channels,1,1);
	timesPlayed=new QLabel("Time played:");
	uneditableLayout->addWidget(timesPlayed,2,0,1,2);
	uneditable->setLayout(uneditableLayout);
	layout->addWidget(uneditable,5,0,3,6);
	done=new QPushButton();
	done->setIcon(QIcon(":images/check.png"));
	done->setToolTip("Commit changes");
	layout->addWidget(done,8,4);
	QObject::connect(done,SIGNAL(clicked()),this,SLOT(slotDone()));
	reset=new QPushButton();
	reset->setIcon(QIcon(":images/quit_button.png"));
	reset->setToolTip("Reset information");
	layout->addWidget(reset,8,5);
	this->setLayout(layout);
	QObject::connect(reset,SIGNAL(clicked()),this,SLOT(slotReset()));
	QObject::connect(conn,SIGNAL(changeStatus(Xmms::Playback::Status)),
				this,SLOT(newStatus(Xmms::Playback::Status)));
	QObject::connect(conn,SIGNAL(currentId(int)),this,SLOT(newId(int)));
	}

void InfoEditor::slotDone()
	{
	if(stopped) return;
	conn->medialib.entryPropertySet(curId,"artist",artist_->text().toStdString())
								(Xmms::bind(&DataBackend::scrapResult, conn));
	conn->medialib.entryPropertySet(curId,"title",title_->text().toStdString())
								(Xmms::bind(&DataBackend::scrapResult, conn));
	conn->medialib.entryPropertySet(curId,"album",album_->text().toStdString())
								(Xmms::bind(&DataBackend::scrapResult, conn));
	conn->medialib.entryPropertySet(curId,"genre",genre_->text().toStdString())
								(Xmms::bind(&DataBackend::scrapResult, conn));
	conn->medialib.entryPropertySet(curId,"date",year_->text().toStdString())
								(Xmms::bind(&DataBackend::scrapResult, conn));
	std::string foo=track_->text().toStdString();
	while(foo.find("_")!=std::string::npos)
		foo.erase(foo.find("_"),1);
	conn->medialib.entryPropertySet(curId,"tracknr",atoi(foo.c_str()))
								(Xmms::bind(&DataBackend::scrapResult, conn));
	conn->playback.currentID()(Xmms::bind(&DataBackend::newSongResponse, conn));
	}

void InfoEditor::slotReset()
	{
	if(stopped) return;
	conn->playback.currentID()(Xmms::bind(&DataBackend::newSongResponse, conn));
	}

void InfoEditor::newId(int id)
	{
	curId=id;
	if(stopped) return;
	MlibData * mlib=((MlibData *)conn->getDataBackendObject(DataBackend::MLIB));
	artist_->setText(mlib->getInfo(QString("artist"),id).toString());
	title_->setText(mlib->getInfo(QString("title"),id).toString());
	album_->setText(mlib->getInfo(QString("album"),id).toString());
	genre_->setText(mlib->getInfo(QString("genre"),id).toString());
	year_->setText(mlib->getInfo(QString("date"),id).toString());
	track_->setText(QString::number(mlib->getInfo(QString("tracknr"),id).toInt()));
	QString foo("");
	foo.append("Filename: ");
	try
		{
		QString bar=mlib->getInfo(QString("url"),id).toString();
		//std::cout<<bar.toStdString()<<std::endl;
		bar = QString::fromUtf8 (xmmsc_result_decode_url (NULL, bar.toAscii()));
		if(bar.startsWith("file://"))
				{
				bar.remove(0,bar.lastIndexOf("/")+1);
				foo.append(bar);
				}
		if(bar.startsWith("http://")||bar.startsWith("ftp://"))
				foo.append("<Internet stream>");
		}
	catch(...){}
	url->setText(foo);
	foo.clear();
	std::stringstream ss;
	foo.append("Bitrate: ");
	std::string tmp="";
	try
		{
		ss << (mlib->getInfo(QString("bitrate"),id).toInt())/1000; ss >> tmp;
		foo.append(tmp.c_str());
		foo.append(" kbit/s");
		}
	catch(...){//std::cout<<"bah"<<std::endl;}
	}
	bitrate->setText(foo);
	foo.clear();
	ss.clear();
	tmp="";
	foo.append("Channels: ");
	try
		{
		ss << mlib->getInfo(QString("channels"),id).toInt(); ss >> tmp;
		foo.append(tmp.c_str());
		}
	catch(...){}
	channels->setText(foo);
	foo.clear();
	ss.clear();
	tmp="";
	foo.append("Times played: ");
	try
		{
		ss << mlib->getInfo(QString("timesPlayed"),id).toInt(); ss >> tmp;
		foo.append(tmp.c_str());
		}
	catch(...){}
	timesPlayed->setText(foo);
	}

void InfoEditor::newStatus(Xmms::Playback::Status status)
	{
	if(status==XMMS_PLAYBACK_STATUS_STOP)
		{
		stopped=true;
		url->hide();
		bitrate->hide();
		channels->hide();
		timesPlayed->hide();
		notPlaying->show();
		artist_->setText(QString());
		artist_->setReadOnly(true);
		title_->setText(QString());
		title_->setReadOnly(true);
		album_->setText(QString());
		album_->setReadOnly(true);
		genre_->setText(QString());
		genre_->setReadOnly(true);
		year_->setText(QString());
		year_->setReadOnly(true);
		track_->setText(QString());
		track_->setReadOnly(true);
		}
	else
		{
		stopped=false;
		conn->playback.currentID()(Xmms::bind(&DataBackend::newSongResponse, conn));
		notPlaying->hide();
		url->show();
		bitrate->show();
		channels->show();
		timesPlayed->show();
		artist_->setReadOnly(false);
		title_->setReadOnly(false);
		album_->setReadOnly(false);
		genre_->setReadOnly(false);
		year_->setReadOnly(false);
		track_->setReadOnly(false);
		}
	}
#endif
