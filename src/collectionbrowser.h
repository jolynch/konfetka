#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#endif
#ifndef COLLECTIONBROWSER_H
#define COLLECTIONBROWSER_H
#include "databackend.h"
#include "layoutpanel.h"
#include "databackend/colldata.h"
#include "databackend/mlibdata.h"
#include <QWidget>
#include <QGridLayout>
#include <QTreeWidget>
#include <QListWidget>
#include <QSplitter>
#include <QStringList>

class CollectionBrowser:public QWidget, public LayoutPanel {
	Q_OBJECT
	private:
	bool side;
	MlibData * mlib;
	CollData * coll;
	DataBackend * conn;
	Xmms::Collection::Namespace collNamespace;
	QGridLayout * layout1;
	QGridLayout * layout2;

	QTreeWidget * collDisplay;
	QListWidget * collList;
	QSplitter * splitter;

	public:
	CollectionBrowser(DataBackend * c,QWidget * parent = 0, Qt::WindowFlags f = 0);
	void setLayoutSide(bool);
	bool recievedNewColl(const Xmms::Coll::Coll&);
	public slots:
	void updateCollList(QStringList);
	void getCollectionFromItem(QListWidgetItem*);
};


#endif


