#ifndef MEDIALIB_H
#define MEDIALIB_H
#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#endif
#include "collectioneditor.h"
#include <QTreeWidget>
#include <QGridLayout>
#include <QTreeView>
#include <QDirModel>
#include <QTreeWidgetItem>
#include <QStringList>
#include "databackend.h"
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

//#include <QVariant>

class DropTreeWidget;

class MediaItem:public QTreeWidgetItem {
	private:
	QHash<QString,QVariant> songInfo;
	public:
	QVariant info(QString);
	QHash<QString,QVariant> allInfo();
	void setInfo(QHash<QString,QVariant>);
	bool matches(QString,QString);
};

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
