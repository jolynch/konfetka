#ifndef MINIMODE_CPP
#define MINIMODE_CPP
#include "minimode.h"

MiniMode::MiniMode(DataBackend * c,AlbumArt * a,QWidget * p, Qt::WindowFlags f):QWidget(p,f)
	{	
	//stylesheets
	setObjectName ("miniMode");
	conn = c;
	parentAlbArt = a;
	timeMode = true;
	miniVis =  new BasicVis(c,this);
	listOfWidgets.append(miniVis);	
	setWindowIcon(QIcon(":images/logomini"));

	setGeometry(50,50,400,200);
	setFixedSize (400, 200);
	setBackgroundRole (QPalette::Window);

	layout = new QGridLayout();
	
	artLabel = new QLabel("AlbumArt");
	listOfWidgets.append(artLabel);
	songInfo = new QLabel();
	listOfWidgets.append(songInfo);
		
	play = new QPushButton(QIcon(":images/play_button.png"),"");
	play->setObjectName("miniButton");
	play->setFixedSize(50,20);
	pause = new QPushButton(QIcon(":images/pause_button.png"),"");
	pause->setObjectName("miniButton");
	pause->setFixedSize(50,20);
	stop = new QPushButton(QIcon(":images/stop_button.png"),"");
	stop->setObjectName("miniButton");
	stop->setFixedSize(50,20);
	back = new QPushButton(QIcon(":images/back_button.png"),"");
	back->setObjectName("miniButton");
	back->setFixedSize(50,20);
	forward = new QPushButton(QIcon(":images/next_button.png"),"");
	forward->setObjectName("miniButton");
	forward->setFixedSize(50,20);
	quit = new QPushButton(QIcon(":images/quit_button.png"),"");
	quit->setObjectName("miniButton");
	quit->setFixedSize(50,20);
		
	position = new SongPositionSlider(conn,Qt::Horizontal,this);
	position->setObjectName("miniSlider");
	timeButton = new QPushButton("Time");
	timeButton->setObjectName("miniTimeButton");
	timeButton->setFixedSize(50,20);
	volume = new QSlider(Qt::Horizontal);
	volume->setObjectName("miniSlider");
	volume->setRange(0,100);
	
	connect(parentAlbArt,SIGNAL(newPixmap(QPixmap)),this,SLOT(loadNewPixmap(QPixmap)));
	
	connect(conn,SIGNAL(currentId(int)),this,SLOT(setNewInfo(int)));
	connect(position,SIGNAL(timeChanged(int)),this,SLOT(changeTimeButton()));
	connect(timeButton,SIGNAL(clicked()),this,SLOT(toggleTimeMode()));
	
	connect(back, SIGNAL(clicked()), conn, SLOT(playPreviousSong()));
	connect(play, SIGNAL(clicked()), this, SLOT(slotPlay()));
	connect(pause, SIGNAL(clicked()), this, SLOT(slotPause()));
	connect(stop, SIGNAL(clicked()), this, SLOT(slotStop()));
	connect(forward, SIGNAL(clicked()), conn, SLOT(playNextSong()));

	connect(quit, SIGNAL(clicked()),this, SIGNAL(hideMe()));

//	connect(this,SIGNAL(changeSongRel(int)),conn,SIGNAL(changeSong(int)));

	connect(volume,SIGNAL(valueChanged(int)),this,SIGNAL(volumeValueChanged(int)));
	connect(this,SIGNAL(setVolumeValue(int)),volume,SLOT(setValue(int)));


	layout->addWidget(artLabel,0,0,2,2);
	layout->addWidget(songInfo,0,2,2,2);
	layout->addWidget(miniVis,0,4,2,2);
	layout->addWidget(volume,2,0,1,2);
	layout->addWidget(position,2,2,1,3);
	layout->addWidget(timeButton,2,5,1,1);
	
	layout->addWidget(back,3,0,1,1);
	layout->addWidget(pause,3,1,1,1);
	layout->addWidget(play,3,2,1,1);
	layout->addWidget(stop,3,3,1,1);
	layout->addWidget(forward,3,4,1,1);
	layout->addWidget(quit,3,5,1,1);

		for(int i=0;i<layout->columnCount();i++) {
		layout->setColumnMinimumWidth(i,66);
		}/*
		for(int i=0;i<layout->rowCount();i++) {
			if(i>2) {
			layout->setRowMinimumHeight(i,60);
			}
			else
			layout->setRowMinimumHeight(i,40);
		}*/
	layout->setRowMinimumHeight(0,70);
	layout->setRowMinimumHeight(1,70);

	this->setLayout(layout);
	songInfo->update();
	highlight = false;
	setMouseTracking (true);


// 	show();
	
	
}

void MiniMode::leaveEvent(QEvent *event) {
	highlight = false;
	update();
}

void MiniMode::resizeEvent(QResizeEvent* event) {
	QPixmap pixmap(size());
	QPainter painter(&pixmap);
	painter.setRenderHint(QPainter::HighQualityAntialiasing);
	painter.fillRect(pixmap.rect(), Qt::white);
	painter.setBrush(Qt::black);
	painter.drawRoundRect(pixmap.rect(),10,10);
	setMask(pixmap.createMaskFromColor(Qt::white));

}


void MiniMode::mouseMoveEvent(QMouseEvent *event) {
	if(event->y()<10) {
// 	std::cout<<"LESS THAN"<<std::endl;
	highlight=true;
	update();
	} else {
// 	std::cout<<"NOT"<<std::endl;
	highlight = false;
	update();
	}

	if (event->buttons() & Qt::LeftButton) {
	QPoint pnt = event->globalPos() - whereAmI;
	int delta = 25;	
		QDesktopWidget *desktop = QApplication::desktop();
		QRect deskRect;
		deskRect = desktop->availableGeometry(0);
		
		if(pnt.x()-delta<0)
		pnt.setX(0);
		if(pnt.y()-delta<0)
		pnt.setY(0);
		if(pnt.x()+width()+delta>deskRect.width())
		pnt.setX(deskRect.width()-width());
		if(pnt.y()+height()+delta>deskRect.height())
		pnt.setY(deskRect.height()-height());
			

		move(pnt);
	event->accept();
	}
}

void MiniMode::mousePressEvent(QMouseEvent *event) {
	if(position->underMouse() || volume->underMouse()) {
	event->ignore();
	return;
	}
	if(event->button() == Qt::RightButton && event->y()<10) {
		toggleSize();
	}
	if (event->button() == Qt::LeftButton) {
// 	if(event->x() < position
	whereAmI = event->globalPos() - frameGeometry().topLeft();
	event->accept();
	}
}

void MiniMode::paintEvent(QPaintEvent *event) {

	QPainter painter(this);

	QStyleOption opt;
	opt.init(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

	painter.setRenderHint(QPainter::HighQualityAntialiasing);
	
	if(height()>100)
 	painter.drawText(QRect(140,0,artLabel->width(),artLabel->height()),
	Qt::AlignLeft | Qt::AlignVCenter,curSongInfo);
	if(highlight) {
// 	std::cout<<"here"<<std::endl;
	painter.setBrush(QColor(150,180,190,100));
	painter.setPen(Qt::NoPen);
	painter.drawRect(0,0,width(),10);
	}
	else {
// 	std::cout<<"back"<<std::endl;
	painter.setBrush(Qt::transparent);
	painter.setPen(Qt::NoPen);
	painter.drawRect(0,0,width(),10);
	}
}


/*void MiniMode::slotBack() {
	emit changeSongRel(-1);
}*/

void MiniMode::slotPlay()
	{
	conn->playback.start()(Xmms::bind(&DataBackend::scrapResult, conn));
}

void MiniMode::slotPause()
	{
	conn->playback.pause()(Xmms::bind(&DataBackend::scrapResult, conn));
// 	timer->stop();
// 	conn->getstatus(stat);
}

void MiniMode::slotStop()
	{
	conn->playback.stop()(Xmms::bind(&DataBackend::scrapResult, conn));
// 	timer->stop();
// 	time=-1;
// 	updateTimer();
}

/*void MiniMode::slotForward()
	{
	emit changeSongRel(1);
}*/

void MiniMode::setVolume() {
}
void MiniMode::setInfo() {
}

void MiniMode::loadNewPixmap(QPixmap image) {
	image = image.scaled(115,115,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
	artLabel->setPixmap(image);
}

void MiniMode::setNewInfo(int id){
	QString str;
	QStringList list;
	list<<"artist"<<"album"<<"title";
	curSongInfo.clear();
	MlibData * mlib=((MlibData *)conn->getDataBackendObject(DataBackend::MLIB));
	for(int i=0;i<list.size();i++) {
	str = list[i].mid(0,1).toUpper()+list[i].mid(1)+":";
		str+=mlib->getInfo(list[i],id).toString();
// 	str=str.left(20);
	if(i+1<list.size())
	str += "\n";
	curSongInfo.append(str);
	}
	setToolTip(curSongInfo);
	
	update();
}

void MiniMode::toggleTimeMode() {
	timeMode = !timeMode;
	changeTimeButton();
}

void MiniMode::changeTimeButton() {
	QTime foo (0,0,0);
	QString format = "h:m:ss";
		if(timeMode) {
		foo=foo.addMSecs(position->getPositiveTime()*1000);
		} else {
		foo=foo.addMSecs(position->getNegativeTime()*1000);
		format.prepend("-");
		}
			if(foo.hour()>0)
			timeButton->setText(foo.toString(format));
			else
			timeButton->setText(foo.toString(format.remove(format.indexOf("h"),2)));
}

void MiniMode::toggleSize() {
	for(int i=0;i<listOfWidgets.size();i++) {
		if(height()>100) {
		listOfWidgets.value(i)->hide();
		}
		else {
		listOfWidgets.value(i)->show();
		}
	}

	if(height()>100) {
	setFixedSize(400,50);
	layout->setSpacing(5);
	layout->setContentsMargins(10,0,10,0);
	layout->setRowMinimumHeight(0,0);
	layout->setRowMinimumHeight(1,0);
	}
	else  {
	layout->setRowMinimumHeight(0,70);
	layout->setRowMinimumHeight(1,70);
	layout->setSpacing(-1);
	layout->setMargin(11);
	setFixedSize(400,200);
	}
}





#endif

