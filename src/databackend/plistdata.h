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

class SinglePlaylist:public QObject
	{	Q_OBJECT
/*	private:
		QList <uint> ids;
		DataBackend * conn;
	public:
*/		SinglePlaylist(DataBackend * c);
/*		QList <uint> getIds();
		void setIds(QList <uint> newVals);
	public slots:
		void respondToChanges(const Xmms::Dict& val);
		void append(std::string url);
		void append(uint id);
		void insert(std::string url, uint pos);
		void insert(uint id, uint pos);
		void remove(uint pos);
*/	};

class PlistData:public QObject
	{	Q_OBJECT
	private:
/*		DataBackend * conn;
		std::string currentPlist;
		QHash <std::string, SinglePlaylist *> plists;

		void createPlaylist(std::string name);
		void refreshPlaylist(SinglePlaylist * plist);

		void connectToServer(SinglePlaylist * plist);
		void disconnectFromServer(SinglePlaylist * plist);
*/	public:
		PlistData(DataBackend * c,QObject * parent=0);
/*		SinglePlaylist * operator[] (std::string plist="");
		std::string getCurrentName();
	public slots:
		void setCurrentName(const std::string & name);
		void collectionChanged(QString name, Xmms::Collection::Namespace ns);
*/	};

#endif


