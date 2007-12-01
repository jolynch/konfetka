#ifndef MEDIALIB_H
#define MEDIALIB_H
#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#endif

#ifndef BOOST_BIND
#define BOOST_BIND
#include <boost/bind.hpp>
#endif

#include "databackend.h"
#include "databackend/mlibdata.h"
#include "databackend/colldata.h"
#include "layoutpanel.h"
#include <QTreeWidget>
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
#include <QInputDialog>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QMenu>

//#include <QVariant>

class DropTreeWidget;
class ComplexSearchDialog;

typedef uint ItemType;
typedef uint SourceType;
enum Operator {opor, opand}; 

class MediaLib:public LayoutPanel{	
Q_OBJECT

	private:
	DataBackend * conn;
	MlibData* mlib;
	CollData* coll;	
	bool adding,complexSearch;
	DropTreeWidget * mediaList;
	QPushButton * loadUniverse;
	QPushButton * update;
	QPushButton * makeColl;
	QPushButton * complexSearchButton;
	QLineEdit * searchLine;
	QLabel * searchLabel;
	QGridLayout * layout;
	QVBoxLayout * buttons;
	QList<QUrl> urlList;
	QShortcut  * delItem;
	QTimer doubleClickTimer;
	QStack<uint> idStack;
	ComplexSearchDialog * searchDialog;
	
	//NEW LISTS
	QHash<uint,QTreeWidgetItem*> idToSongItem;
	Xmms::Coll::Coll * visibleMedia;
	Xmms::Coll::Coll * lastVisibleMedia;
	static const ItemType ARTIST = 0x001;
	static const ItemType ALBUM = 0x010;
	static const ItemType SONG = 0x011;
	static const ItemType INVALID = 0x100;
	static const SourceType SELECTED = 0x01;
	static const SourceType VISIBLE = 0x10;
	ItemType getItemType(QTreeWidgetItem*);	

	public:
	MediaLib(DataBackend * c, QWidget * parent = 0, Qt::WindowFlags f = 0);
	~MediaLib();
	
	Xmms::Coll::Union* selectedAsColl();
	bool addToPlaylistFromCollectionDrag(const Xmms::List <Xmms::Dict> &list);
	void setLayoutSide(bool right_side);
	void loadUpCollection(Xmms::Coll::Coll*);
	int numDone,total;

	//NEW LISTS
	bool gotAlbums(QTreeWidgetItem* artist,const Xmms::List <Xmms::Dict> &list);
	bool gotSongs(QTreeWidgetItem* artist,const Xmms::List <uint> &list);
	bool handleUnknown(const Xmms::List <uint> &list);

	public slots:
	void refreshList();
	void checkIfRefreshIsNeeded();
	void respondToPeriodicUpdate();
	//refer to cpp
// 	bool getArtists(const Xmms::List <Xmms::Dict> &list);
	void loadUniv();
	void useSelected();
	void useVisible();
	void newColl(SourceType type);
	void searchMlib();
	
	void addToMlibDrag(QTreeWidgetItem*,int);
	void addToMlibDoubleClick(QTreeWidgetItem *,int);
	void startDrag();
	void stopTimerAndClearList();

	void slotRemove();
	void removeNodes(QList<QTreeWidgetItem*>);
	bool removeIds(const Xmms::List <uint> &list);
	
	//NEW LISTS
	void gotNewList(QString property, QList<QString> info);
	void artistList(QList<QString> info);
	void itemExpanded(QTreeWidgetItem* item);
	void getAlbumList(QTreeWidgetItem* item);
	void getSongInfo(QTreeWidgetItem* item);

	void infoChanged(int id);
	void toggleComplexSearch();
	void addAnotherSearchItem();
	void recievedNewList(QList< QPair <Xmms::Coll::Coll*,Operator> >);
	
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
	
	//Drag Drop
	void dropEvent(QDropEvent *event);
	void dragMoveEvent ( QDragMoveEvent * ); 
	void dragEnterEvent(QDragEnterEvent *event);
	void keyPressEvent(QKeyEvent *event);

	//Adding to Mlib related
	void recurAdd(QString,bool);
	void numSongs(QString path);
	
	signals:
	void removeSelected();
};

class ComplexSearchDialog:public QDialog {
	Q_OBJECT
	
	private:
	DataBackend* conn;
	Xmms::Coll::Coll* searchMedia;
	QList< QPair <Xmms::Coll::Coll*,Operator> > complexSearchItems;
	QShortcut * delItem;
	QTreeWidget * itemList;
	QLabel * tagLabel;
	QComboBox * tag;
	QLabel * operLabel;
	QComboBox * oper;
	QLabel * valueLabel;
	QLineEdit * value;
	QLabel * appendageTypeLabel;
	QComboBox * appendageType;
	QPushButton * add;
	QCheckBox * notCheck;
	QDialogButtonBox * buttons;
	QGridLayout * layout;

	public:
	ComplexSearchDialog(DataBackend*, Xmms::Coll::Coll*);
	Xmms::Coll::Coll* newColl(QString attr,QString oper,QString val,bool notFlag);
	void keyPressEvent(QKeyEvent *event);

	public slots:
	void accept();
	void addOperand();
	void removeOperand();
	void clearItems();
	
	signals:
	void newList(QList< QPair <Xmms::Coll::Coll*,Operator> >);
	
};




#endif
