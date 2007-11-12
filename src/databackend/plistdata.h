#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#endif
#ifndef PLISTDATA_H
#define PLISTATA_H
#include "colldata.h"
#include "../databackend.h"
#include <QObject>
#include <QString>
#include <QHash>
#include <string>


class SinglePlaylist:public QObject
	{	Q_OBJECT
	private:
		bool connected;
/*		QList <uint> ids;
		DataBackend * conn;
*/	public:
		SinglePlaylist(DataBackend * c);
		bool isConnected();
		void connectToggle();
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
		DataBackend * conn;
		std::string currentPlist;
		QHash <QString, SinglePlaylist *> plists;

		void createPlaylist(std::string name);
		void refreshPlaylist(SinglePlaylist * plist,std::string name="");

		void connectToServer(SinglePlaylist * plist);
		void disconnectFromServer(SinglePlaylist * plist);
	public:
		PlistData(DataBackend * c,QObject * parent=0);
		SinglePlaylist * getPlist (std::string plist="");
		QString getCurrentName();
	public slots:
		void setCurrentName(const std::string & name);
		void collectionChanged(QString name, Xmms::Collection::Namespace ns);
	signals:
		void playlistReady(std::string,SinglePlaylist *);
	};

#endif


