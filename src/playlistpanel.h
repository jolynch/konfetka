#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#include <xmmsclient/xmmsclient.h>
#endif

#ifndef PLAYLISTPANEL_H
#define PLAYLISTPANEL_H
#include "databackend.h"
/*#include "playlistbackend.h"
#include "playlistfrontend.h"*/
#include "playlist.h"
#include "collectionchooser.h"
#include "layoutpanel.h"
#include "databackend/colldata.h"
#include <QWidget>
#include <QIcon>
#include <QPushButton>
#include <QGridLayout>
#include <QMenu>
#include <QTreeView>
#include <QDirModel>
#include <QtAlgorithms>
#include <QLabel>
#include <QShortcut>
#include <QComboBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QSpinBox>
#include <sstream>
#include <string>
#include <iostream>
#include <list>

class PlaylistPanel : public QWidget, LayoutPanel
	{	Q_OBJECT
	private:
		Xmms::Coll::Type type;
		std::string currentPlist;
		DataBackend * conn;
		QGridLayout * layout;
		bool adding; bool editing; int addType; bool reformat; bool pass; bool dups;
		QIcon * addIcon;
		QIcon * playlistIcon;
		QIcon * removeIcon;
		QIcon ** repeatAry;
		int currentRepeat;
		QIcon * shuffleIcon;
		QIcon * sortIcon;
		QPushButton * toggler;
		QPushButton * remove;
		QPushButton * repeatType;
		QPushButton * shuffle;
		QPushButton * sort;
		QPushButton * showDuplicates;
		QMenu * addMenu;
		QMenu * removeMenu;
		QMenu * sortMenu;
		QMenu *shuffleMenu;
//		PlaylistBackend * playlist;
//		PlaylistFrontend * plist;
		Playlist * plist;
		QTreeView * add;
		QDirModel * addModel;
		QGridLayout * addLayout;
		QPushButton * play;
		QPushButton * enqueue;
		QPushButton * editLib;
		QShortcut * del;
		QPushButton * plistToggler;
		QComboBox * plistChooser;
		QGridLayout * chooserLayout;
		CollectionChooser * addPlist;
		CollectionChooser * addColl;
		QPushButton * open;
		QPushButton * save;
		QPushButton * newPlist;
		QMenu * createMenu;
		QGridLayout * infoLayout;
		QLabel * history;
		QLabel * upcoming;
		QSpinBox * hist;
		QSpinBox * up;
	public:
		PlaylistPanel(DataBackend * c,QWidget* parent, Qt::WindowFlags f=0);
		bool ovldrepeat(std::string val);
		bool getPlists(const Xmms::List<std::string>& list);
		bool saveThis(const Xmms::Coll::Coll& is);
		bool openError(const std::string& error);
		bool getType(const Xmms::Coll::Coll& co);
		bool setVals(const Xmms::Coll::Coll& co);
		//!Anton if you didn't abstract the playlist from the rest of the file I wouldnt have to have this ....
		Playlist* returnPlaylist();
		void setLayoutSide(bool right_side);
	signals:
		void openMedialibRequest();
		void needRefresh();
		void toggledState(bool,std::string);
	public slots:
		void update(int width, int height);
		void changeRepeatState(std::string val="-1");
		void toggle();
		void slotAdd();
		void slotEnqueue();
		void slotCrop();
		void slotRemove();
		void slotClear();
		void changesResponse(const Xmms::Dict& valChanged);
		void shuffleAction();
		void sortByArtist();
		void sortByTitle();
		void sortByAlbum();
		void sortByFilename();
		void slotRemovePlaylist();
		void openFilesystem();
		void openCollections();
		void openPlaylist();
		void slotChangePlist(int);
		void slotChangePlistState();
		void changeCurrent(const std::string & name);
		void dictsChanged(const Xmms::Dict& change);
		void saveCopy();
		void openPFile();
		void makePlist();
		void makePShuffle();
		void makeQueue();
		void setValues(int val);
		//Joe
		void updateHeaders(QList<QString>);
		void slotShowDuplicates();
		void stateToggled(bool,std::string);
		//Done
	};
#endif
