#ifndef PLAYLISTPANEL__H
#define PLAYLISTPANEL__H
#include "databackend.h"
#include "databackend/plistdata.h"
#include "databackend/colldata.h"
#include "layoutpanel.h"
#include <iostream>
#include <string>
#include <QWidget>
#include <QGridLayout>
#include <QTreeView>
#include <QTableView>
#include <QString>
#include <QComboBox>
#include <QPushButton>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QHeaderView>
#include <QModelIndex>
#include <QList>
#include <QModelIndexList>
#include <QtAlgorithms>
#include <QShortcut>
#include <QContextMenuEvent>
#include <QMenu>
#include <QKeyEvent>

class Playlist_:public QTableView
	{	Q_OBJECT
	private:
		DataBackend * conn;
		QWidget * parent;
		QMenu * rightClickMenu;
	public:
		Playlist_(DataBackend * c,QWidget * p);
		void setModelAndDelegate(SinglePlaylist * model);
		QList <uint> getSortedSelectedRows(); //Sorted from lowest to highest
		void dropEvent(QDropEvent *event);
		void keyPressEvent(QKeyEvent* event);
		void contextMenuEvent ( QContextMenuEvent * event );
	private slots:
		void doubleClicked(const QModelIndex & index);
	};

class PlaylistPanel_:public LayoutPanel
	{	Q_OBJECT
	private:
		bool editing, locked;
		DataBackend * conn;
		std::string currentPlaylistName;
		SinglePlaylist * currentPlaylist;

		QGridLayout * centralLayout;
		Playlist_ * playlistView;
		QComboBox * playlistSwitcher;
		QPushButton * playlistModeSwitcher;

		PlistData * plistBackend;
		QShortcut * del;
	public:
		PlaylistPanel_(DataBackend * c);
		void setLayoutSide(bool right_side);
	private slots:
		void playlistSelected(QString name);
		void playlistModeSwitched();
	public slots:
		void playlistsChanged(QStringList playlists);
		void setCurrentName(std::string name);
		void playlistReady(std::string name,SinglePlaylist * plist);
		void deleteSelected();
		void cropSelected();
		void clear();
	};

#endif
