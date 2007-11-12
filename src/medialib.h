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
#include "collectioneditor.h"
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

//#include <QVariant>

class DropTreeWidget;
class ComplexSearchDialog;

typedef uint ItemType;
enum Operator {opor, opand}; 

class MediaLib:public QWidget {	
Q_OBJECT

	private:
	bool adding,complexSearch;
	DropTreeWidget * mediaList;
	QPushButton * add;
	QPushButton * update;
	QPushButton * makeColl;
	QPushButton * complexSearchButton;
	QLineEdit * searchLine;
	QLabel * searchLabel;
	QGridLayout * layout;
	QVBoxLayout * buttons;
	QList<QUrl> urlList;
// 	QDrag * drag;
// 	QMimeData * mimeData;
	QShortcut  * delItem;
	QTimer doubleClickTimer;
	QStack<uint> idStack;
	ComplexSearchDialog * searchDialog;
	
	//NEW LISTS
	QHash<uint,MediaItem*> idToMediaItem;
	Xmms::Coll::Coll * visibleMedia;
	static const ItemType ARTIST = 0x001;
	static const ItemType ALBUM = 0x010;
	static const ItemType SONG = 0x011;
	ItemType getItemType(QTreeWidgetItem*);	

	public:
	MediaLib(DataBackend * conn, QWidget * parent = 0, Qt::WindowFlags f = 0);
	~MediaLib();
	
	Xmms::Coll::Union* selectedAsColl();
	bool addToPlaylistFromCollectionDrag(const Xmms::List <Xmms::Dict> &list);
	DataBackend * xmms;
	QDirModel * dirModel;
	QTreeView * fileList;
	bool fromKonfetka;
	int numDone,total;

	//NEW LISTS
	bool gotAlbums(QTreeWidgetItem* artist,const Xmms::List <Xmms::Dict> &list);
	bool gotSongs(QTreeWidgetItem* artist,const Xmms::List <uint> &list);
	bool handleUnknown(const Xmms::List <uint> &list);

	public slots:
	void refreshList();
	void checkIfRefreshIsNeeded();
	//refer to cpp
// 	bool getArtists(const Xmms::List <Xmms::Dict> &list);
	void toggleFileList();
	void newColl();
	void searchMlib();
	
	void addToMlibDrag(QTreeWidgetItem*,int);
	void addToMlibDoubleClick(QTreeWidgetItem *,int);
	void startDrag();
	void stopTimerAndClearList();

	void slotRemove();
	void removeNodes(QList<QTreeWidgetItem*>);
	bool removeIds(const Xmms::List <uint> &list);
	
	bool mlibChanged(const unsigned int& id);

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

	public:
	DropTreeWidget(MediaLib*);
	~DropTreeWidget();
	
	//Drag Drop
	void dropEvent(QDropEvent *event);
	void dragMoveEvent ( QDragMoveEvent * ); 
	void dragEnterEvent(QDragEnterEvent *event);
	

	//Adding to Mlib related
	void recurAdd(QString,bool);
	void numSongs(QString path);
};

class ComplexSearchDialog:public QDialog {
	Q_OBJECT
	
	private:
	Xmms::Coll::Coll* searchMedia;
	QList< QPair <Xmms::Coll::Coll*,Operator> > complexSearchItems;
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
	ComplexSearchDialog();
	Xmms::Coll::Coll* newColl(QString attr,QString oper,QString val,bool notFlag);	

	public slots:
	void accept();
	void addOperand();
	void clearItems();
	
	signals:
	void newList(QList< QPair <Xmms::Coll::Coll*,Operator> >);
	
};




#endif
