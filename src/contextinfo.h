#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#include <xmmsclient/xmmsclient.h>
#endif

#ifndef BOOST_BIND
#define BOOST_BIND
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#endif

#ifndef CONTEXTINFO_H
#define CONTEXTINFO_H
#include "databackend.h"
#include "databackend/mlibdata.h"
#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QGridLayout>
#include <QPalette>
#include <QStringList>
#include <QHash>
#include <QBuffer>
#include <QMouseEvent>
#include <QHeaderView>
#include <QScrollBar>
#include <QPainter>
#include <QtDebug>

class ContextInfo:public QWidget {
	Q_OBJECT
	private:
	DataBackend * conn;
	MlibData * mlib;
	QTreeWidget * tree;
	QTreeWidgetItem * artistHeader;
	QHash<int,QTreeWidgetItem*> idToItem;
	QHash<QString,QTreeWidgetItem*> albumToItem;
	int curId; int cntr;

	public:
	ContextInfo(DataBackend * c,QWidget * parent = 0, Qt::WindowFlags f = 0);
	void setInfo(int id);
	void constructArtist();
	bool gotAlbums(const Xmms::List <Xmms::Dict> &list);
	bool constructAlbum(QTreeWidgetItem*,const Xmms::List <Xmms::Dict> &list);
	bool gotAlbumCover(int id,const Xmms::bin& res);
	bool addAlbumToPlist(const Xmms::List <uint> &list);
	
	public slots:
	void infoChanged(int);
	void slotUpdateInfo(int);
	void setUpdatedEnabled(bool w = true);
	void addToPlist(QTreeWidgetItem * item);
	
};


#endif
