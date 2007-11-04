#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#include <xmmsclient/xmmsclient.h>
#endif

#ifndef COMPLEXEDITOR_CPP
#define COMPLEXEDITOR_CPP
#include "complexeditor.h"
/*CollectionEditor::CollectionEditor(XMMS2Interface * conn_,QWidget * parent):QWidget(parent)
	{
	middle=new QFrame();
	middle->setFrameStyle(QFrame::StyledPanel);
	QPalette pal=middle->palette();
	middle->setAutoFillBackground(true);
	pal.setColor(QPalette::Window,QColor(223,223,223,127));
	pal.setColor(QPalette::Button,QColor(223,223,223,127));
	middle->setPalette(pal);
	middleLayout=new QGridLayout();
	middle->setLayout(middleLayout);
	conn=conn_;
	view=new QGraphicsView(parent);
	scene=new QGraphicsScene(parent);
	view->setScene(scene);
	layout=new QGridLayout();
	layout->addWidget(view,0,0,3,3);
	layout->addWidget(middle,1,1);
	layout->setColumnMinimumWidth(0,20);
	layout->setColumnMinimumWidth(2,20);
	layout->setRowMinimumHeight(0,20);
	layout->setRowMinimumHeight(2,20);
	this->setLayout(layout);

	chooser=new QListView(middle);
	chooserModel=new QStringListModel();
	chooser->setModel(chooserModel);
	middleLayout->addWidget(chooser,0,0,1,3);
	open=new QPushButton();
	open->setText("Open collection");
	new_=new QPushButton();
	new_->setText("New collection");
	middleLayout->addWidget(open,1,2);
	middleLayout->addWidget(new_,1,1);
	QObject::connect(conn,SIGNAL(collectionsChanged(const Xmms::Dict&)),
						this,SLOT(collectionsChanged(const Xmms::Dict&)));
	conn->collection.list(Xmms::Collection::ALL)
				 (Xmms::bind(&CollectionEditor::listCollections, this));
	QObject::connect(new_,SIGNAL(clicked()),this,SLOT(newColl()));
	Collection * bar=new Collection(XMMS_COLLECTION_TYPE_UNION,0,scene);
	}

bool CollectionEditor::listCollections(const Xmms::List<std::string>& list)
	{
	QStringList qlist;
	for (list.first (); list.isValid (); ++list)
		qlist<<(*list).c_str();
	chooserModel->setStringList(qlist);
	return true;
	}

void CollectionEditor::collectionsChanged(const Xmms::Dict& change)
	{
	conn->collection.list(Xmms::Collection::ALL)
				 (Xmms::bind(&CollectionEditor::listCollections, this));
	}

void CollectionEditor::update(int width, int height)
	{
	layout->setColumnMinimumWidth(1,width-75);
	layout->setRowMinimumHeight(1,height-50);
	}

void CollectionEditor::newColl()
	{
	middle->hide();
	}*/

/*ComplexEditorView::ComplexEditorView(QWidget * parent = 0)
	{
	QMenu * rcMenu;
	}

void ComplexEditorView::mousePressEvent ( QMouseEvent * e )
	{
	}

void ComplexEditorView::save()
	{
	}*/

#endif
