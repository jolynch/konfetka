#ifndef REARPANEL_CPP
#define REARPANEL_CPP
#include <time.h>
#include "rearpanel.h"
RearPanel::RearPanel(DataBackend * c,QRect deskRect,QWidget * parent,Qt::WindowFlags f):QWidget(parent,f)
	{
	p=parent;
	conn=c;
	this->deskRect = deskRect;
	infoeditor=new InfoEditor(conn);
	int wid = deskRect.width();
	songInfo=new QLabel(this);
	songInfo->setFrameStyle(QFrame::StyledPanel|QFrame::Sunken);
	songInfo->setAlignment(Qt::AlignCenter);
	songInfo->setObjectName("songInfo");
	statusString="";
	infoString="";
	finalString=new QString();
	albumArt=new AlbumArt();
	netInfo=new QTabWidget();
	netInfo->setTabPosition(QTabWidget::South);
	info=new QWidget();
	logo=new QLabel();
	logo->setPixmap(*(new QPixmap(":images/logoNoXmms2-secondTry.png")));
	logo->setAlignment(Qt::AlignCenter);
	authors=new QLabel("Powered by XMMS2.\nCode:\nAnton Frolenkov\nJoe E. Lynch");
	authors->setAlignment(Qt::AlignCenter);
	//random=new QLabel("who | grep -i blonde | date && (cd $HOME; unzip; touch; strip; finger; mount; fsck; yes; unmount; make clean); sleep");
	//random=new QLabel("Instead of office chair package contained bobcat. Would not buy again.");
	srand ( time(NULL) );
	/*if(system("fortune > /dev/null")==0)
		if(rand()%2==0) random=getRandom(true);
		else random=getRandom(false);
	else*/
		random=getRandom(false);
	random->setAlignment(Qt::AlignCenter);
	random->setWordWrap(true);
	random->setTextInteractionFlags(random->textInteractionFlags() | Qt::TextSelectableByMouse);
	infoLayout=new QGridLayout();
	infoLayout->addWidget(logo,0,0);
	infoLayout->addWidget(authors,1,0);
	infoLayout->addWidget(random,2,0);
	infoLayout->setRowMinimumHeight(0,175);
//	infoLayout->setRowMinimumHeight(1,100);
	info->setLayout(infoLayout);
	wiki=new WikiView(parent,f);
	lyrics=new LyricBrowser();
	options=new QScrollArea();
	//JOEY MOVED THESE TWO LINES UP FOR OPTIONS
	//visualization=new Visualization(conn,this, NULL);
	newVis = new BasicVis(conn,deskRect.height(),deskRect.width(),this, NULL);
	//DONE
	opt=new Options();

	//JOEY ADDED THIS FOR OPTIONS / other stuff
	QObject::connect(opt, SIGNAL(stayOnTop(bool)), p, SLOT(slotStayOnTop(bool)));
	connect(opt,SIGNAL(reflectImage(bool)),albumArt,SLOT(slotReflect(bool)));
	connect(opt,SIGNAL(newVis()),this,SLOT(createNewVis()));
	connect(opt,SIGNAL(updatePlaylistHeaders(QList<QString>)),this,SIGNAL(sigUpdateHeaders(QList<QString>)));
	connect(opt,SIGNAL(doubleClick(bool)),this,SIGNAL(sigDoubleClick(bool)));
	connect(opt,SIGNAL(searchWhat(int)),this,SIGNAL(sigSearchWhat(int)));
	//DONE

	options->setWidget(opt);
	netInfo->addTab(info, "About");
	netInfo->addTab(infoeditor, "Edit Info");
	netInfo->addTab(wiki, "Artist Info");
	netInfo->addTab(lyrics, "Lyrics");
	netInfo->addTab(options, "Options");
	equalizer=new Equalizer(conn,(int)(wid*(2.0/7)),this,NULL);
	layout=new QGridLayout();
	layout->addWidget(songInfo,0,0);
	layout->addWidget(albumArt,1,0);
	layout->addWidget(netInfo,0,1,2,1);
 	//layout->addWidget(view,0,2);
	layout->addWidget(newVis,0,2);
	//layout->addWidget(visualization,0,2);
	layout->addWidget(equalizer,1,2);
	layout->setRowMinimumHeight(0,180);
	layout->setRowMinimumHeight(1,180);
	////std::cout<<deskRect.width()<<std::endl;
	////std::cout<<(int)(wid*(2.0/7))<<std::endl;
	layout->setColumnMinimumWidth(0,(int)(wid*(2.0/7)));
	layout->setColumnMinimumWidth(1,(int)(wid*(3.0/7)));
	layout->setColumnMinimumWidth(2,(int)(wid*(2.0/7)));
	layout->setSpacing(5);
	this->setLayout(layout);
	QObject::connect(conn,SIGNAL(newSong(Xmms::PropDict)),
				this,SLOT(updateSong(Xmms::PropDict)));
	QObject::connect(conn,SIGNAL(newSong(Xmms::PropDict)),
				albumArt,SLOT(fetchXML(Xmms::PropDict)));
	QObject::connect(conn,SIGNAL(changeStatus(Xmms::Playback::Status)),
				this,SLOT(updateStatus(Xmms::Playback::Status)));
	conn->playback.currentID()(Xmms::bind(&DataBackend::newSongResponse, conn));
	conn->playback.getStatus()(Xmms::bind(&DataBackend::getstatus, conn));
	}

QLabel * RearPanel::getRandom(bool fortune)
	{
	QTextStream textstream;
	QLabel * out;
	//srand ( time(NULL) );
	if(fortune)
		{
		QProcess * p=new QProcess();
		textstream.setDevice(p);
		p->start("fortune");
		p->waitForFinished();
		QString s=textstream.readAll();
		if(s.length()<150)
			out=new QLabel(s);
		else
			fortune=false;
		}
	if(!fortune)
		{
		QFile * f=new QFile(":weirdline");
		f->open(QFile::ReadOnly);
		textstream.setDevice(f);
		QString s=textstream.readAll();
		delete f;
		QStringList sl=s.split("\n");
		int r=rand()%sl.size();
		out=new QLabel(sl[r]);
		}
	return out;
	}

RearPanel::~RearPanel()
	{
	delete songInfo;
	delete albumArt;
	delete netInfo;
// 	delete visualization;
	delete newVis;
	delete equalizer;
	//delete info;
	//delete infoLayout;
	delete finalString;
	//delete logo;
	//delete authors;
	//delete opt;
	//delete wiki;
	//delete lyrics;
	delete layout;
	}

AlbumArt* RearPanel::getAlbumArt() {
return albumArt;
}

void RearPanel::updateStatus(Xmms::Playback::Status status)
	{
	{using namespace std;
	switch(status)
		{
		case Xmms::Playback::STOPPED:
		statusString="Stopped:\n";
		break;
		case Xmms::Playback::PLAYING:
		statusString="Playing:\n";
		break;
		case Xmms::Playback::PAUSED:
		statusString="Paused:\n";
		break;
		}}
	//delete finalString;
	finalString=new QString();
	finalString->append(statusString.c_str());
	finalString->append(infoString.c_str());
	songInfo->setText(*finalString);
	}
void RearPanel::updateSong(Xmms::PropDict info)
	{
	{
	using namespace std;
		try {infoString=info.get<string>("artist");}
		catch(Xmms::no_such_key_error& err )
			{infoString="No Artist";}
		infoString+="\n";
		try {infoString+=info.get<string>("title");}
		catch(Xmms::no_such_key_error& err )
			{infoString+="No Title";}
		infoString+="\n";
		try {infoString+=info.get<string>("album");}
		catch(Xmms::no_such_key_error& err )
			{infoString+="No Album";}
		infoString+="\n";
	}
	finalString->clear();
	finalString->append(statusString.c_str());
	finalString->append(infoString.c_str());
	songInfo->setText(*finalString);
	wiki->myWiki->parseUrl(info);
	wiki->setHome(info);
	lyrics->parseUrl(info);
	}

//JOEY
void RearPanel::createNewVis() {
	delete newVis;
	newVis = new BasicVis(conn,deskRect.height(),deskRect.width(),this, NULL);
	layout->addWidget(newVis,0,2);
}
//DONE
#endif
