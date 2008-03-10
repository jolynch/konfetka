#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#include <xmmsclient/xmmsclient.h>
#endif
#ifndef DATABACKEND_H
#define DATABACKEND_H
#include "databackend/xmms2interface.h"
#include "databackend/xmmsqt4.h"
#include <QObject>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QQueue>
#include <QStack>
#include <cstdlib>
#include <iostream>
//For debugging
#include <QtDebug>

#define UPCOMING 10

typedef uint DataBackendType;

class DataBackend:public XMMS2Interface//, public QObject
	{	Q_OBJECT
	private:
	QObject * mlibData;
	QObject * plistData;
	QObject * collData;
	QObject * panelController;
	bool random;
	
	public:
	static const DataBackendType COLL	=0x0001;
	static const DataBackendType PLIST	=0x0010;
	static const DataBackendType MLIB	=0x0100;
	static const DataBackendType PANEL      =0x1000;
	
	DataBackend(QObject * parent, std::string name);
	QObject * getDataBackendObject(DataBackendType type);
	void initPanelController(int width);

///FORWARD/BACKWARD
	private:
	uint curr;
	QStack <uint> history;
	QQueue <uint> upcoming;

	void addSongIdToQueue();
	void fillUpcoming();
	void clearRandomData();
	void checkValidity(QQueue <uint> v);
	void checkValidity(QStack <uint> v);
	public slots:
	void playNextSong();
	void playPreviousSong();
	void currentPosChanged(uint);
	private slots:
	void getRandom(QString name, QVariant newValue);
	void playlistChanged(QString name);
	
///SETTINGS
	/**
	***All QSettings keys starting with "konfetka"
	***have their inital values sent out in qsettingsValueChanged
	**/
	public:
	void emitInitialQSettings();

	public slots:
	//Only call this from the options GUI, please.
	void changeAndSaveQSettings(QString name, QVariant newValue);

	signals:
	void qsettingsValueChanged(QString /*name*/, QVariant /*newValue*/);


///CURRENT SONG LOOPBACK
	//The following is for emitting the currentId signal anytime info is changed.
	private:
	int currId;

	private slots:
	void currentIdChanged(int newId);

	public slots:
	void infoChanged(int id);
	};

#endif

