#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#endif
#ifndef FILEBROWSER_H
#define FILEBROWSER_H
#include "databackend.h"
#include "databackend/colldata.h"
#include "databackend/plistdata.h"
#include <QObject>
#include <QWidget>
#include <QGridLayout>
#include <QTreeView>
#include <QListView>
#include <QDirModel>
#include <QLineEdit>
#include <QSplitter>
#include <QUrl>
#include <QDesktopServices>
#include <QCompleter>
#include <QShortcut>
#include <QItemSelectionModel>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QWidgetAction>
#include <QAction>
#include <QToolBar>
#include <QStack>

// #include <QFileDialog>


class FileBrowser:public QWidget {
	Q_OBJECT
	private:
	DataBackend * conn;
	QGridLayout * layout;
	QTreeView * fileTree;
	QListView * fileList;
	QDirModel * model;
	QLineEdit * filterLine;
	QSplitter * splitter;
	QShortcut * deleteSC; 
	
	QModelIndex last;
	QStack<QModelIndex> forward;
	QStack<QModelIndex> backward;
	
	QComboBox * toWhat;
	QComboBox * whichPlaylist;

	public:
	FileBrowser(DataBackend * c,QWidget * parent = 0, Qt::WindowFlags f = 0);

	public slots:
	void setRoot(QModelIndex index,bool keep = true);
	void slotRemove();
	void addSelected();
	void destinationChanged(QString);
	void handleCompleter();
	void handleCompleter(QModelIndex);
	void updatePlaylists(QStringList);
	void refresh();
	void goHome();
	void goUp();
	void goBack();
	void goForward();
};

#endif

