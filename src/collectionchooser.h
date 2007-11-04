#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#include <xmmsclient/xmmsclient.h>
#endif
#ifndef COLLECTIONCHOOSER_H
#define COLLECTIONCHOOSER_H
#include "databackend.h"
#include <QWidget>
#include <QCheckBox>
#include <QIcon>
#include <QPushButton>
#include <QLabel>
#include <QListView>
#include <QStringListModel>
#include <QGridLayout>
#include <QModelIndex>
#include <string>

class CollectionChooser:public QWidget
	{	Q_OBJECT
	private:
		std::string plist;
		Xmms::Collection::Namespace type;
		DataBackend * conn;
		QGridLayout * layout;
		QListView * list;
		QStringListModel * model;
		QLabel * info;
		QCheckBox * filename;
		QCheckBox * artist;
		QCheckBox * album;
		QCheckBox * title;
		QPushButton * done;
		QPushButton * cancel;
	public:
		CollectionChooser(std::string pl, Xmms::Collection::Namespace t,DataBackend * c,
										QWidget* parent=0, Qt::WindowFlags f=0);
		bool listCollections(const Xmms::List<std::string>& list);
		bool add(const Xmms::Coll::Coll& collection);
		void setPlist(std::string pl);
	public slots:
		void collectionsChanged(const Xmms::Dict& change);
		void cancelPressed();
		void addClicked();
		void doubleClick(const QModelIndex & index);
	signals:
		void close();
	};

#endif
