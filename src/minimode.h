#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#include <xmmsclient/xmmsclient.h>
#endif
#ifndef MINIMODE_H
#define MINIMODE_H
#include "databackend.h"
#include "basicvis.h"
#include "albumart.h"
#include "mainbar.h"
#include "songpositionslider.h"
#include <QGridLayout>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QStringList>
class MiniMode:public QWidget
	{	Q_OBJECT
	private:
		bool timeMode;
		QGridLayout * layout;
		BasicVis * miniVis;
		AlbumArt * parentAlbArt;
		DataBackend * conn;
		QLabel * artLabel;
		QLabel * songInfo;
		QPushButton * play;
		QPushButton * pause;
		QPushButton * stop;
		QPushButton * back;
		QPushButton * forward;
		QPushButton * quit;
		QPushButton * timeButton;
		SongPositionSlider * position;
		QSlider * volume;
		QPoint whereAmI;
		QString curSongInfo;
		QTimer timer;
		bool highlight;
		QList<QWidget*> listOfWidgets;
	public:
		MiniMode(DataBackend * c,AlbumArt * a,QWidget * parent = 0, Qt::WindowFlags f = 0);

	public slots:
		void slotPlay();
		void slotStop();
		void slotPause();
		void slotBack();
		void slotForward();
		void setVolume();
		void setInfo();
		void loadNewPixmap(QPixmap);
		void setNewInfo(Xmms::PropDict);
		void changeTimeButton();
		void toggleTimeMode();
		void toggleSize();
	
	protected:
		void mouseMoveEvent(QMouseEvent *event);
		void mousePressEvent(QMouseEvent *event);
		void leaveEvent (QEvent *event); 
		void paintEvent(QPaintEvent *event);
		void resizeEvent(QResizeEvent* event);
	signals:
		void changeSongRel(int);
		void hideMe();
		void volumeValueChanged(int);
		void setVolumeValue(int);

};


#endif
