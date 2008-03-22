#ifndef PANELCONTROLLER_H
#define PANELCONTROLLER_H
#include <QObject>
#include <QTimer>
#include <QString>
#include <QList>
#include <QHash>
#include <QAction>
#include <QMenu>
#include <QFont>
#include <QIcon>
#include "../panel.h"

typedef int PanelControllerFlag;

#define ANIMATION_STEP 100
#define ANIMATION_INTERVAL 33

class PanelController:public QObject
	{	Q_OBJECT
	private:
		int timerId;
		Panel * left;
		Panel * right;
		int screenWidth;
		bool stuck;
		bool leftAnimating;  int leftAnimationTarget;
		bool rightAnimating; int rightAnimationTarget;
		QHash <QString, Panel *> nameHash;
		QHash <Panel *, QString> panelHash;
		QList <QString> leftPanels;
		QList <QString> rightPanels;
		QStringList panelNames;
		
		bool getSide(QString name);
		QString getId(QString name);
		QString getName(QString id,PanelControllerFlag f);
		QString getName(QString id,bool s);

		void changeToPanel(QString name);

		void timerEvent ( QTimerEvent * event );

		static const PanelControllerFlag BLANK		=0x000000;
	public:
		static const PanelControllerFlag LEFTPANEL 	=0x000001;
		static const PanelControllerFlag RIGHTPANEL	=0x000010;
		static const PanelControllerFlag LAYOUT_PANEL	=0x000100;

		PanelController(int width);
		/*Restrictions on IDs that I remembered to put here:
		**	IDs MUST NOT be repeated on each side. i.e. a side can only have one id of "Playlist". The other side can have an id of "Playlist" also.
		**	No id can be "lock".
		**	Can't use "$%$" in an id.
		*/
		void registerPanel(Panel * panel,QString id,PanelControllerFlag f);
		void demandPanel(QString id); //Add more options probably
		void demandPanel(QString id, bool scrapSide); //keeps a side
		QStringList getPanelNames();
		Panel * getPanel(QString id, bool right_side=false);

	public slots:
		void handleDraggedTo(Panel *,int);
		void timerClicked(Panel *);
		void rightClicked(Panel *,int,int);
		void scrolledUp(Panel *);
		void scrolledDown(Panel *);
		void release();

	signals:
		void newPanelRegistered(QString /*id*/,bool /*side*/);
	//Right click menu stuff
	private:
		QList<QAction *> leftActions;
		QList<QAction *> rightActions;
		QList<QAction *> findReplace(QString name);
		
	};

#endif
