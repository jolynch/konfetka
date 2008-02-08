#ifndef MEDIALIB_H
#define MEDIALIB_H
#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#endif

#ifndef BOOST_BIND
#define BOOST_BIND
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#endif

#include "databackend.h"
#include "databackend/mlibdata.h"
#include "databackend/colldata.h"
#include "layoutpanel.h"
#include <QTreeWidget>
#include <QTableWidget>
#include <QGridLayout>
#include <QTreeView>
#include <QDirModel>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QHeaderView>
#include <QLabel>
#include <QFile>
#include <QPushButton>
#include <QDrag>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QStringListModel>
#include <QHash>
#include <QSet>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QThread>
#include <QProgressBar>
#include <QTextStream>
#include <QStack>
#include <QShortcut>
#include <QSettings>
#include <QTime>
#include <QTimer>
#include <QInputDialog>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QMenu>

//#include <QVariant>

class DropTreeWidget;

typedef uint ItemType;
typedef uint SourceType;
typedef uint SearchType;

static const ItemType ARTIST = 0x001;
static const ItemType ALBUM = 0x010;
static const ItemType SONG = 0x011;
static const ItemType INVALID = 0x100;
static const SourceType SELECTED = 0x01;
static const SourceType VISIBLE = 0x10;
static const SearchType SIMPLE= 0;
static const SearchType PARSE= 1;

class MediaLib:public LayoutPanel{	
Q_OBJECT

	private:
	DataBackend * conn;
	QTimer searchTimer;
	MlibData* mlib;
	CollData* coll;	
	bool adding,dblClickAdd;
	SearchType searchType;
	QList<QString> searchTags;
	DropTreeWidget * mediaList;
	QPushButton * loadUniverse;
	QPushButton * updateAll;
	QPushButton * makeColl;
	QLineEdit * searchLine;
	QLabel * searchLabel;
	QGridLayout * layout;
	QVBoxLayout * buttons;
	QList<QUrl> urlList;
	QShortcut  * delItem;
	QStack<uint> idStack;
	QMenu * infoMenu;
	QPoint dragStartPosition;
	
	//NEW LISTS
	QHash<uint,QTreeWidgetItem*> idToSongItem;
	Xmms::Coll::Coll * visibleMedia;
	Xmms::Coll::Coll * baseMedia;
	ItemType getItemType(QTreeWidgetItem*);	

	public:
	MediaLib(DataBackend * c, QWidget * parent = 0, Qt::WindowFlags f = 0);
	~MediaLib();
	
	Xmms::Coll::Union* selectedAsColl();
	bool addToPlaylistFromCollectionDrag(const Xmms::List <Xmms::Dict> &list);
	void setLayoutSide(bool right_side);
	void contextMenuEvent(QContextMenuEvent *event);
	QMimeData* getCurrentMimeData();
	void loadUpCollection(Xmms::Coll::Coll*);

	//NEW LISTS
	bool gotAlbums(QTreeWidgetItem* artist,const Xmms::List <Xmms::Dict> &list);
	bool gotSongs(QTreeWidgetItem* artist,const Xmms::List <uint> &list);
	bool handleUnknown(const Xmms::List <uint> &list);

	public slots:
	void refreshList();
	void checkIfRefreshIsNeeded();
	void respondToPeriodicUpdate();
	void loadUniv();
	void useSelected();
	void useVisible();
	void newColl(SourceType type);
	void possiblySearchMlib();
	void searchMlib();
	void displaySongInfo();
	
	void addFromMlibDrag(QTreeWidgetItem*,int);
	void addFromMlibDoubleClick(QTreeWidgetItem *,int);

	void slotRemove();
	void removeNodes(QList<QTreeWidgetItem*>);
	bool removeIds(const Xmms::List <uint> &list);
	
	void respondToConfigChange(QString name,QVariant value);

	//NEW LISTS
	void gotNewList(QString property, QList<QString> info);
	void artistList(QList<QString> info);
	void itemExpanded(QTreeWidgetItem* item);
	void getAlbumList(QTreeWidgetItem* item);
	void getSongInfo(QTreeWidgetItem* item);

	void infoChanged(int id);
};

class DropTreeWidget:public QTreeWidget {
Q_OBJECT
	
	private:
	MediaLib * lib;
	QString * path;
	DataBackend * conn;
	QDirModel * dirModel;

	public:
	DropTreeWidget(MediaLib*,DataBackend* c);
	~DropTreeWidget();
	
	//Drag Drop / User interaction
	void dropEvent(QDropEvent *event);
	void dragMoveEvent ( QDragMoveEvent * ); 
	void dragEnterEvent(QDragEnterEvent *event);
	void keyPressEvent(QKeyEvent *event);
	QStringList mimeTypes() const;
	QMimeData* mimeData(const QList<QTreeWidgetItem *> items) const;

	signals:
	void removeSelected();
};

class InfoDialog:public QDialog {
Q_OBJECT

	private:
	DataBackend * conn;
	QTableWidget * table;
	
	public:
	InfoDialog(DataBackend*, ItemType, QString);
	
// 	void keyPressEvent(QKeyEvent *event);
	
	public slots:
	void accept();
};
#endif
