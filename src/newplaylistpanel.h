#ifndef PLAYLISTPANEL__H
#define PLAYLISTPANEL__H
#include "databackend.h"
#include "databackend/plistdata.h"
#include "layoutpanel.h"
#include <iostream>
#include <string>
#include <QWidget>
#include <QGridLayout>
#include <QTreeView>
#include <QString>

class PlaylistPanel_:public QWidget, LayoutPanel
	{	Q_OBJECT
	private:
		DataBackend * conn;
		QGridLayout * layout;
		QTreeView * playlistView;
		std::string currentPlaylistName;
		SinglePlaylist * currentPlaylist;
		PlistData * plistBackend;
	public:
		PlaylistPanel_(DataBackend * c);
		void setLayoutSide(bool right_side);
	public slots:
		void setCurrentName(std::string name);
		void playlistReady(std::string name,SinglePlaylist * plist);
	};

#endif
