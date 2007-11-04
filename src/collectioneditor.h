#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#include <xmmsclient/xmmsclient.h>
#endif

#ifndef COLLECTIONEDITOR_H
#define COLLECTIONEDITOR_H
#include "databackend.h"
#include "complexeditor.h"
#include <iostream>
#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QPalette>
#include <QColor>
#include <QGridLayout>
#include <QPushButton>
#include <QStringList>
#include <QStringListModel>
#include <QListView>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QComboBox>
#include <QLineEdit>
#include <QIcon>
#include <QMessageBox>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QFrame>

///COMPLEX EDITOR ---> complexeditor.h
	//Cause this file is too damn long already.

class SimpleEditorItem: public QFrame
	{	Q_OBJECT
	private:
		bool complexItem;
		std::string currColl;
		QLabel * complexError;
		QComboBox * negator;
		QComboBox * property;
		QComboBox * type;
		QLineEdit * value;
		QGridLayout * layout;
		QLabel * header;
		QComboBox * reference;
		QPushButton * remove;
		QWidget * p;
		DataBackend * conn;
		Xmms::Coll::Coll * coll;
	public:
		SimpleEditorItem(std::string curr,Xmms::Coll::Coll & collection_,DataBackend * conn_,QWidget * parent);
		~SimpleEditorItem();
		bool listCollections(const Xmms::List<std::string>& list);
		void setColl(std::string name);
		Xmms::Coll::Coll * toColl();
	private:
		void changeProp(std::string val);
	public slots:
		void collectionsChanged(const Xmms::Dict& change);
	private slots:
		void selfRemove();
	};

class SimpleEditor:public QWidget
	{	Q_OBJECT
	private:
		std::string currColl;
		QGridLayout * layout;
		QPushButton * addItem;
		QWidget * itemArea;
		QVBoxLayout * itemLayout;
		QList < SimpleEditorItem * > items;
		QScrollArea * scroller;
		DataBackend * conn;
	public:
		SimpleEditor(DataBackend * conn_,QWidget * parent);
		bool getColl(const Xmms::Coll::Coll & collection);
		void saveColl(std::string oldName, std::string newName);
		void setColl(std::string name);
		void removeItem(SimpleEditorItem * item);
	private slots:
		void addBlank();
	};

class CollectionEditor:public QFrame
	{	Q_OBJECT
	private:
		std::string currColl;
		bool complexEditor;
		SimpleEditor * simple;
		QLabel * complex;
		QComboBox * switcher;
		QPushButton * toggler;
		QPushButton * remove;
		QLineEdit * saveName;
		QPushButton * save;
		QPushButton * reset;
		QGridLayout * layout;
		DataBackend * conn;
	public:
		CollectionEditor(DataBackend * conn_,QWidget * parent);
		bool listCollections(const Xmms::List<std::string>& list);
	public slots:
		void collectionsChanged(const Xmms::Dict& change);
		void toggleEditor();
		void removeCollection();
		void saveCollection();
		void resetCollection();
		void selectCollection(const QString & text);
//		void update(int width, int height);
	};

#endif
