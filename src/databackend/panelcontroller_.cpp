#ifndef PANELCONTROLLER__CPP
#define PANELCONTROLLER__CPP
#include "panelcontroller_.h"
PanelController_::PanelController_(int width):QObject()
	{
	screenWidth=width;
	animator=new QTimer();
	animator->setInterval(ANIMATION_INTERVAL);
	animator->setSingleShot(false);
	left=NULL;
	right=NULL;
	stuck=false;
	leftAnimating=false;
	leftAnimationTarget=-1;
	rightAnimating=false;
	rightAnimationTarget=-1;
	}

bool PanelController_::getSide(QString name)
	{
	if((name.split("$%$")[0])=="l")
		return false;
	if((name.split("$%$")[0])=="r")
		return true;
	return 0;
	}

QString PanelController_::getId(QString name)
	{
	}

void PanelController_::changeToPanel(bool rightside, QString name)
	{
	}

void PanelController_::startAnim()
	{
	}

void PanelController_::stopAnim()
	{
	}

void PanelController_::registerPanel(Panel_ * panel,QString id,PanelControllerFlag f)
	{
	}

void PanelController_::demandPanel(QString id)
	{
	}

void PanelController_::demandPanel(QString id, bool keepSide)
	{
	}

QStringList PanelController_::getPanelNames()
	{
	}

Panel_ * PanelController_::getPanel(QString name, bool right_side)
	{
	}

void PanelController_::animate()
	{
	}

void PanelController_::handleDraggedTo(Panel_ *,int)
	{
	}

void PanelController_::timerClicked(Panel_ *)
	{
	}

void PanelController_::rightClicked(Panel_ *,int,int)
	{
	}

void PanelController_::scrolledUp(Panel_ *)
	{
	}

void PanelController_::scrolledDown(Panel_ *)
	{
	}
#endif
