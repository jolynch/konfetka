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
	albumArt=new AlbumArt(conn);
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
	wiki=new WikiView(conn,parent,f);
	lyrics=new LyricBrowser(conn);
	//JOEY MOVED THESE TWO LINES UP FOR OPTIONS
	//visualization=new Visualization(conn,this, NULL);
	newVis = new BasicVis(conn,this);
	//DONE

	netInfo->addTab(info, "About");
	netInfo->addTab(infoeditor, "Edit Info");
	netInfo->addTab(wiki, "Artist Info");
	netInfo->addTab(lyrics, "Lyrics");
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
	QObject::connect(conn,SIGNAL(currentId(int)),
				this,SLOT(updateSong(int)));
	QObject::connect(conn,SIGNAL(currentId(int)),
				albumArt,SLOT(fetchXML(int)));
	QObject::connect(conn,SIGNAL(changeStatus(Xmms::Playback::Status)),
				this,SLOT(updateStatus(Xmms::Playback::Status)));
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
	if(finalString==NULL) finalString=new QString();
	finalString->clear();
	finalString->append(statusString.c_str());
	finalString->append(infoString);
	songInfo->setText(*finalString);
	}

void RearPanel::updateSong(int id)
	{
	MlibData * mlib=((MlibData *)conn->getDataBackendObject(DataBackend::MLIB));
	infoString=mlib->getInfo(QString("artist"),id).toString();
	infoString.append("\n");
	infoString.append(mlib->getInfo(QString("title"),id).toString());
	infoString.append("\n");
	infoString.append(mlib->getInfo(QString("album"),id).toString());
	infoString.append("\n");
	finalString->clear();
	finalString->append(statusString.c_str());
	finalString->append(infoString);
	songInfo->setText(*finalString);
	wiki->myWiki->parseUrl(id);
	wiki->setHome(id);
	lyrics->parseUrl(id);
	}

//JOEY
void RearPanel::createNewVis() {
	delete newVis;
	newVis = new BasicVis(conn,this);
	layout->addWidget(newVis,0,2);
}
//DONE
#endif
