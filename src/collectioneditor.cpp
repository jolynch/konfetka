#ifndef COLLECTIONEDITOR_CPP
#define COLLECTIONEDITOR_CPP
#include "collectioneditor.h"
///COMPLEX EDITOR ---> complexeditor.h
	//Cause this file is too damn long already.
///SIMPLE EDITOR ITEM
SimpleEditorItem::SimpleEditorItem(std::string curr,Xmms::Coll::Coll & collection_,DataBackend * conn_,QWidget * parent):QFrame(parent)
	{
	conn=conn_;
	p=parent;
	currColl=curr;
	coll=&collection_;
	complexItem=false;
	complexError=new QLabel("This collection tree is a complex structure. Use the complex editor to edit.");
	negator=new QComboBox();
	negator->addItem("is");
	negator->addItem("is not");
	negator->setCurrentIndex(0);
	property=new QComboBox();
	property->addItem("music");
	//TODO: QSetting this list, and add a parser in the necessary spot:
	QStringList properties;
	properties<<"artist"<<"title"<<"album"<<"genre";
	property->addItems(properties);
	property->setCurrentIndex(0);
	type=new QComboBox();
	//Do not QSettings this! / Static
	QStringList types;
	types<<"included"<<"equal exactly to"<<"containing"<<"greater than"<<"smaller than";
	type->addItems(types);
	value=new QLineEdit();
	header=new QLabel("In");
	header->setFixedWidth(42);
	reference=new QComboBox();
	reference->addItem("All Media");
	conn->collection.list(Xmms::Collection::COLLECTIONS)(Xmms::bind(&SimpleEditorItem::listCollections, this));
	remove=new QPushButton();
	remove->setIcon(QIcon(":images/minus.png"));
	remove->setFixedHeight(32);
	remove->setToolTip("Remove property");
	remove->setFixedWidth(32);
	layout=new QGridLayout();
	Xmms::Coll::Type type_=coll->getType();
	bool run=true;
	std::string val=" ";
	for(int i=0;run&&i<2;i++)
		{
		type_=coll->getType();
		switch(type_)
			{
			case Xmms::Coll::COMPLEMENT:
				if(i==1)
					{
					layout->addWidget(complexError);
					complexItem=true;
					break;
					}
				negator->setCurrentIndex(1);
				coll=coll->getOperand().get();
				break;
			case Xmms::Coll::HAS:
				type->setCurrentIndex(0);
				changeProp((*coll)["field"]);
				val=(*coll)["value"];
				value->setText(val.c_str());
				run=false;
				break;
			case Xmms::Coll::EQUALS:
				type->setCurrentIndex(1);
				changeProp((*coll)["field"]);
				val=(*coll)["value"];
				value->setText(val.c_str());
				run=false;
				break;
			case Xmms::Coll::MATCH:
				type->setCurrentIndex(2);
				changeProp((*coll)["field"]);
				val=(*coll)["value"];
				value->setText(val.c_str());
				run=false;
				break;
			case Xmms::Coll::SMALLER:
				type->setCurrentIndex(4);
				changeProp((*coll)["field"]);
				val=(*coll)["value"];
				value->setText(val.c_str());
				run=false;
				break;
			case Xmms::Coll::GREATER:
				type->setCurrentIndex(3);
				changeProp((*coll)["field"]);
				val=(*coll)["value"];
				value->setText(val.c_str());
				run=false;
				break;
			case Xmms::Coll::REFERENCE:
				type->setCurrentIndex(0);
				property->setCurrentIndex(0);
				run=false;
				break;
			default:
				layout->addWidget(complexError);
				run=false;
				complexItem=true;
				break;
			};
		}
	if(!complexItem)
		{
		layout->addWidget(header,0,0);
		layout->addWidget(reference,0,1,1,4);
		layout->addWidget(property,1,0,1,2);
		layout->addWidget(negator,1,2);
		layout->addWidget(type,1,3);
		layout->addWidget(value,1,4,1,2);
		layout->addWidget(remove,0,5);
		}
	QObject::connect(remove,SIGNAL(clicked()),this,SLOT(selfRemove()));
	this->setLayout(layout);
	this->setFixedHeight(100);
	this->setFrameStyle(QFrame::StyledPanel|QFrame::Sunken);
	this->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));
	}

void SimpleEditorItem::changeProp(std::string val)
	{
	QString value=QString(val.c_str());
	if(property->findText(value)!=-1)
		property->setCurrentIndex(property->findText(value));
	else
		{
		property->addItem(value);
		property->setCurrentIndex(property->findText(value));
		}
	}

SimpleEditorItem::~SimpleEditorItem()
	{
	delete complexError;
	delete negator;
	delete property;
	delete type;
	delete value;
	delete header;
	delete reference;
	delete remove;
	delete layout;
	}

bool SimpleEditorItem::listCollections(const Xmms::List<std::string>& list)
	{
	reference->setCurrentIndex(0);
	QString current=reference->currentText();
	for (int i=0;i< reference->count(); i++) {reference->removeItem(i);}
	for (list.first (); list.isValid (); ++list)
		{
		if(*list==currColl) continue;
		reference->addItem((*list).c_str());
		}
	if(reference->findText(current)!=1)
		reference->setCurrentIndex(reference->findText(current));
	return true;
	}

void SimpleEditorItem::setColl(std::string name)
	{
	currColl=name;
	conn->collection.list(Xmms::Collection::COLLECTIONS)(Xmms::bind(&SimpleEditorItem::listCollections, this));
	}

Xmms::Coll::Coll * SimpleEditorItem::toColl()
	{
	if(complexItem) return coll;
	Xmms::Coll::Coll * out;
	if(reference->currentText()=="All Media")
		out=new Xmms::Coll::Universe();
	else
		out=new Xmms::Coll::Reference(reference->currentText().toStdString(),Xmms::Collection::COLLECTIONS);
	if(property->currentText()!="music")
		switch(type->currentIndex())
			{
			case 0:
				out=new Xmms::Coll::Equals(*out,property->currentText().toStdString());
				break;
			case 1:
				out=new Xmms::Coll::Equals(*out,property->currentText().toStdString(),value->text().toStdString());
				break;
			case 2:
				out=new Xmms::Coll::Match(*out,property->currentText().toStdString(),value->text().toStdString());
				break;
			case 3:
				out=new Xmms::Coll::Greater(*out,property->currentText().toStdString(),value->text().toStdString());
				break;
			case 4:
				out=new Xmms::Coll::Smaller(*out,property->currentText().toStdString(),value->text().toStdString());
				break;
		};
	if(negator->currentIndex()==1)
		{
		out=new Xmms::Coll::Complement(*out);
		}
	return out;
	}

void SimpleEditorItem::collectionsChanged(const Xmms::Dict& change)
	{setColl(currColl);}

void SimpleEditorItem::selfRemove()
	{((SimpleEditor*)(p))->removeItem(this);}

///SIMPLE EDITOR
SimpleEditor::SimpleEditor(DataBackend * conn_,QWidget * parent)
	{
	conn=conn_;
	layout=new QGridLayout();
	addItem=new QPushButton();
	addItem->setIcon(QIcon(":images/plus.png"));
	addItem->setToolTip("Add new property");
	addItem->setFixedWidth(42);
	itemArea=new QWidget();
	itemLayout=new QVBoxLayout();
	layout->addWidget(addItem,1,1);
	itemArea->setLayout(itemLayout);
	scroller=new QScrollArea();
	scroller->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
	layout->addWidget(scroller,0,0,1,2);
	scroller->setWidget(itemArea);
	scroller->setWidgetResizable(true);
	this->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
	this->setLayout(layout);
	QObject::connect(addItem,SIGNAL(clicked()),this,SLOT(addBlank()));
	}

bool SimpleEditor::getColl(const Xmms::Coll::Coll & collection)
	{
	for(int i=items.size()-1; i>=0; i--)
		{
		itemLayout->removeWidget(items[i]);
		delete items[i];
		items.removeAt(i);
		}
	if(collection.getType()!=Xmms::Coll::UNION)
		{
		SimpleEditorItem * singleItem=new SimpleEditorItem(currColl,const_cast<Xmms::Coll::Coll &> (collection),conn,this);
		itemLayout->addWidget(singleItem);
		items.append(singleItem);
		return false;
		}
	Xmms::Coll::OperandIterator itr=collection.getOperandIterator();
	itr.first();
	while(itr.valid())
		{
		SimpleEditorItem * singleItem=new SimpleEditorItem(currColl,*(*itr).get(),conn,this);
		itemLayout->addWidget(singleItem);
		items.append(singleItem);
		itr.next();
		}
	return false;
	}

void SimpleEditor::saveColl(std::string oldName, std::string newName)
	{
	if(newName=="Enter collection name")
		return;
	Xmms::Coll::Coll * save;
	if(items.size()==0)
		save=new Xmms::Coll::Universe();
	else if(items.size()==1)
		save=items[0]->toColl();
	else
		{
		save=new Xmms::Coll::Intersection();
		for(int i=0; i<items.size(); i++)
			{
			save->addOperand(*(items[i]->toColl()));
			}
		}
	if(oldName==newName)
		conn->collection.save(*save,newName,Xmms::Collection::COLLECTIONS)(Xmms::bind(&DataBackend::scrapResult, conn));
	else
		{
		conn->collection.save(*save,newName,Xmms::Collection::COLLECTIONS)(Xmms::bind(&DataBackend::scrapResult, conn));
		conn->collection.remove(oldName,Xmms::Collection::COLLECTIONS)(Xmms::bind(&DataBackend::scrapResult, conn));
		}
	}

void SimpleEditor::setColl(std::string name)
	{
	currColl=name;
	if(name!="Enter collection name")
		conn->collection.get(name,Xmms::Collection::COLLECTIONS)(Xmms::bind(&SimpleEditor::getColl,this));
	else
		for(int i=items.size()-1; i>=0; i--)
			{
			itemLayout->removeWidget(items[i]);
			delete items[i];
			items.removeAt(i);
			}
	}

void SimpleEditor::removeItem(SimpleEditorItem * item)
	{
	itemLayout->removeWidget(item);
	items.removeAt(items.indexOf(item));
	delete item;
	}

void SimpleEditor::addBlank()
	{
	Xmms::Coll::Universe coll;
	SimpleEditorItem * item=new SimpleEditorItem(currColl,coll,conn,this);
	items.append(item);
	itemLayout->addWidget(item);
	}

///COLLECTION EDITOR
CollectionEditor::CollectionEditor(DataBackend * conn_,QWidget * parent):QFrame(parent)
	{
	conn=conn_;
	currColl="Create New Collection";
	simple=new SimpleEditor(conn,this);
	simple->setColl(currColl);
	complex=new QLabel("Complex Editor Here");
	switcher=new QComboBox();
	switcher->addItem(currColl.c_str());
	toggler=new QPushButton();
	toggler->setText("Simple Edit:");
	remove=new QPushButton();
	remove->setIcon(QIcon(":images/trashcan.png"));
	remove->setFixedWidth(42);
	remove->setToolTip("Delete collection");
	saveName=new QLineEdit();
	saveName->setText("Enter collection name");
	save=new QPushButton();
	save->setIcon(QIcon(":images/save.png"));
	save->setFixedWidth(42);
	save->setToolTip("Save collection");
	reset=new QPushButton();
	reset->setIcon(QIcon(":images/quit_button.png"));
	reset->setFixedWidth(42);
	reset->setToolTip("Discard changes");
	layout=new QGridLayout();
	layout->addWidget(simple,1,0,1,7);
	layout->addWidget(complex,1,0,1,7);
	complex->hide();
	simple->show();
	layout->addWidget(switcher,0,2,1,5);
	layout->addWidget(toggler,0,0,1,2);
	layout->addWidget(remove,2,0,1,1);
	layout->addWidget(saveName,2,1,1,4);
	layout->addWidget(save,2,5,1,1);
	layout->addWidget(reset,2,6,1,1);
	this->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
	this->setLayout(layout);
	this->setFrameStyle(QFrame::StyledPanel|QFrame::Sunken);
	QObject::connect(conn,SIGNAL(collectionsChanged(const Xmms::Dict&)),
						this,SLOT(collectionsChanged(const Xmms::Dict&)));
	conn->collection.list(Xmms::Collection::COLLECTIONS)
				 (Xmms::bind(&CollectionEditor::listCollections, this));
	QObject::connect(remove,SIGNAL(clicked()),this,SLOT(removeCollection()));
	QObject::connect(toggler,SIGNAL(clicked()),this,SLOT(toggleEditor()));
	QObject::connect(reset,SIGNAL(clicked()),this,SLOT(resetCollection()));
	QObject::connect(save,SIGNAL(clicked()),this,SLOT(saveCollection()));
	QObject::connect(switcher,SIGNAL(currentIndexChanged(const QString &)),this,SLOT(selectCollection(const QString &)));
	};

bool CollectionEditor::listCollections(const Xmms::List<std::string>& list)
	{
	switcher->setCurrentIndex(0);
	for (int i=switcher->count()-1; i>=1; i--) {switcher->removeItem(i);}
	for (list.first (); list.isValid (); ++list)
		switcher->addItem((*list).c_str());
	if(switcher->findText(currColl.c_str())!=-1)
		switcher->setCurrentIndex(switcher->findText(currColl.c_str()));
	return true;
	}


void CollectionEditor::collectionsChanged(const Xmms::Dict& change)
	{
	conn->collection.list(Xmms::Collection::COLLECTIONS)
		(Xmms::bind(&CollectionEditor::listCollections, this));
	}

void CollectionEditor::toggleEditor()
	{
	/*if(!complexEditor)
		{
		simple->hide();
		complex->show();
		complexEditor=true;
		selectCollection(QString(currColl.c_str()));
		toggler->setText("Complex Edit:");
		}
	else
		}
		complex->hide();
		simple->show();
		complexEditor=false;
		selectCollection(QString(currColl.c_str()));
		toggler->setText("Simple Edit:");
		}*/
	}

void CollectionEditor::removeCollection()
	{
	if(currColl=="Enter collection name")
		return;
	else
		conn->collection.remove(currColl,Xmms::Collection::COLLECTIONS)(Xmms::bind(&DataBackend::scrapResult, conn));
	}

void CollectionEditor::saveCollection()
	{
	std::string newName=saveName->text().toStdString();
	if(newName=="Enter collection name")
		{
		QMessageBox::critical(0, "No save name entered", "Please enter a save name.");
		return;
		}
	if(complexEditor){}
//		complex->saveColl(currColl, newName);
	else
		simple->saveColl(currColl, newName);
	}

void CollectionEditor::resetCollection()
	{
	saveName->setText(currColl.c_str());
	if(complexEditor){}
//		complex->setColl(currColl);
	else
		simple->setColl(currColl);
	}

void CollectionEditor::selectCollection(const QString & text)
	{
	if(text=="Create New Collection")
		saveName->setText("Enter collection name");
	else
		saveName->setText(text);
	currColl=saveName->text().toStdString();
	if(complexEditor){}
//		complex->setColl(currColl);
	else
		simple->setColl(currColl);
	}
#endif
