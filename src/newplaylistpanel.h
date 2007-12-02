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

class Playlist_:public QTableView
	{	Q_OBJECT
	private:
		DataBackend * conn;
	public:
		Playlist_(DataBackend * c);
		void setModelAndDelegate(SinglePlaylist * model);
		QList <uint> getSortedSelectedRows(); //Sorted from lowest to highest
		void dropEvent(QDropEvent *event);
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
		void deleteSelected();
	public slots:
		void playlistsChanged(QStringList playlists);
		void setCurrentName(std::string name);
		void playlistReady(std::string name,SinglePlaylist * plist);
	};

#endif
