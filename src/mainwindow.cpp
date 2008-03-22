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
	QRect deskRect = desktop->availableGeometry();
	QRect screenRect = desktop->screenGeometry();
	std::cout<<"DR- X: "<<deskRect.x()<<" Y: "<<deskRect.y()<<" WIDTH: "<<deskRect.width()<<" HEIGHT: "<<deskRect.height()<<std::endl;
	std::cout<<"SR- X: "<<screenRect.x()<<" Y: "<<screenRect.y()<<" WIDTH: "<<screenRect.width()<<" HEIGHT: "<<screenRect.height()<<std::endl;
	screenW = screenRect.width();
	screenH = screenRect.height();
	height=450; currentHeight=450; step=-20;
	this->setGeometry(screenRect.x(),screenRect.y(),screenW,450);
	
	conn->initPanelController(width());
	PanelController * pc=((PanelController *)conn->getDataBackendObject(DataBackend::PANEL));

	rearpanel=new RearPanel(conn,screenRect, this, NULL);	

		if(s.value("staysOnTop").toBool())
		minibar=new MiniMode(conn,rearpanel->getAlbumArt(),NULL,Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);
		else
		minibar=new MiniMode(conn,rearpanel->getAlbumArt(),NULL,Qt::FramelessWindowHint);
	
	connect(minibar,SIGNAL(volumeValueChanged(int)),mainbar,SLOT(slotSetVolume(int)));
	connect(mainbar,SIGNAL(volumeChanged(int)),minibar,SIGNAL(setVolumeValue(int)));
	
	icon=new QSystemTrayIcon(QIcon(":images/logomini.png"));
	QObject::connect(icon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
						this,SLOT(clickResponse(QSystemTrayIcon::ActivationReason)));
	QObject::connect(mainbar,SIGNAL(infoChanged()),this,SLOT(trayTool()));

	QAction * temp;
	QMenu * menu=new QMenu();
	menu->setTitle("Quick Access");
	temp = menu->addAction(QIcon(":images/play_button.png"),"Play",mainbar,SLOT(slotPlay()));
	menu->addAction(QIcon(":images/pause_button.png"),"Pause",mainbar,SLOT(slotPause()));
	menu->addAction(QIcon(":images/stop_button.png"),"Stop",mainbar, SLOT(slotStop()));
	menu->addAction(QIcon(":images/next_button.png"),"Next Song",conn,SLOT(playNextSong()));
	menu->addAction(QIcon(":images/back_button.png"),"Previous Song",conn,SLOT(playPreviousSong()));
	
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
	/*!!!!!!!!!!!!!!need to remove this hardcoding somehow!!!!!!!!!!!!!!!!!*/
	/*!Other than the panel handles being hard coded to being 187 high what hard coding?!*/
	QRect geom=this->geometry();
	mainbar->resize(geom.width(),75);
	mainbar->move(geom.x(),geom.height()-75);
	mainbar->show();
	rearpanel->resize(geom.width()-40,geom.height()-75);
	rearpanel->move(geom.x()+20,0);
	rearpanel->show();
	leftSide=geom.x();
	rightSide=geom.width()-20;
	middle=geom.width()/2-10;
	mlibHH=rearpanel->height()-187;
	playHH=0;

	currentPlaylistPos=leftSide;
	currentMedialibPos=rightSide;
	currentState=konfetka::rearPanel;

	minibar->hide();

/*	pc->registerPanel(new Panel(this,"PLAYLIST",new PlaylistPanel(conn)),"Playlist",PanelController::LEFTPANEL|PanelController::LAYOUT_PANEL);
	pc->registerPanel(new Panel(this,"MEDIALIB",new MediaLib(conn,this,NULL)),"Medialib",PanelController::RIGHTPANEL|PanelController::LAYOUT_PANEL);
	pc->registerPanel(new Panel(this,"PLAYLIST",new PlaylistPanel(conn)),"Playlist",PanelController::RIGHTPANEL|PanelController::LAYOUT_PANEL);
	pc->registerPanel(new Panel(this,"MEDIALIB",new MediaLib(conn,this,NULL)),"Medialib",PanelController::LEFTPANEL|PanelController::LAYOUT_PANEL);
	pc->registerPanel(new Panel(this,"FILESYSTEM",new FileBrowser(conn,this)),"File System",PanelController::RIGHTPANEL);
	pc->registerPanel(new Panel(this,"FILESYSTEM",new FileBrowser(conn,this)),"File System",PanelController::LEFTPANEL);
	pc->registerPanel(new Panel(this,"COLLECTIONS",new CollectionBrowser(conn,this)),"Collections",PanelController::LEFTPANEL|PanelController::LAYOUT_PANEL);
	pc->registerPanel(new Panel(this,"COLLECTIONS",new CollectionBrowser(conn,this)),"Collections",PanelController::RIGHTPANEL|PanelController::LAYOUT_PANEL);
	pc->registerPanel(new Panel(this,"OPTIONS",new Options(conn,this)),"Options",PanelController::RIGHTPANEL|PanelController::LAYOUT_PANEL);
	pc->registerPanel(new Panel(this,"OPTIONS",new Options(conn,this)),"Options",PanelController::LEFTPANEL|PanelController::LAYOUT_PANEL);*/
	connect(conn,SIGNAL(qsettingsValueChanged(QString,QVariant)),this,SLOT(respondToConfigChange(QString,QVariant)));
	conn->emitInitialQSettings();
	conn->emitInitialXmms2Settings();
	}

void MainWindow::respondToConfigChange(QString name,QVariant value) {
	if(name!="konfetka/stayOnTop") return;
	
	if(value.toBool())
	setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	else
	setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
	
	show();
	setWindowIcon(QIcon(":images/logomini"));
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
	//Playlisthandle->hide();
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

#endif
