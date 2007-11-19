#ifndef MAINWINDOW_CPP
#define MAINWINDOW_CPP
#include "mainwindow.h"

MainWindow::MainWindow(QApplication * a, QWidget * parent, Qt::WindowFlags f):QWidget(parent, f)
	{
	locked=false;
	this->setWindowTitle ("Konfetka");
	papa=a;
	conn=new DataBackend(NULL, std::string("Konfetka"));
	QSettings s;
	QString foo=Xmms::getUserConfDir().c_str();
	foo.append("/clients/konfetka/stylesheet");
	QFile file(foo);
	QString sheet;
	if (file.open(QIODevice::ReadOnly))
	sheet = QString(file.readAll());
	qApp->setStyleSheet(sheet);
	animator=new QTimer();
	mainbar=new MainBar(conn,this,this,NULL);
	QDesktopWidget *desktop = papa->desktop();
	QRect deskRect;
	deskRect = desktop->availableGeometry(0);

	rearpanel=new RearPanel(conn,deskRect, this, NULL);	

		if(s.value("staysOnTop").toBool())
		minibar=new MiniMode(conn,rearpanel->getAlbumArt(),NULL,Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);
		else
		minibar=new MiniMode(conn,rearpanel->getAlbumArt(),NULL,Qt::FramelessWindowHint);
	
	connect(minibar,SIGNAL(volumeValueChanged(int)),mainbar,SLOT(slotSetVolume(int)));
	connect(mainbar,SIGNAL(volumeChanged(int)),minibar,SIGNAL(setVolumeValue(int)));
	medialibView=new MediaLib(conn,this,NULL);
	//playlistpanel=new PlaylistPanel(conn,this);
	screenW = deskRect.width();
	screenH = deskRect.height();
	height=450; currentHeight=450; step=-20;
	this->setGeometry(deskRect.x(),deskRect.y(),screenW,450);
// 	this->setFixedHeight(height);
// 	this->setFixedWidth(screenW);
	icon=new QSystemTrayIcon(QIcon(":images/logomini.png"));
	QObject::connect(icon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
						this,SLOT(clickResponse(QSystemTrayIcon::ActivationReason)));
	QObject::connect(mainbar,SIGNAL(infoChanged()),this,SLOT(trayTool()));

	//Joe
	//connect(rearpanel,SIGNAL(sigUpdateHeaders(QList<QString>)),playlistpanel,SLOT(updateHeaders(QList<QString>)));
	connect(rearpanel,SIGNAL(sigDoubleClick(bool)),medialibView,SLOT(changeDoubleClick(bool)));
	connect(rearpanel,SIGNAL(sigSearchWhat(int)),medialibView,SLOT(changeSearchAmount(int)));
	//-anton damn you and your abstraction of the god damn playlist code...

	QAction * temp;
	QMenu * menu=new QMenu();
	menu->setTitle("Quick Access");
	temp = menu->addAction(QIcon(":images/play_button.png"),"Play",mainbar,SLOT(slotPlay()));
	menu->addAction(QIcon(":images/pause_button.png"),"Pause",mainbar,SLOT(slotPause()));
	menu->addAction(QIcon(":images/stop_button.png"),"Stop",mainbar, SLOT(slotStop()));
	menu->addAction(QIcon(":images/next_button.png"),"Next Song",mainbar,SLOT(slotForward()));
	menu->addAction(QIcon(":images/back_button.png"),"Previous Song",mainbar,SLOT(slotBack()));
	
	menu->addSeparator();
	
	volumeLabel = new QWidgetAction(this);
	volLabel = new QLabel ("Volume");
	volLabel->setAlignment (Qt::AlignCenter);
	volumeLabel->setDefaultWidget(volLabel);
	menu->addAction(volumeLabel);

	volume = new QWidgetAction(this);
	slider = new QSlider(Qt::Horizontal);
	slider->setRange(0, 100);
	slider->setValue(100);
	connect(slider,SIGNAL(valueChanged(int)),mainbar,SLOT(slotSetVolume(int)));
	connect(mainbar,SIGNAL(volumeChanged(int)),slider,SLOT(setValue(int)));
	connect(minibar,SIGNAL(hideMe()),this,SLOT(toggle()));
	volume->setDefaultWidget(slider);
	menu->addAction(volume);

	menu->addSeparator();
	
	menu->addAction(QIcon(":images/toggle_button.png"),"Toggle Konfetka",this,SLOT(toggle())/*,QKeySequence(Qt::Key_F11)*/);
	menu->addAction(QIcon(":images/quit_button.png"),"Quit Konfetka",this,SLOT(slotQuit()));

	icon->setContextMenu(menu);
	icon->show();
//	CollectionEditor * edit=new CollectionEditor(conn,this);
	/*!!!!!!!!!!!!!!need to remove this hardcoding somehow!!!!!!!!!!!!!!!!!*/
	/*!Other than the panel handles being hard coded to being 187 high what hard coding?!*/
	QRect geom=this->geometry();
	mainbar->resize(geom.width(),75);
	mainbar->move(geom.x(),geom.height()-75);
	rearpanel->resize(geom.width()-40,geom.height()-75);
	rearpanel->move(geom.x()+20,0);
	leftSide=geom.x();
	rightSide=geom.width()-20;
	middle=geom.width()/2-10;
	mlibHH=rearpanel->height()-187;
	playHH=0;
	//playlist_handle=new Handle(":images/playlist_handle",this);
	//playlist=new Panel(leftSide,playHH,screenW,geom.height()-75,false,playlistpanel,this);
	//medialib=new Panel(rightSide,playHH,screenW,geom.height()-75,true,medialibView,this);
//	medialib=new Panel(rightSide,playHH,screenW,geom.height()-75,true,edit,this);
	//medialib_handle=new Handle(":images/medialib_handle",this);
	//playlist_handle->move(leftSide,playHH);
	//medialib_handle->move(rightSide,mlibHH);
	currentPlaylistPos=leftSide;
	currentMedialibPos=rightSide;
	currentState=konfetka::rearPanel;
	/*QObject::connect(playlist_handle,SIGNAL(clicked()),this,SLOT(slotPlaylistClicked()));
	QObject::connect(medialib_handle,SIGNAL(clicked()),this,SLOT(slotMedialibClicked()));
	QObject::connect(playlist_handle,SIGNAL(done()),this,SLOT(slotAnimationDone()));
	QObject::connect(medialib_handle,SIGNAL(done()),this,SLOT(slotAnimationDone()));
	QObject::connect(this, SIGNAL(movePlaylist(int,int,int,int)),
						playlist_handle,SLOT(moveThis(int,int,int,int)));
	QObject::connect(this, SIGNAL(resizePlaylist(int,int)),
						playlistpanel,SLOT(update(int,int)));
//	QObject::connect(this, SIGNAL(resizeMedialib(int,int)),
//						edit,SLOT(update(int,int)));
	QObject::connect(this, SIGNAL(movePlaylist(int,int,int,int)),
						playlist,SLOT(moveThis(int,int,int,int)));
	QObject::connect(this, SIGNAL(moveMedialib(int,int,int,int)),
						medialib_handle,SLOT(moveThis(int,int,int,int)));
	QObject::connect(this, SIGNAL(moveMedialib(int,int,int,int)),
						medialib,SLOT(moveThis(int,int,int,int)));
	QObject::connect(this, SIGNAL(stateChanged(konfetka::windowState,konfetka::windowState)),
			this, SLOT(slotChangeState(konfetka::windowState,konfetka::windowState)));
	QObject::connect(playlistpanel,SIGNAL(needRefresh()), this, SLOT(provideRefresh()));*/
	minibar->hide();
	PanelController *pc=new PanelController(width());
	//pc->registerPanel(new Panel(this,"PLAYLIST",new PlaylistPanel(conn,this)),PanelController::LEFTPANEL);
	pc->registerPanel(new Panel(this,"NEW PLIST",new PlaylistPanel_(conn)),PanelController::LEFTPANEL);
	Panel * mlibPanel=new Panel(this,"MEDIALIB",medialibView);
	PanelPtrList * mlibincompatlist=new PanelPtrList();
	mlibincompatlist->append(mlibPanel);
	pc->registerPanel(mlibPanel,PanelController::DOUBLE_SIDED_PANEL,mlibincompatlist);
	//pc->registerPanel(new Panel(this,"PLAYLIST",new PlaylistPanel(conn,this)),PanelController::RIGHTPANEL);

	conn->emitInitialQSettings();


	pc->registerPanel(new Panel(this,"FILESYSTEM",new FileBrowser(conn,this)),PanelController::RIGHTPANEL);
	}

void MainWindow::trayTool() {
QString info = mainbar->curInfo();
info = info.remove("Artist: ");
info = info.remove(info.indexOf("-"),info.lastIndexOf("-")-info.indexOf("-"));
info = info.remove("Title: ");
icon->setToolTip(info.trimmed());
}

void MainWindow::slotHide()
	{
	minibar->show();
	this->hide();
	}
void MainWindow::slotQuit()
	{
	delete conn;
	delete minibar;
	delete rearpanel;
	delete mainbar;
	delete slider;
	delete volume;
	delete volumeLabel;
	papa->quit();
	}

void MainWindow::slotStayOnTop(bool state)
	{
		if(state)
		minibar->setWindowFlags(Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);
		else
		minibar->setWindowFlags(Qt::FramelessWindowHint);
	}

/*void MainWindow::slotPlaylistClicked()
	{
	switch(currentState)
		{
		case konfetka::rearPanel:
			emit(slotChangeState(currentState,konfetka::playlistOnly));
			break;
		case konfetka::playlistOnly:
			emit(slotChangeState(currentState,konfetka::rearPanel));
			break;
		case konfetka::medialibOnly:
			emit(slotChangeState(currentState,konfetka::playlist_medialib));
			break;
		case konfetka::playlist_medialib:
			emit(slotChangeState(currentState,konfetka::medialibOnly));
			break;
		}
	}

void MainWindow::slotMedialibClicked()
	{
	switch(currentState)
		{
		case konfetka::rearPanel:
			emit(slotChangeState(currentState,konfetka::medialibOnly));
			break;
		case konfetka::medialibOnly:
			emit(slotChangeState(currentState,konfetka::rearPanel));
			break;
		case konfetka::playlistOnly:
			emit(slotChangeState(currentState,konfetka::playlist_medialib));
			break;
		case konfetka::playlist_medialib:
			emit(slotChangeState(currentState,konfetka::playlistOnly));
			break;
		}
	}

void MainWindow::slotChangeState(konfetka::windowState oldS, konfetka::windowState newS)
	{
	currentState=newS;
	rearpanel->hide();
	switch(oldS)
		{
		case konfetka::rearPanel:
			if(newS==konfetka::medialibOnly)
				{
				emit(moveMedialib(mlibHH,leftSide,rightSide,-5));
				emit(resizeMedialib(rightSide+20-leftSide,height-75));
				}
			if(newS==konfetka::playlistOnly)
				{
				emit(movePlaylist(playHH,rightSide,leftSide,5));
				emit(resizePlaylist(rightSide,height-75));
				}
			break;
		case konfetka::medialibOnly:
			if(newS==konfetka::rearPanel)
				{
				emit(moveMedialib(mlibHH,rightSide,leftSide,5));
				emit(resizeMedialib(rightSide+20-rightSide,height-75));
				}
			if(newS==konfetka::playlist_medialib)
				{
				emit(moveMedialib(mlibHH,middle,leftSide,5));
				emit(resizeMedialib(rightSide+20-middle,height-75));
				emit(movePlaylist(playHH,middle,leftSide,5));
				emit(resizePlaylist(middle,height-75));
				}
			break;
		case konfetka::playlistOnly:
			if(newS==konfetka::rearPanel)
				{
				emit(movePlaylist(playHH,leftSide,rightSide,-5));
				emit(resizePlaylist(leftSide,height-75));
				}
			if(newS==konfetka::playlist_medialib)
				{
				emit(moveMedialib(mlibHH,middle,rightSide,-5));
				emit(resizeMedialib(rightSide+20-middle,height-75));
				emit(movePlaylist(playHH,middle,rightSide,-5));
				emit(resizePlaylist(middle,height-75));
				}
			break;
		case konfetka::playlist_medialib:
			if(newS==konfetka::medialibOnly)
				{
				emit(movePlaylist(playHH,leftSide,middle,-5));
				emit(moveMedialib(mlibHH,leftSide,middle,-5));
				emit(resizeMedialib(rightSide+20-leftSide,height-75));
				emit(resizePlaylist(leftSide,height-75));
				}
			if(newS==konfetka::playlistOnly)
				{
				emit(movePlaylist(playHH,rightSide,middle,5));
				emit(moveMedialib(mlibHH,rightSide,middle,5));
				emit(resizeMedialib(rightSide+20-rightSide,height-75));
				emit(resizePlaylist(rightSide,height-75));
				}
			break;
		}
	}*/

void MainWindow::slotAnimationDone()
	{
	rearpanel->show();
	}

void MainWindow::toggle()
	{
	if(animator->isActive()) return;
	if(minibar->isVisible()) {minibar->hide();this->show();return;}
// 	std::cout<<"showing"<<std::endl;
	this->show();
	locked=true;
	//mainbar->hide();
	//playlist_handle->hide();
	//playlist->hide();
	//medialib_handle->hide();
	//medialib->hide();
	//rearpanel->hide();
	step = 45;
	if(currentHeight == height)
	step*=-1;

	setUpdatesEnabled(0);
	QObject::connect(animator,SIGNAL(timeout()),this,SLOT(slotResizeThis()));
	animator->start(10);
	}

void MainWindow::slotResizeThis() {
	if(!isVisible()) {
	show();
	}

	currentHeight += step;
		if(currentHeight<0)
		currentHeight = 0;
		if(currentHeight>height)
		currentHeight = height;
	
	setGeometry(x(),y(),width(),currentHeight);
	
	QRegion maskedRegion(0,0,width(),currentHeight);
	setMask(maskedRegion);
	
	if(currentHeight == 0) {
	hide();
	animator->stop();
	animator->disconnect();
	setUpdatesEnabled(1);
	}
	else if(currentHeight == height) {
	animator->stop();
	animator->disconnect();
	setUpdatesEnabled(1);
	}
}

void MainWindow::clickResponse(QSystemTrayIcon::ActivationReason reason)
	{
	if(reason==QSystemTrayIcon::Context) return;
	if(reason==QSystemTrayIcon::Trigger) toggle();
	}

/*void MainWindow::provideRefresh()
	{
	switch(currentState)
		{
		case konfetka::medialibOnly:
		case konfetka::rearPanel:
			emit(resizePlaylist(leftSide,height-75));
			return;
		case konfetka::playlist_medialib:
			emit(resizePlaylist(middle,height-75));
			return;
		case konfetka::playlistOnly:
			emit(resizePlaylist(rightSide,height-75));
			return;
		};
	}*/


//! ADDED TO MAKE RAND PLAY WORK, also makes other things more convenient
Playlist* MainWindow::thePlaylist() {
return playlistpanel->returnPlaylist();
}
MediaLib* MainWindow::theMediaLib() {
return medialibView;
}
//! done
#endif
