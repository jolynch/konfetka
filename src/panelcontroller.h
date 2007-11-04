#ifndef PANELCONTROLLER_H
#define PANELCONTROLLER_H
#include "panel.h"
#include "layoutpanel.h"
#include <QObject>
#include <QWidget>
#include <QList>
#include <QHash>

typedef int PanelControllerOption;
typedef QList<Panel *> PanelPtrList;

class PanelController:public QObject
	{	Q_OBJECT
	private:
		Panel * left;
		Panel * right;
		int x_left; int x_right; int width;
		PanelPtrList left_list;
		PanelPtrList right_list;
		QHash<Panel *,PanelPtrList> incompatibleList;
		QHash<Panel *,PanelControllerOption> flags;
		bool glued, moveBack;

/*		STATE defs
		typedef int PanelState;
		PanelState current;
		static const PanelState LeftSelected		=0x000001;
		static const PanelState RightSelected		=0x000010;
		static const PanelState NoneSelected		=0x000100;
		
		
//		END STATE*/

		static const PanelControllerOption BLANK		=0x000000;
	public:
//		STATIC FLAGS for PanelController::registerPanel
		static const PanelControllerOption LEFTPANEL 		=0x000001;
		static const PanelControllerOption RIGHTPANEL		=0x000010;
		static const PanelControllerOption LAYOUT_PANEL		=0x000100;
		static const PanelControllerOption INCOMPATIBLE_WITH	=0x001000;
		static const PanelControllerOption DOUBLE_SIDED_PANEL	=0x011011;
//		END STATIC FLAGS

		PanelController(int width_);
		/*!PanelController::registerPanel
		@arg p: panel to register.
		@arg options: Bitwise or'd list of the following options:
			PanelController::LEFTPANEL
				p is added to the left side.
			PanelController::RIGHTPANEL
				p is added to the right side.
			PanelController::LAYOUT_PANEL
				p inherits LayoutPanel and can change its layout based on the side it is on.
			PanelController::INCOMPATIBLE_WITH
				p must not be displayed alongside the panels in incompat_list
			PanelController::DOUBLE_SIDED_PANEL
				implies PanelController::LEFTPANEL and PanelController::RIGHTPANEL
				also implies PanelController::INCOMPATIBLE_WITH since we do not want the two sides displaying
					the same panel, with the same changes in data.
					!p must be present in incompat_list!
		@arg incompat_list: ignored if PanelController::INCOMPATIBLE_WITH is not set.
		@arg preferred: ignored if PanelController::DOUBLE_SIDED_PANEL is not set. Sets the inital side for the panel.*/
		void registerPanel(Panel * p, PanelControllerOption options, PanelPtrList * incompat_list=NULL,bool preferred=true);
	public slots:
		void leftCycle(bool up);
		void rightCycle(bool up);
		void processMove(bool timer,int newX,int y,bool side_right,Panel * p);
		void released(Panel * p);
	signals:
		void endTimers();
	};
#endif
