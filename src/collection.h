#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#include <xmmsclient/xmmsclient.h>
#endif

#ifndef COLLECTION_H
#define COLLECTION_H
#include "databackend.h"
#include <string>
#include <iostream>
#include <QObject>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QRect>
#include <QRectF>
#include <QList>
#include <QPair>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

class Collection : public QGraphicsItem
	{
	private:
		static const double width=100.0;
		static const double height=50.0;
		bool reference;
		Xmms::Coll::Coll * ncoll;
		std::string name;
		Xmms::Collection::Namespace nspace;
		Xmms::Coll::Type type;
		QList <QPair <Collection*,QGraphicsLineItem*> > links;
		QGraphicsScene* scene;
	public:
		Collection (std::string name_,Xmms::Collection::Namespace nspace_,
						QGraphicsItem* parent,QGraphicsScene* scene_);
		Collection (Xmms::Coll::Type type_,QGraphicsItem* parent,QGraphicsScene* scene_);
		QRectF boundingRect() const;
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);
		void mousePressEvent ( QGraphicsSceneMouseEvent * event );
	};

#endif
