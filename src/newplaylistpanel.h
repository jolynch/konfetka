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
#include <QString>
#include <QComboBox>
#include <QPushButton>

class PlaylistPanel_:public QWidget, LayoutPanel
	{	Q_OBJECT
	private:
		bool editing, locked;
		DataBackend * conn;
		std::string currentPlaylistName;
		SinglePlaylist * currentPlaylist;

		QGridLayout * centralLayout;
		QTreeView * playlistView;
		QComboBox * playlistSwitcher;
		QPushButton * playlistModeSwitcher;

		PlistData * plistBackend;
	public:
		PlaylistPanel_(DataBackend * c);
		void setLayoutSide(bool right_side);
	private slots:
		void doubleClicked(const QModelIndex & index);
		void playlistSelected(QString name);
		void playlistModeSwitched();
	public slots:
		void playlistsChanged(QStringList playlists);
		void setCurrentName(std::string name);
		void playlistReady(std::string name,SinglePlaylist * plist);
	};

#endif
