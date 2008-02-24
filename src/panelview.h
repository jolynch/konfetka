#ifndef PANELVIEW_H
#define PANELVIEW_H
#include "databackend.h"
#include "databackend/panelcontroller_.h"
#include <QWidget>
#include <QList>
#include <QHash>
#include <QPixmap>
#include <QImage>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QMenu>
#include <QFile>
#include <QTextStream>

class PanelView:public QWidget {
	Q_OBJECT
	private:
		DataBackend * conn;
		PanelController_ * controller;
		QHash<QString,QPixmap> panels;
		QHash<QString,QString> helpText;
		int selected;
		QPainterPath path;
		QStringList keys;
		QMenu * infoMenu;
	public:
		PanelView(DataBackend * c,QWidget * parent = 0, Qt::WindowFlags f = 0);
		void paintEvent(QPaintEvent * event);
		void mousePressEvent(QMouseEvent * event);
		void mouseDoubleClickEvent(QMouseEvent * event);
		void contextMenuEvent(QContextMenuEvent * event);
		void keyPressedEvent(QKeyEvent * event);
	public slots:
		void demandPanel();
		void helpInfo();
		void loadPanels();
		void registerHelpInfo(QString id,QString info);
};

#endif


