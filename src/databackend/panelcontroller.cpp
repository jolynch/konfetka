#ifndef PANELCONTROLLER_CPP
#define PANELCONTROLLER_CPP
#include "panelcontroller.h"
PanelController::PanelController(int width):QObject()
	{
	screenWidth=width;
	left=NULL;
	right=NULL;
	stuck=false;
	leftAnimating=false;
	leftAnimationTarget=-1;
	rightAnimating=false;
	rightAnimationTarget=-1;
	}

bool PanelController::getSide(QString name)
	{
	if((name.split("$%$")[0])=="l")
		return false;
	if((name.split("$%$")[0])=="r")
		return true;
	return 0;
	}

void PanelController::startTimer()
	{
	timerId=((QObject*)this)->startTimer(ANIMATION_INTERVAL);
	}
	
void PanelController::endTimer()
	{
	if(!leftAnimating&&!rightAnimating)
		killTimer(timerId);
	}

QString PanelController::getId(QString name)
	{return (name.split("$%$")[1]);}

QString PanelController::getName(QString id,PanelControllerFlag f)
	{
	QString out="";
	if((f&LEFTPANEL)!=BLANK) out="l";
	else if((f&RIGHTPANEL)!=BLANK) out="r";
	out.append("$%$");
	out.append(id);
	return out;
	}

QString PanelController::getName(QString id,bool s)
	{
	QString out="";
	if(!s) out="l";
	else out="r";
	out.append("$%$");
	out.append(id);
	return out;
	}

void PanelController::changeToPanel(QString name)
	{
	int lPos=0;
	if(getSide(name))
		{
		if(right!=NULL)
			{
			right->hide();
			lPos=right->getPosition();
			}
		else lPos=screenWidth-PBG_WIDTH/2;
		right=nameHash[name];
		right->mv(lPos);
		right->show();
		}
	else
		{
		if(left!=NULL) 
			{
			left->hide();
			lPos=left->getPosition();
			}
		else lPos=PBG_WIDTH/2;
		left=nameHash[name];
		left->mv(lPos);
		left->show();
		}
	}

void PanelController::timerEvent ( QTimerEvent * event )
	{
	if(event->timerId()!=timerId) return;
	if(leftAnimating)
		{
		if(left->getPosition()>=leftAnimationTarget)
			{
			left->mv(leftAnimationTarget);
			leftAnimating=false;
			endTimer();
			left->showAttached();
			}
		else if(leftAnimationTarget>left->getPosition()) left->mv(left->getPosition()+ANIMATION_STEP);
		else left->mv(left->getPosition()-ANIMATION_STEP);
		}
	if(rightAnimating)
		{
		if(right->getPosition()>=rightAnimationTarget)
			{
			right->mv(rightAnimationTarget);
			rightAnimating=false;
			endTimer();
			right->showAttached();
			}
		else if(rightAnimationTarget>right->getPosition()) right->mv(right->getPosition()+ANIMATION_STEP);
		else right->mv(right->getPosition()-ANIMATION_STEP);
		}
	}

void PanelController::registerPanel(Panel * panel,QString id,PanelControllerFlag f)
	{
	QString name=getName(id,f);
	nameHash[name]=panel;
	panelHash[panel]=name;
	bool r=getSide(name);
	QAction * a=new QAction(id,0);
	a->setData(name);
	if(r) 
		{
		panel->init(screenWidth-PBG_WIDTH/2,r);
		rightPanels.append(name);
		rightActions.append(a);
		}
	else 
		{
		panel->init(PBG_WIDTH/2,r);
		leftPanels.append(name);
		leftActions.append(a);
		}
	panel->unlock();
	if((f&LAYOUT_PANEL)!=BLANK)
		((LayoutPanel *)panel->attachedWidget())->setLayoutSide(r);
	if(r&&right==NULL)
		{
		right=panel;
		panel->show();
		}
	else if(r&&right!=NULL) panel->hide();
	else if(!r&&left==NULL)
		{
		left=panel;
		panel->show();
		}
	else if(!r&&left!=NULL) panel->hide();
	if(!panelNames.contains(id))
		panelNames.append(id);
	//Connects
	QObject::connect(panel,SIGNAL(released()),this,SLOT(release()));
	QObject::connect(panel,SIGNAL(timerClicked(Panel *)),this,SLOT(timerClicked(Panel *)));
	QObject::connect(panel,SIGNAL(rightClicked(Panel *,int,int)),this,SLOT(rightClicked(Panel *,int,int)));
	QObject::connect(panel,SIGNAL(handleDraggedTo(Panel *,int)),this,SLOT(handleDraggedTo(Panel *,int)));
	QObject::connect(panel,SIGNAL(scrolledUp(Panel *)),this,SLOT(scrolledUp(Panel *)));
	QObject::connect(panel,SIGNAL(scrolledDown(Panel *)),this,SLOT(scrolledDown(Panel *)));
	emit newPanelRegistered(id,r);
	}

void PanelController::demandPanel(QString id)
	{
	int l=left->getPosition();
	int r=right->getPosition();
	if(left->isLocked()) demandPanel(id,true);
	if(right->isLocked()) demandPanel(id,false);
	if(l==PBG_WIDTH/2&&r!=screenWidth-PBG_WIDTH/2)
		{demandPanel(id,false);
		return;}
	if(r==screenWidth-PBG_WIDTH/2&&l!=PBG_WIDTH/2)
		{demandPanel(id,true);
		return;}
	r=screenWidth-r;
	if(l>r)
		{demandPanel(id,true);
		return;}
	else
		{demandPanel(id,false);
		return;}
	}

void PanelController::demandPanel(QString id, bool scrapSide)
	{
	if(scrapSide&&right->isLocked()) return;
	if(!scrapSide&&left->isLocked()) return;
	changeToPanel(getName(id,scrapSide));
	int l=left->getPosition();
	int r=right->getPosition();
	//Ensures side is visible
	if(l==PBG_WIDTH/2&&!scrapSide)
		{
		timerClicked(left);
		}
	if(r==screenWidth-PBG_WIDTH/2&&scrapSide)
		{
		timerClicked(right);
		}
	}

QStringList PanelController::getPanelNames()
	{return panelNames;}

Panel * PanelController::getPanel(QString id, bool right_side)
	{return nameHash[getName(id,right_side)];}

void PanelController::release()
	{stuck=false;}

void PanelController::handleDraggedTo(Panel * p, int newPos)
	{
	if(p!=right&&p!=left) return;
	if((newPos<PBG_WIDTH/2))
		newPos=PBG_WIDTH/2-1;
	if((newPos>(screenWidth-PBG_WIDTH/2)))
		newPos=(screenWidth-PBG_WIDTH/2)-1;
	int l=left->getPosition();
	int r=right->getPosition();
	bool ir=(p==right);
	if(ir&&left->isLocked()&&l>newPos) {p->mv(l);return;}
	if(!ir&&right->isLocked()&&r<newPos) {p->mv(r);return;}
	if(ir&&l>newPos) stuck=true;
	if(!ir&&r<newPos) stuck=true;
	if(ir&&newPos==PBG_WIDTH/2-1) stuck=true;
	if(!ir&&newPos==(screenWidth-PBG_WIDTH/2)-1)stuck=true;
	p->mv(newPos);
	if(ir&&stuck)
		left->mv(newPos);
	if(!ir&&stuck)
		right->mv(newPos);
	}

void PanelController::timerClicked(Panel * p)
	{
	if(p!=right&&p!=left) return;
	int l=left->getPosition();
	bool le=(l>PBG_WIDTH/2);
	int r=right->getPosition();
	bool re=(r<screenWidth-PBG_WIDTH/2);
	bool ir=(p==right);
	if(ir&&!re&&(r==l))
		{
		leftAnimationTarget=screenWidth/2;
		leftAnimating=true;
		left->hideAttached();
		rightAnimationTarget=screenWidth/2;
		rightAnimating=true;
		right->hideAttached();
		startTimer();
		return;
		}
	if(!ir&&!le&&(r==l))
		{
		leftAnimationTarget=screenWidth/2;
		leftAnimating=true;
		left->hideAttached();
		rightAnimationTarget=screenWidth/2;
		rightAnimating=true;
		right->hideAttached();
		startTimer();
		return;
		}
	if(ir&&(r==l))
		{
		rightAnimationTarget=screenWidth-PBG_WIDTH/2;
		rightAnimating=true;
		right->hideAttached();
		startTimer();
		return;
		}
	if(!ir&&(r==l))
		{
		leftAnimationTarget=PBG_WIDTH/2;
		leftAnimating=true;
		left->hideAttached();
		startTimer();
		return;
		}
	if(ir)
		{
		rightAnimationTarget=l;
		rightAnimating=true;
		right->hideAttached();
		startTimer();
		return;
		}
	if(!ir)
		{
		leftAnimationTarget=r;
		leftAnimating=true;
		left->hideAttached();
		startTimer();
		return;
		}
	}

void PanelController::rightClicked(Panel * p, int x, int y)
	{
	QAction * a=QMenu::exec(findReplace(panelHash[p]),QPoint(x,y));
	if(a==0) return;
	QString name=a->data().toString();
	if(getId(name)=="lock"&&p->isLocked())
		p->unlock();
	else if(getId(name)=="lock"&&!p->isLocked())
		p->lock();
	else if(!p->isLocked()) changeToPanel(name);
	}

QList<QAction *> PanelController::findReplace(QString name)
	{
	QList<QAction *> out;
	QList<QAction *> tmp;
	if(getSide(name)) tmp=rightActions;
	else tmp=leftActions;
	for(int i=0; i<tmp.size(); i++)
		{
		if(tmp[i]->data().toString()==name)
			{
			QAction * t=new QAction("Toggle lock",0);
			if(getSide(name)) t->setData("r$%$lock");
			else t->setData("l$%$lock");
			if(nameHash[name]->isLocked()) t->setIcon(QIcon(":images/panel-tiny-locked.png"));
			else t->setIcon(QIcon(":images/panel-tiny-unlocked.png"));
			QFont f=t->font();
			f.setItalic(true);
			t->setFont(f);
			out.append(t);
			}
		else out.append(tmp[i]);
		}
	return out;
	}

void PanelController::scrolledUp(Panel * p)
	{
	QString name=panelHash[p];
	if(getSide(name))
		{
		int pos=rightPanels.indexOf(name);
		if(pos-1<0) pos=rightPanels.size()-1;
		else pos=pos-1;
		changeToPanel(rightPanels[pos]);
		}
	else
		{
		int pos=leftPanels.indexOf(name);
		if(pos-1<0) pos=leftPanels.size()-1;
		else pos=pos-1;
		changeToPanel(leftPanels[pos]);
		}
	}

void PanelController::scrolledDown(Panel * p)
	{
	QString name=panelHash[p];
	if(getSide(name))
		{
		int pos=rightPanels.indexOf(name);
		if(pos+1>=rightPanels.size()) pos=0;
		else pos=pos+1;
		changeToPanel(rightPanels[pos]);
		}
	else
		{
		int pos=leftPanels.indexOf(name);
		if(pos+1>=leftPanels.size()) pos=0;
		else pos=pos+1;
		changeToPanel(leftPanels[pos]);
		}
	}
#endif
