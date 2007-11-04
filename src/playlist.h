#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#include <xmmsclient/xmmsclient.h>
#endif

#ifndef BOOST_BIND
#define BOOST_BIND
#include <boost/bind.hpp>
#endif

#ifndef PLAYLIST_H
#define PLAYLIST_H
#include "databackend.h"
#include <string>
#include <iostream>
#include <QTime>
#include <QList>
#include <QTableView>
#include <QTreeWidget>
#include <QTreeWidget>
#include <QModelIndex>
#include <QItemSelectionModel>
#include <QItemDelegate>
#include <QPainter>
#include <QHeaderView>
#include <QHash>
#include <QString>
#include <QPoint>
#include <QStack>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QSettings>
#include <QUrl>
#include <QThread>
#include <QPalette>
class Playlist;
class checkableQTreeWidgetItem : public QTreeWidgetItem
{
	public:
		virtual void setData(int column, int role, const QVariant& value);
};

class Playlist:public QTreeWidget
	{	Q_OBJECT
	private:
// 		QHash <int,QTreeWidgetItem*> idToItem;
// 		QHash <int,int> mlibIdToPlistId;
		QHash <int,int> rowToId;
		QList <QString> labels;
		QList <QTreeWidgetItem *> checkedItems;
		DataBackend * conn;
		std::string currentPlist;
		bool editing;
		int items,curRow,curColor,pos;
		int dragItem;
		QPoint dragStartPosition;
		QTreeWidgetItem * curItem;
		QList<QTreeWidgetItem *> fadingItems;
		QTimer timer;
		int randNum; bool playRand; bool currentPosOverride;
		QStack<int> havePlayed;
		QVector<bool> haveNotPlayed; 
	public:
		Playlist(DataBackend * c, QWidget * p=0);
		bool refreshList(const Xmms::List< unsigned int > &list);
		void toggleEdit();
		bool isEditing();
		bool enterInfo(const Xmms::PropDict & info, int row);
		int itemCount();
		void showDuplicates();
		void fadeItem(QTreeWidgetItem*,int);
		void resetColors();
	public slots:
		void refresh (int id);
		void setHeader(QList<QString> info);
		void currentPos(const unsigned int& val);
		void slotDoubleClicked(QTreeWidgetItem * item, int column );
		void changeResponse(const Xmms::Dict& val);
		void getCurrentName(const std::string & name);
	
		void fadeItems();
		void removeCheckedItems();
		void dragEnterEvent(QDragEnterEvent *event);
		void dropEvent(QDropEvent *event);
		void dragMoveEvent ( QDragMoveEvent * ); 
		void itemCheckStateChanged(checkableQTreeWidgetItem* item);
		void playRandomly();
		void highlightCurrent(Xmms::PropDict);
		void randomlyPickNextSong();
		void changeSongRel(int i = 1);
		void respondToSongAboutToEnd();
	};


#endif
