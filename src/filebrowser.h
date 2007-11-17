#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#endif
#ifndef FILEBROWSER_H
#define FILEBROWSER_H
#include "databackend.h"
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


class FileBrowser:public QWidget {
	Q_OBJECT
	private:
	DataBackend * conn;
	QGridLayout * layout;
	QTreeView * fileTree;
	QListView * fileList;
	QDirModel * model;
	QLineEdit * filterLine;
	QSplitter* splitter;

	public:
	FileBrowser(DataBackend * c,QWidget * parent = 0, Qt::WindowFlags f = 0);

	public slots:
	void handleListDoubleClick(QModelIndex);
};

// class MyFileIconProvider:public QFileIconProvider {
// 	Q_OBJECT
// 	private:
// 	QList<QIcon> icons;
// 	
// }



#endif

