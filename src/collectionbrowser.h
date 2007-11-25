#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#endif
#ifndef COLLECTIONBROWSER_H
#define COLLECTIONBROWSER_H
#include "databackend.h"
#include "layoutpanel.h"
#include "databackend/colldata.h"
#include "databackend/mlibdata.h"
#include <QWidget>
#include <QGridLayout>
#include <QTreeWidget>
#include <QListWidget>
#include <QSplitter>
#include <QStringList>
#include <QScrollBar>
#include <QSettings>
#include <QHeaderView>
#include <QTimer>
#include <QMimeData>
#include <QDrag>
#include <QUrl>


class CollectionBrowser:public LayoutPanel {
	Q_OBJECT
	private:
	bool side;
	int numFetched;
	MlibData * mlib;
	CollData * coll;
	DataBackend * conn;
	Xmms::Collection::Namespace collNamespace;
	Xmms::Collection::Namespace plistNamespace;
	QHash<uint,QTreeWidgetItem*> idToItem;
	QStringList labels;
	QGridLayout * layout1;

	QTreeWidget * collDisplay;
	QListWidget * collList;
	QListWidget * plistList;
	
	QSplitter * splitter;
	QSplitter * listSplitter;
	//Dragging
	QTimer waitTimer;
	QMimeData * mimeData;
	QDrag * drag;
	
	void addIdToView(int id,bool isPriority);

	public:
	CollectionBrowser(DataBackend * c,QWidget * parent = 0, Qt::WindowFlags f = 0);
	void setLayoutSide(bool);
	bool recievedNewColl(const Xmms::Coll::Coll&);
	bool updateCollDisplay(const Xmms::List <uint> &list);
	void resizeEvent(QResizeEvent* event);

	public slots:
	void updateCollList(QStringList);
	void updatePlistList(QStringList);
	void getCollectionFromItem(QListWidgetItem*);
	void updateInfo(int id);
	void getNextFew(int value);
	void addItemToPlist(QTreeWidgetItem*,int);
	void greyItem(QTreeWidgetItem*);
	void startDragTree(QTreeWidgetItem*,int);
	void startDragList(QListWidgetItem*);
};


#endif


