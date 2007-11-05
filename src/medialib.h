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
#include <QQueue>

//#include <QVariant>

class DropTreeWidget;

class MediaLib:public QWidget {	
Q_OBJECT

	private:
	QString mlibHashPath;
	bool updating,adding,complexSearch;
	DropTreeWidget * mediaList;
	QPushButton * add;
	QPushButton * update;
	QPushButton * makeColl;
	QPushButton * complexSearchButton;
	QLineEdit * searchLine;
	QLabel * searchLabel;
	QGridLayout * layout;
	QVBoxLayout * buttons;
	QList<MediaItem*> songs;
	QList<MediaItem*> matchedSongs;
	QProgressBar * bar;
	QHash<QString,QTreeWidgetItem*> artists;
	QHash<QString,MediaItem*> titles;
	QStringList lookUps;
	QList<QUrl> urlList;
	QDrag * drag;
	QMimeData * mimeData;
	QShortcut  * delItem;
	QStack<int> insertIds;
	QTimer insertTimer;
	QTimer doubleClickTimer;
	
	//NEW LISTS
	QHash<uint,MediaItem*> idToMediaItem;
	Xmms::Coll::Coll * visibleMedia;

	public:
	MediaLib(DataBackend * conn, QWidget * parent = 0, Qt::WindowFlags f = 0);
	~MediaLib();
	void saveToFile();
	void formatNode(QTreeWidgetItem * item,QFile * file,int depth);
	void loadFromFile();	
	MediaItem* loadItem(QString);
	void process(QTreeWidgetItem*,int);
	void removeNode(QTreeWidgetItem *);
	void pushId(int);
	void insertStackedIds();
	DataBackend * xmms;
	QDirModel * dirModel;
	QTreeView * fileList;
	bool fromKonfetka;
	int numDone,total;

	//NEW LISTS
	bool gotAlbums(QTreeWidgetItem* artist,const Xmms::List <Xmms::Dict> &list);
	bool gotSongs(QTreeWidgetItem* artist,const Xmms::List <uint> &list);

	public slots:
	void getSongList();
	bool getSongIds(const Xmms::List<unsigned int> &list);
	bool getMediaInfo(const Xmms::PropDict& info);
	//refer to cpp
// 	bool getArtists(const Xmms::List <Xmms::Dict> &list);
	void toggleFileList();
	void newColl();
	void showProgress(int,int);
	void insert(MediaItem*);
	void searchMlib(QString);
	
	void addToMlibDrag(QTreeWidgetItem*,int);
	void addToMlibDoubleClick(QTreeWidgetItem *,int);
	void startDrag();
	void stopTimerAndClearList();

	void slotRemove();
	bool mlibChanged(const unsigned int& id);
	void insertAnyways();
	void insertNextFew();

	//NEW LISTS
	void gotNewList(QString property, QList<QString> info);
	void artistList(QList<QString> info);
	void albumList(QTreeWidgetItem* artist,QList<QString> info);
	void songList(QTreeWidgetItem* album,QList<QString> info);
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



#endif
