#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#endif
#ifndef PLISTDATA_H
#define PLISTATA_H
// #include "../databackend.h"
#include <QObject>
#include <QString>
#include <QHash>
#include <string>

class DataBackend;

class SinglePlaylist
	{
// 	private:
// 		QList <uint> ids;
// 		DataBackend * conn;
	public:
		SinglePlaylist(DataBackend * c);
// 		QList <uint> getIds();
// 		void setIds(QList <uint> newVals);
// 	public slots:
// 		void append(std::string url);
// 		void append(uint id);
// 		void insert(std::string url, uint pos);
// 		void insert(uint id, uint pos);
// 		void remove(uint pos);
	};

class PlistData:public QObject
	{	Q_OBJECT
// 	private:
// 		DataBackend * conn;
// 		std::string currentPlist;
// 		QHash <std::string, SinglePlaylist *> plists;
// 		uint currentPos;
	public:
		PlistData(DataBackend * c,QObject * parent=0);
// 		SinglePlaylist * operator[] (std::string plist="");
// 		uint getCurrentPos();
// 		std::string getCurrentName();
// 	public slots:
// 		void respondToChanges(const Xmms::Dict& val);
// 		void setCurrentName(const std::string & name);
// 		void setCurrentPos(const unsigned int& val);
	};

#endif


