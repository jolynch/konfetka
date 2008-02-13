#ifndef PANELCONTROLLER__H
#define PANELCONTROLLER__H
#include <QObject>
#include <QTimer>
#include <QString>
#include <QList>
#include <QHash>
#include "../panel_.h"

typedef int PanelControllerFlag;

#define ANIMATION_STEP 100
#define ANIMATION_INTERVAL 20

class PanelController_:public QObject
	{
	private:
		QTimer * animator;
		Panel_ * left;
		Panel_ * right;
		int screenWidth;
		bool stuck;
		bool leftAnimating;  int leftAnimationTarget;
		bool rightAnimating; int rightAnimationTarget;
		QHash <QString, Panel_ *> nameHash;
		QList <Panel_ * > leftPanels;
		QList <Panel_ * > rightPanels;
		
		bool getSide(QString name);
		QString getId(QString name);

		void changeToPanel(bool rightside, QString name);
		void startAnim();
		void stopAnim();

		static const PanelControllerFlag BLANK		=0x000000;
	public:
		static const PanelControllerFlag LEFTPANEL 	=0x000001;
		static const PanelControllerFlag RIGHTPANEL	=0x000010;
		static const PanelControllerFlag LAYOUT_PANEL	=0x000100;

		PanelController_(int width);
		void registerPanel(Panel_ * panel,QString id,PanelControllerFlag f);
		void demandPanel(QString id); //Add more options probably
		void demandPanel(QString id, bool keepSide); //keeps a side
		QStringList getPanelNames();
		Panel_ * getPanel(QString name, bool right_side=false);
		
	public slots:
		void animate();
		void handleDraggedTo(Panel_ *,int);
		void timerClicked(Panel_ *);
		void rightClicked(Panel_ *,int,int);
		void scrolledUp(Panel_ *);
		void scrolledDown(Panel_ *);

	signals:
		void newPanelRegistered(QString /*id*/);
	};

#endif
