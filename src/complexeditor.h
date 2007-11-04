#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#include <xmmsclient/xmmsclient.h>
#endif

#ifndef COMPLEXEDITOR_H
#define COMPLEXEDITOR_H
#include <iostream>
#include <QWidget>

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>

#include <QMenu>
#include <QList>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
/*	private:
		QFrame * middle;
		QGridLayout * layout;
		QGridLayout * middleLayout;
		QListView * chooser;
		QStringListModel * chooserModel;
		QPushButton * open;
		QPushButton * new_;
		Collection * root;
		QGraphicsView * view;
		QGraphicsScene * scene;
		XMMS2Interface * conn;
	public:
		CollectionEditor(XMMS2Interface * conn_,QWidget * parent);
		bool listCollections(const Xmms::List<std::string>& list);
	public slots:
		void collectionsChanged(const Xmms::Dict& change);
		void update(int width, int height);
//		void setColl();
		void newColl();*/

/*class ComplexEditorItem:public QGraphicsItem
	{
	public:
		ComplexEditorItem();
		QRectF boundingRect() const;
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);
		void mousePressEvent ( QGraphicsSceneMouseEvent * event );
		void dropEvent ( QGraphicsSceneDragDropEvent * event );
	private:
		int xpos, ypos;
		QString text;
		QList <ComplexEditorItem *> upstreams;
	};

class ComplexEditorView:public QGraphicsView
	{	Q_OBJECT
	public:
		ComplexEditorView(QWidget * parent = 0);
		void mousePressEvent ( QMouseEvent * e );
	public slots:
		void save();
	private slots:
		
	private:
		QMenu * rcMenu;
		QList<ComplexEditorItem *> savepoints;
	}

class ComplexEditor:public QWidget
	{	Q_OBJECT
	public:
		ComplexEditor(XMMS2Interface * c,QWidget * parent = 0)
		bool getColl(const Xmms::Coll::Coll & collection);
		void saveColl(std::string oldName, std::string newName);
		void setColl(std::string name);
	private:
		XMMS2Interface * conn;
	};*/

#endif
