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
	if(mini)
		miniIcon=new QIcon(":images/un_minimize.png");
	else
		miniIcon=new QIcon(":images/minimize.png");
	stopButton = new QToolButton();
	stopButton->setIcon(QIcon(":images/stop_button.png"));
	playButton=new QToolButton();
	playButton->setIcon(QIcon(":images/play_button.png"));
	backButton=new QToolButton();
	backButton->setIcon(QIcon(":images/back_button.png"));
	forwardButton=new QToolButton();
	forwardButton->setIcon(QIcon(":images/next_button.png"));
	volumeButton = new QToolButton();
	volumeButton->setIcon(QIcon(":images/volume_button_sound.png"));
	miniButton=new QToolButton();
	miniButton->setIcon(*miniIcon);

	positionSlider = new SongPositionSlider(conn,Qt::Horizontal,this);

	positionTime=new QLabel();

	positionMinusTime = new QLabel();

	volume = new QLabel();
	volume->setText("Volume");

	infoBar = new NiceLabel();
	infoBar->setText("This will be replaced");
	infoBar->setObjectName("infoBar");
	infoBar->setFixedWidth(500);

	infoBar->setFrameStyle(QFrame::StyledPanel);
	positionTime->setAlignment(Qt::AlignCenter);
	positionMinusTime->setAlignment(Qt::AlignCenter);
	volume->setAlignment(Qt::AlignCenter);

	volumeSlider = new QSlider(Qt::Horizontal);
	volumeSlider->setRange(0, 100);
	volumeSlider->setValue(100);


	optionsButton=new QPushButton("Options");

	quitButton=new QToolButton();
	quitButton->setIcon(QIcon(":images/quit_button.png"));

	layout->addWidget(volumeSlider,1,1,1,2);
	layout->addWidget(positionTime,0,0,1,1);
	layout->addWidget(volumeButton,1,0,1,1);
	layout->addWidget(positionSlider,0,1,1,7);
	layout->addWidget(positionMinusTime,0,8,1,1);
	layout->addWidget(miniButton,0,9);
	layout->addWidget(infoBar,1,3,1,2);
	layout->addWidget(backButton,1,5);
	layout->addWidget(stopButton,1,6);
	layout->addWidget(playButton,1,7);
	layout->addWidget(forwardButton,1,8);
	layout->addWidget(quitButton,1,9);

	layout->setColumnMinimumWidth(0,30);
	layout->setColumnMinimumWidth(9,30);

	this->setLayout(layout);

	connect(backButton, SIGNAL(clicked()), conn, SLOT(playPreviousSong()));

	connect(volumeButton, SIGNAL(clicked()),this, SLOT(slotMute()));
//	QObject::connect(shortcut, SIGNAL(activated()), this, SLOT(slotHide()));

	connect(playButton, SIGNAL(clicked()), this, SLOT(slotPlay()));

	connect(stopButton, SIGNAL(clicked()), this, SLOT(slotStop()));

	connect(forwardButton, SIGNAL(clicked()), conn, SLOT(playNextSong()));

	connect(quitButton, SIGNAL(clicked()), p, SLOT(slotQuit()));

	connect(positionSlider,SIGNAL(timeChanged(int)),this,SLOT(updateTime()));

	connect(volumeSlider,SIGNAL(valueChanged(int)),this,SLOT(slotSetVolume(int)));

	connect(conn,SIGNAL(currentId(int)),this,SLOT(slotUpdateInfo(int)));
	connect(conn,SIGNAL(volumeChanged(Xmms::Dict)),
					this,SLOT(slotGetVolume(Xmms::Dict)));
	if(mini)
		connect(miniButton,SIGNAL(clicked()),this,SLOT(slotMini()));
	else
		connect(miniButton,SIGNAL(clicked()),p,SLOT(slotHide()));
	connect(conn,SIGNAL(changeStatus(Xmms::Playback::Status)),
				this,SLOT(newStatus(Xmms::Playback::Status)));
}
	

MainBar::~MainBar()
	{
	delete backButton;
	delete playButton;
	delete stopButton;
	delete forwardButton;
	delete positionSlider;
	delete optionsButton;
	delete positionTime;
	delete quitButton;
	delete volumeSlider;
	}

void MainBar::slotPlay() {
	if(stat == XMMS_PLAYBACK_STATUS_PLAY)
	conn->playback.pause()(Xmms::bind(&DataBackend::scrapResult, conn));
	else
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
	MlibData * mlib=((MlibData *)conn->getDataBackendObject(DataBackend::MLIB));
	scrollInfo=(mlib->getInfo(QString("artist"),id).toString())+"  -  ";
	scrollInfo+=(mlib->getInfo(QString("album"),id).toString())+"  -  ";
	scrollInfo+=(mlib->getInfo(QString("title"),id).toString());
	scrollInfo+=" (";scrollInfo+=(mlib->getInfo(QString("time"),id).toString());scrollInfo+=")";
	infoBar->setText(scrollInfo);
	emit infoChanged();
}

void MainBar::newStatus(Xmms::Playback::Status s) {
	stat = s;
	switch(stat) {
		case XMMS_PLAYBACK_STATUS_PLAY:
		playButton->setIcon(QIcon(":images/pause_button.png"));
		break;
		default:
		playButton->setIcon(QIcon(":images/play_button.png"));
	}
}

void MainBar::slotMute()
{
	if(muted) {
	volumeButton->setIcon(QIcon(":images/volume_button_sound.png"));
	slotSetVolume(volVal);
	muted=false;
	}
	else {
	volVal=volumeSlider->value();
	volumeButton->setIcon(QIcon(":images/volume_button_NOsound.png"));
	slotSetVolume(0);
	volumeSlider->setValue(volVal);
	muted=true;
	}
}

void MainBar::slotMini()
	{
	p->show();
	this->hide();
	}

QString MainBar::curInfo() {
	return scrollInfo;
}

#endif
