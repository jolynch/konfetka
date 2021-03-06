#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#include <xmmsclient/xmmsclient.h>
#endif

#ifndef INFOEDITOR_H
#define INFOEDITOR_H
#include "databackend.h"
#include "databackend/mlibdata.h"
#include <QWidget>
#include <QScrollArea>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QIcon>
#include <QPushButton>
#include <sstream>

class InfoEditor:public QWidget
	{	Q_OBJECT
	private:
		bool stopped;
		DataBackend * conn;
		int curId;
		QGridLayout * layout;
		QLabel * title;
		QLabel * artist;
		QLabel * album;
		QLabel * year;
		QLabel * track;
		QLabel * genre;
		QLineEdit * title_;
		QLineEdit * artist_;
		QLineEdit * album_;
		QLineEdit * year_;
		QLineEdit * track_;
		QLineEdit * genre_;
		QScrollArea * uneditable;
		QGridLayout * uneditableLayout;
		QLabel * notPlaying;
		QLabel * url;
		QLabel * bitrate;
		QLabel * channels;
		QLabel * timesPlayed;
		QPushButton * done;
		QPushButton * reset;
		MlibData * mlib;
	public:
		InfoEditor(DataBackend * c,bool autoUpdate = true,QWidget* parent=0, Qt::WindowFlags f=0);
	private slots:
		void slotDone();
		void slotReset();
		void handleInfoChanged(int id);
	public slots:
		void newId(int id);
		void newStatus(Xmms::Playback::Status status);
	};

#endif
