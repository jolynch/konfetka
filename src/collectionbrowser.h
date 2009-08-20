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
#include <QLineEdit>
#include <QKeyEvent>
#include <QtDebug>
#include <QPushButton>
#include <QInputDialog>

//Boost
#include <boost/bind.hpp>
#include <boost/ref.hpp>

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
	QHash<int,QTreeWidgetItem*> idToItem;
	QStringList labels;
	QString currentCollection; Xmms::Collection::Namespace currentNamespace;
	int currentCollectionType;
	QGridLayout * layout;

	QTreeWidget * collDisplay;
	QListWidget * collList;
	QListWidget * plistList;
	
	QSplitter * splitter;
	QSplitter * listSplitter;
	QLineEdit * dispColl;
	
	//Dragging
	QTimer waitTimer;
	QMimeData * mimeData;
	QDrag * drag;
	
	void addIdToView(int id,bool isPriority);

	public:
	CollectionBrowser(DataBackend * c,QWidget * parent = 0, Qt::WindowFlags f = 0);
	void setLayoutSide(bool);
	bool recievedNewColl(const Xmms::Coll::Coll& newColl);
	bool updateCollDisplay(const Xmms::List <int> &list);
	//Removes a collection
	void removeSelectedCollections();
	//Removes a playlist
	void removeSelectedPlaylists();
	//Appends an extra NOT collection
	void removeSelectedItems();
	
	void resizeEvent(QResizeEvent* event);
	void keyPressEvent(QKeyEvent* event);
	QMimeData* getMimeInfo(const QList<QTreeWidgetItem *> items);
	void appendNewCollection(Xmms::Coll::Coll*);
	//Bin related funcs
	int getCurrentType();
	QString getCurrentName();
	Xmms::Collection::Namespace getCurrentNamespace();
	xmmsc_coll_t* findPartOfBin(xmmsc_coll_t* bin,int type);
	void appendListToBin(QList<int> list);
	void appendCollToBin(const Xmms::Coll::Coll& coll);
	void removeFromBin(QList<int> list);
	/*For whenever we can do the collection editing through cpp bindings 
	bool appendListToBin(QList<int> list,const Xmms::Coll::Coll& coll);
	bool appendCollToBin(const Xmms::Coll::Coll& coll);
	bool removeFromBin(QList<int> list,const Xmms::Coll::Coll& coll);
	*/


	public slots:
	void updateCollList(QStringList);
	void updatePlistList(QStringList);
	void getCollectionFromItem(QListWidgetItem*);
	void updateInfo(int id);
	void getNextFew(int value);
	void addItemToPlist(QTreeWidgetItem*,int);
	void greyItem(QTreeWidgetItem*);
	void startDragList(QListWidgetItem*);
	void startDrag();
	void createNewBin();
};

class CollTreeWidget:public QTreeWidget {
	Q_OBJECT
	private:
	CollectionBrowser* lib;
	DataBackend * conn;
	int numIDs;
	bool addingIDs;
	QList<int> listOfIDs;
	public:
	CollTreeWidget(CollectionBrowser* l,DataBackend* c,QWidget * parent = 0);
	~CollTreeWidget();

	bool lookupId(bool isAdd,const int val);	
	bool lookupColl(const Xmms::Coll::Coll& coll);
	//Drag Drop / User interaction
	void dropEvent(QDropEvent *event);
	void dragMoveEvent ( QDragMoveEvent * ); 
	void dragEnterEvent(QDragEnterEvent *event);
	QStringList mimeTypes() const;
	QMimeData* mimeData(const QList<QTreeWidgetItem *> items) const;
};


#endif


