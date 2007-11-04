#ifndef COLLECTIONCHOOSER_CPP
#define COLLECTIONCHOOSER_CPP
#include "collectionchooser.h"

CollectionChooser::CollectionChooser(std::string pl, Xmms::Collection::Namespace t,DataBackend * c,
								QWidget* parent, Qt::WindowFlags f):QWidget(parent, f)
	{
	conn=c;
	plist=pl;
	type=t;
	layout=new QGridLayout;
	model=new QStringListModel();
	list=new QListView();
	list->setModel(model);
	layout->addWidget(list,0,0,1,6);
	info=new QLabel("Sort by:");
	info->setFixedHeight(32);
	layout->addWidget(info,1,0,1,2);
	filename=new QCheckBox("Filename");
	filename->setFixedHeight(32);
	layout->addWidget(filename,2,0);
	artist=new QCheckBox("Artist");
	artist->setFixedHeight(32);
	layout->addWidget(artist,2,1);
	album=new QCheckBox("Album");
	album->setFixedHeight(32);
	layout->addWidget(album,2,2);
	title=new QCheckBox("Title");
	title->setFixedHeight(32);
	layout->addWidget(title,2,3);
	done=new QPushButton();
	done->setFixedSize(32,32);
	done->setIcon(QIcon(":images/plus.png"));
	done->setToolTip("Add selected");
	layout->addWidget(done,3,4);
	cancel=new QPushButton();
	cancel->setFixedSize(32,32);
	cancel->setIcon(QIcon(":images/quit_button.png"));
	cancel->setToolTip("Cancel");
	layout->addWidget(cancel,3,5);
	QObject::connect(conn,SIGNAL(collectionsChanged(const Xmms::Dict&)),
							this,SLOT(collectionsChanged(const Xmms::Dict&)));
	QObject::connect(done,SIGNAL(clicked()),this,SLOT(addClicked()));
	QObject::connect(cancel,SIGNAL(clicked()),this,SLOT(cancelPressed()));
	QObject::connect(list,SIGNAL(doubleClicked(const QModelIndex &)),
					this,SLOT(doubleClick(const QModelIndex &)));
	conn->collection.list(type) (Xmms::bind(&CollectionChooser::listCollections, this));
	this->setLayout(layout);
	}

bool CollectionChooser::listCollections(const Xmms::List<std::string>& list)
	{
	QStringList qlist;
	for (list.first (); list.isValid (); ++list)
		qlist<<(*list).c_str();
	model->setStringList(qlist);
	return true;
	}

void CollectionChooser::collectionsChanged(const Xmms::Dict& change)
	{
	conn->collection.list(type) (Xmms::bind(&CollectionChooser::listCollections, this));
	}

void CollectionChooser::cancelPressed()
	{
	emit(close());
	}

void CollectionChooser::addClicked()
	{
	QModelIndex idx=list->currentIndex();
	QString dat=model->data(idx,Qt::DisplayRole).toString();
	conn->collection.get(dat.toStdString(),type)
					(Xmms::bind(&CollectionChooser::add,this));
	}

bool CollectionChooser::add(const Xmms::Coll::Coll& collection)
	{
	std::list<std::string> ops;
	if(filename->checkState()==Qt::Checked)
		ops.push_back("url");
	if(artist->checkState()==Qt::Checked)
		ops.push_back("artist");
	if(album->checkState()==Qt::Checked)
		ops.push_back("album");
	if(title->checkState()==Qt::Checked)
		ops.push_back("title");
	if(ops.size()==0)
		ops.push_back("url");
	conn->playlist.addCollection(collection,ops,plist);
	emit(close());
	return true;
	}

void CollectionChooser::doubleClick(const QModelIndex & index)
	{
	addClicked();
	}

void CollectionChooser::setPlist(std::string pl) {plist=pl;}
#endif
