#ifndef PANELVIEW_CPP
#define PANELVIEW_CPP
#include "panelview.h"

PanelView::PanelView(DataBackend * c,QWidget * parent, Qt::WindowFlags f):QWidget(parent,f) {
	conn = c;
	controller = (PanelController_*)(conn->getDataBackendObject(DataBackend::PANEL));
	
	loadPanels();
	
	infoMenu = new QMenu("Panel Control");
	infoMenu->addAction("Demand Panel",this,SLOT(demandPanel()));
	infoMenu->addAction("What is this?",this,SLOT(helpInfo()));
	
	setFocusPolicy(Qt::StrongFocus);
	QString dirName; QString info;
	for(int i=0;i<panels.size();i++) {
		info.clear();
		dirName = "/clients/konfetka/helpFiles/";
		dirName.prepend(Xmms::getUserConfDir().c_str());
		if(keys.value(i) == "Playlist")
			dirName+="playlist_panel.txt";
		else if(keys.value(i) == "Options")
			dirName+="options_panel.txt";
		else if(keys.value(i) == "File System")
			dirName+="filesystem_panel.txt";
		else if(keys.value(i) == "Collections")
			dirName+="collection_panel.txt";
		else if(keys.value(i) == "Medialib")
			dirName+="medialib_panel.txt";
		
		qDebug()<<dirName;
		QFile file(dirName);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) continue;
		
		while (!file.atEnd()) {
		QByteArray line = file.readLine();
		info += line;
		}
		
		registerHelpInfo(keys.value(i),info);
	}
}

void PanelView::paintEvent(QPaintEvent * event) { 
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing);
	int h = height()/panels.size();
	int w = width(); int offset = 5; int x = 0;
	QRectF rect;
	for(int i=0;i<panels.size();i++) {
		path = QPainterPath();
		rect = QRectF(0,i*h,w,h);
		path.addRoundRect(rect,20);
		painter.drawPath(path);
		if(i==selected) {
			painter.fillPath(path,QBrush("#418ad4"));
		}
		painter.drawPixmap(0,i*h,panels.value(keys.value(i)).scaled(h,h,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
		painter.drawText(h+offset,i*h+h/2,keys.value(i));
		//Draws the text, wrapping, into the space provided.
		//h+fontMetrics().width(keys.value(i))+2*offset
		x = qMax(h+fontMetrics().width(keys.value(i))+2*offset,140);
		painter.drawText(QRect(x,i*h, width()-x,h),Qt::AlignLeft | Qt::TextWordWrap,helpText.value(keys.value(i)));
	}
}

void PanelView::mousePressEvent(QMouseEvent * event) {
	int h = height()/panels.size();
	selected = event->y()/h;
// 	qDebug()<<selected<<h<<event->y();
	update();
}

void PanelView::mouseDoubleClickEvent(QMouseEvent * event) {
	controller->demandPanel(keys.value(selected));
	qDebug()<<"Demanded Panel"<<keys.value(selected);
}

void PanelView::contextMenuEvent(QContextMenuEvent * event) {
	infoMenu->exec(event->globalPos());
}

void PanelView::keyPressedEvent(QKeyEvent * event){
	qDebug()<<"pressed";
	if(event->key()==Qt::Key_Up) {
		selected += 1;
		selected %= panels.size();
	}
	else if(event->key() == Qt::Key_Down) {
		selected -= 1;
		if(selected < 0) selected = panels.size() + selected;
	}
	else
	event->ignore();
	update();
}

void PanelView::demandPanel() {
	controller->demandPanel(keys.value(selected));
	qDebug()<<"Demanded Panel"<<keys.value(selected);
}

void PanelView::helpInfo() {
	qDebug()<<"Help"<<keys.value(selected);
}

void PanelView::loadPanels() {
// 	foreach(QString str,controller->getPanelNames()) {
// 		panels->insert(str,QPixmap::grabWidget(controller->getPanel(str)));
// 	}
	QList<QString> temp; temp<<"Playlist"<<"Options"<<"File System"<<"Collections"<<"Medialib";
	foreach(QString val,temp){
		panels.insert(val,QPixmap("images/no_album175"));
	}
	keys = panels.keys();
}

void PanelView::registerHelpInfo(QString id,QString info) {
	helpText.insert(id,info);
}

#endif
