#ifndef COLLECTION_CPP
#define COLLECTION_CPP
#include "collection.h"

Collection::Collection (std::string name_,Xmms::Collection::Namespace nspace_,
			QGraphicsItem* parent,QGraphicsScene* scene_):QGraphicsItem(parent,scene_)
	{
	this->setFlag(QGraphicsItem::ItemIsSelectable,true);
	this->setFlag(QGraphicsItem::ItemIsFocusable,true);
	this->setFlag(QGraphicsItem::ItemIsMovable,true);
	reference=true;
	name=name_;
	nspace=nspace_;
	scene=scene_;
	}

Collection::Collection (Xmms::Coll::Type type_,
			QGraphicsItem* parent,QGraphicsScene* scene_):QGraphicsItem(parent,scene_)
	{
	this->setFlag(QGraphicsItem::ItemIsSelectable,true);
	this->setFlag(QGraphicsItem::ItemIsFocusable,true);
	this->setFlag(QGraphicsItem::ItemIsMovable,true);
	reference=false;
	type=type_;
	scene=scene_;
	}

QRectF Collection::boundingRect() const
	{
	return QRectF(0.0,0.0,width,height);
	}

void Collection::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
	{
	if(this->isSelected())
		painter->setBrush(QColor(96,166,198,188));
	else
		painter->setBrush(QColor(169,204,220,188));
	painter->drawRect(0,0,width,height);
	painter->setBrush(Qt::black);
	QString * type_;
	switch(type)
		{
		case  XMMS_COLLECTION_TYPE_UNION:
		type_=new QString("Added collections"); break;
		case  XMMS_COLLECTION_TYPE_INTERSECTION:
		type_=new QString("Intersection of collections"); break;
		case  XMMS_COLLECTION_TYPE_COMPLEMENT:
		type_=new QString("Not"); break;
		case  XMMS_COLLECTION_TYPE_HAS:
		type_=new QString("Has a value"); break;
		case  XMMS_COLLECTION_TYPE_EQUALS:
		type_=new QString("Value equals exactly"); break;
		case  XMMS_COLLECTION_TYPE_MATCH:
		type_=new QString("Value contains"); break;
		case  XMMS_COLLECTION_TYPE_SMALLER:
		type_=new QString("Value smaller than"); break;
		case  XMMS_COLLECTION_TYPE_GREATER:
		type_=new QString("Value greater than"); break;
		case  XMMS_COLLECTION_TYPE_IDLIST:
		type_=new QString("Song list"); break;
		case  XMMS_COLLECTION_TYPE_QUEUE:
		type_=new QString("Queue"); break;
		case  XMMS_COLLECTION_TYPE_PARTYSHUFFLE:
		type_=new QString("Party shuffle"); break;
		};
	if(!reference)
		painter->drawText(5,5,width-10.0,height-10.0,
					Qt::AlignCenter|Qt::TextWordWrap,*type_);
	else
		{
		QString name_(name.c_str());
		name_.prepend("Reference: ");
		painter->drawText(5,5,width-10.0,height-10.0,
					Qt::AlignCenter|Qt::TextWordWrap,name_);
		}
	}

void Collection::mousePressEvent ( QGraphicsSceneMouseEvent * event )
	{
	}

#endif
