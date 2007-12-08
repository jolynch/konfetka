#ifndef MAINBAR_CPP
#define MAINBAR_CPP
#include "mainbar.h"

using namespace std;
MainBar::MainBar(DataBackend * c,QWidget * papa,
			QWidget * parent, Qt::WindowFlags f):QWidget(parent, f)
	{
	stat = XMMS_PLAYBACK_STATUS_PLAY;
	muted=false;
	if(f==0)
		mini=false;
	else
		mini=true;
	p=papa;
	conn=c;
	conn->getstatus(stat);
	scrollInfo="";
	if(mini)
		{
		QDesktopWidget *desktop = QApplication::desktop();
		QRect deskRect;
		deskRect = desktop->availableGeometry(0);
	 	screenW = deskRect.width();
	 	screenH = deskRect.height();
		this->setGeometry(deskRect.x(),deskRect.y(),screenW,50);
		}
	channels=0;
	layout = new QGridLayout();

	playIcon = new QIcon(":images/play_button.png");
	stopIcon = new QIcon(":images/stop_button.png");
	pauseIcon = new QIcon(":images/pause_button.png");
	nextIcon = new QIcon(":images/next_button.png");
	backIcon = new QIcon(":images/back_button.png");
	quitIcon = new QIcon(":images/quit_button.png");
	volumeIcon = new QIcon(":images/volume_button_sound.png");
	if(mini)
		miniIcon=new QIcon(":images/un_minimize.png");
	else
		miniIcon=new QIcon(":images/minimize.png");
	muteIcon = new QIcon(":images/volume_button_NOsound.png");
	stopButton = new QToolButton();
	stopButton->setIcon(*stopIcon);
	playButton=new QToolButton();
	playButton->setIcon(*playIcon);
	pauseButton=new QToolButton;
	pauseButton->setIcon(*pauseIcon);
	backButton=new QToolButton();
	backButton->setIcon(*backIcon);
	forwardButton=new QToolButton();
	forwardButton->setIcon(*nextIcon);
	volumeButton = new QToolButton();
	volumeButton->setIcon(*volumeIcon);
	miniButton=new QToolButton();
	miniButton->setIcon(*miniIcon);


	//forwardButton=new QPushButton("Forward");

	positionSlider = new SongPositionSlider(conn,Qt::Horizontal,this);

	positionTime=new QLabel();

	positionMinusTime = new QLabel();

	volume = new QLabel();
	volume->setText("Volume");

	TitleBar = new QLabel();
	TitleBar -> setText("This will be replaced");
	TitleBar->setObjectName("titleBar");

	TitleBar->setFrameStyle(QFrame::StyledPanel);
	positionTime->setAlignment(Qt::AlignCenter);
	positionMinusTime->setAlignment(Qt::AlignCenter);
	volume->setAlignment(Qt::AlignCenter);
	TitleBar->setAlignment(Qt::AlignCenter);

	volumeSlider = new QSlider(Qt::Horizontal);
	volumeSlider->setRange(0, 100);
	volumeSlider->setValue(100);


	optionsButton=new QPushButton("Options");

//	makeSmallButton=new QPushButton("Hide");

	quitButton=new QToolButton();
	quitButton->setIcon(*quitIcon);

// 	conn->playback.getPlaytime()(Xmms::bind(&DataBackend::getCurPlaytime, conn));
// 	conn->playback.volumeGet()(Xmms::bind(&DataBackend::volumeResponse, conn));

	scrollTimer = new QTimer(this);
	scrollTimer->start(500);

	layout->addWidget(volumeSlider,1,1,1,2);
	layout->addWidget(positionTime,0,0,1,1);
	//layout->setColumnStretch ( 1, 0 );
	layout->addWidget(volumeButton,1,0,1,1);
	layout->addWidget(positionSlider,0,1,1,8);
	layout->addWidget(positionMinusTime,0,9,1,1);
	layout->addWidget(miniButton,0,10);
	layout->addWidget(TitleBar,1,3,1,2);
	layout->addWidget(backButton,1,5);
	layout->addWidget(pauseButton,1,6);
	layout->addWidget(playButton,1,7);
	layout->addWidget(stopButton,1,8);
	layout->addWidget(forwardButton,1,9);
	layout->addWidget(quitButton,1,10);

	layout->setColumnMinimumWidth(0,30);
	layout->setColumnMinimumWidth(9,30);

	this->setLayout(layout);

	QObject::connect(backButton, SIGNAL(clicked()), conn, SLOT(playPreviousSong()));

	QObject::connect(volumeButton, SIGNAL(clicked()),this, SLOT(slotMute()));
//	QObject::connect(shortcut, SIGNAL(activated()), this, SLOT(slotHide()));

	QObject::connect(playButton, SIGNAL(clicked()), this, SLOT(slotPlay()));

	QObject::connect(pauseButton, SIGNAL(clicked()), this, SLOT(slotPause()));

	QObject::connect(stopButton, SIGNAL(clicked()), this, SLOT(slotStop()));

	QObject::connect(forwardButton, SIGNAL(clicked()), conn, SLOT(playNextSong()));

	QObject::connect(quitButton, SIGNAL(clicked()), p, SLOT(slotQuit()));

	connect(positionSlider,SIGNAL(timeChanged(int)),this,SLOT(updateTime()));

	QObject::connect(volumeSlider,SIGNAL(valueChanged(int)),this,SLOT(slotSetVolume(int)));

	QObject::connect(scrollTimer, SIGNAL(timeout()), this, SLOT(slotScroll()));

	QObject::connect(conn,SIGNAL(currentId(int)),this,SLOT(slotUpdateInfo(int)));
	QObject::connect(conn,SIGNAL(volumeChanged(Xmms::Dict)),
					this,SLOT(slotGetVolume(Xmms::Dict)));
	if(mini)
		QObject::connect(miniButton,SIGNAL(clicked()),this,SLOT(slotMini()));
	else
		QObject::connect(miniButton,SIGNAL(clicked()),p,SLOT(slotHide()));
	QObject::connect(conn,SIGNAL(changeStatus(Xmms::Playback::Status)),
				this,SLOT(newStatus(Xmms::Playback::Status)));
//	connect(this,SIGNAL(changeSongRel(int)),conn,SIGNAL(changeSong(int)));
}
	

MainBar::~MainBar()
	{
	delete backButton;
	delete playButton;
	delete pauseButton;
	delete stopButton;
	delete forwardButton;
	delete positionSlider;
	delete optionsButton;
//	delete makeSmallButton;
	delete positionTime;
	delete quitButton;
	delete volumeSlider;
	}

/*void MainBar::slotBack()
	{
	emit changeSongRel(-1);
// 	conn->playlist.setNextRel(-1)(Xmms::bind(&DataBackend::scrapResultI, conn));
// 	conn->playback.tickle()(Xmms::bind(&DataBackend::scrapResult, conn));
	}*/

void MainBar::slotPlay()
	{
	conn->playback.start()(Xmms::bind(&DataBackend::scrapResult, conn));
	}

void MainBar::slotPause()
	{
	conn->playback.pause()(Xmms::bind(&DataBackend::scrapResult, conn));
	}

void MainBar::slotStop()
	{
	conn->playback.stop()(Xmms::bind(&DataBackend::scrapResult, conn));
	}

/*void MainBar::slotForward()
	{
	emit changeSongRel(1);
// 	conn->playlist.setNextRel(1)(Xmms::bind(&DataBackend::scrapResultI, conn));
// 	conn->playback.tickle()(Xmms::bind(&DataBackend::scrapResult, conn));
	}*/

/*All stuff to do with seeking and timing of the song*/


//Volume

void MainBar::slotSetVolume(int vol)
	{
	if (channels == 1)
		{
		conn->playback.volumeSet("master",vol)(Xmms::bind(&DataBackend::scrapResult, conn));
		return;
		}
	conn->playback.volumeSet ("left",vol)(Xmms::bind(&DataBackend::scrapResult, conn));
	conn->playback.volumeSet ("right",vol)(Xmms::bind(&DataBackend::scrapResult, conn));
	}

void MainBar::slotGetVolume(const Xmms::Dict& val)
	{
	if (val.contains ("master"))
		{
		if(!muted)
		volumeSlider->setValue(val.get<uint32_t> ("master"));
		channels = 1;
		}
	else if (val.contains ("left") && val.contains ("right"))
		{
		if(!muted)
		volumeSlider->setValue((val.get<uint32_t> ("left")+val.get<uint32_t> ("right")) / 2);
		channels = 2;
		}
	emit volumeChanged(volumeSlider->value());
	}

void MainBar::updateTime() {
	QTime tmpTime(0,0,0);
	QString format = "h:mm:ss";
	tmpTime = tmpTime.addMSecs(1000 * positionSlider->getPositiveTime());
		if(tmpTime.hour()>0)
		positionTime->setText(tmpTime.toString(format));
		else
		positionTime->setText(tmpTime.toString(format.remove(format.indexOf("h"),2)));
	format.prepend("-");
	tmpTime = QTime(0,0,0);
	format = "h:mm:ss";
	tmpTime = tmpTime.addMSecs(1000 * positionSlider->getNegativeTime());
		if(tmpTime.hour()>0)
		positionMinusTime->setText(tmpTime.toString(format));
		else
		positionMinusTime->setText(tmpTime.toString(format.remove(format.indexOf("h"),2)));
	if(positionSlider->curType == positionSlider->STREAM) {
	positionTime->setText("STREAM");
	positionMinusTime->setText("");
	//positionMinusTime->setText("STREAM");
	}
	else if(positionSlider->curType == positionSlider->UNKNOWN) {
	positionTime->setText("ERROR");
	positionMinusTime->setText("");
	//positionMinusTime->setText("ERROR");
	}
}

void MainBar::slotUpdateInfo(int id)
{
QString display;

QPalette temp;
temp.setColor(QPalette::WindowText, QColor(150, 150, 200));
temp.setColor(QPalette::Window, QColor(0, 0, 0));
	MlibData * mlib=((MlibData *)conn->getDataBackendObject(DataBackend::MLIB));
	scrollInfo="Artist: ";
	scrollInfo+=(mlib->getInfo(QString("artist"),id).toString().toUtf8().data());
	scrollInfo+="  -  ";
	scrollInfo+="Title: ";
	scrollInfo+=(mlib->getInfo(QString("title"),id).toString().toUtf8().data());
	scrollInfo+="  -  ";
	scrollInfo+="Album: ";
	scrollInfo+=(mlib->getInfo(QString("album"),id).toString().toUtf8().data());
	scrollInfo+="     ";
for(int i=scrollInfo.length();i < TitleBar->width()/5;i++)
{scrollInfo+=" ";}

display.append(QString::fromUtf8(scrollInfo.c_str()));
TitleBar->setAlignment ( Qt::AlignCenter );
TitleBar -> setPalette(temp);
TitleBar->setMaximumSize(TitleBar->width(),50);
TitleBar -> setText(display);
emit infoChanged();
}

void MainBar::newStatus(Xmms::Playback::Status s) {
stat = s;
}

void MainBar::slotScroll()
{
string var;
var=scrollInfo.substr(0,1);
scrollInfo.erase(0,1);
scrollInfo+=var;
TitleBar -> setText(QString::fromUtf8(scrollInfo.c_str()));
}

void MainBar::slotMute()
{
if(muted) {
volumeButton->setIcon(*volumeIcon);
slotSetVolume(volVal);
muted=false;
}
else
{
volVal=volumeSlider->value();
volumeButton->setIcon(*muteIcon);
slotSetVolume(0);
volumeSlider->setValue(volVal);
muted=true;
}
}

/*void MainBar::slotHide()
{
if(this-> isVisible()) {
cout<<"yeha"<<endl;
this->hide();
}
else
{

cout<<"yeha"<<endl;
this->show();
}
}*/

void MainBar::slotMini()
	{
	p->show();
	this->hide();
	}

QString MainBar::curInfo() {
return QString::fromUtf8(scrollInfo.c_str());
}

#endif
