#ifndef PANELCONTROLLER_CPP
#define PANELCONTROLLER_CPP
#include <iostream>
#include "panelcontroller.h"
PanelController::PanelController(int width_):QObject()
	{
	width=width_;
	x_left=0;
	x_right=width-PBG_WIDTH;
	left=NULL;
	right=NULL;
	glued=false;
	moveBack=false;
	}

void PanelController::registerPanel(Panel * p, PanelControllerOption options, PanelPtrList * incompat_list,bool preferred)
	{
	flags[p]=options;
	connect(p,SIGNAL(requestMove(bool,int,int,bool,Panel *)),this,SLOT(processMove(bool,int,int,bool,Panel *)));
	connect(this,SIGNAL(endTimers()),p,SLOT(stop()));
	connect(p,SIGNAL(released(Panel *)),this,SLOT(released(Panel *)));
	if((options&0x010000)!=BLANK)
		{
		p->setSide(preferred);
		}
	if((options&LEFTPANEL)!=BLANK)
		{
		p->setSide(false);
		if((options&LAYOUT_PANEL)!=BLANK) ((LayoutPanel *)p)->setLayoutSide(false);
		if(left==NULL) {left=p; left->show();}
		left_list.append(p);
		connect(p,SIGNAL(cycle(bool)),this,SLOT(leftCycle(bool)));
		}
	if((options&RIGHTPANEL)!=BLANK)
		{
		p->setSide(true);
		if((options&LAYOUT_PANEL)!=BLANK) ((LayoutPanel *)p)->setLayoutSide(true);
		if(right==NULL) {right=p; right->show();}
		right_list.append(p);
		connect(p,SIGNAL(cycle(bool)),this,SLOT(rightCycle(bool)));
		}
	if((options&0x010000)!=BLANK)
		{
		p->setSide(preferred);
		if(p->isActive())
			if(preferred)
				connect(p,SIGNAL(cycle(bool)),this,SLOT(rightCycle(bool)));
			else
				connect(p,SIGNAL(cycle(bool)),this,SLOT(leftCycle(bool)));
		else p->disconnect(SIGNAL(cycle(bool,int)));
		}
	if((options&INCOMPATIBLE_WITH)!=BLANK)
		incompatibleList[p]=*incompat_list;
	return;
	}

void PanelController::leftCycle(bool up)
	{
	if((flags[left]&0x010000)!=BLANK)
		{left->disconnect(SIGNAL(cycle(bool)));}
	int idx=left_list.indexOf(left);
	do{
		if(up)
			{idx++;
			if(idx==left_list.size()) idx=0;}
		else
			{idx--;
			if(idx==-1) idx=left_list.size()-1;}
	}while(incompatibleList[right].contains(left_list[idx]));
	left->hide();
	left=left_list[idx];
	left->setSide(false);
	if((flags[left]&LAYOUT_PANEL)!=BLANK) ((LayoutPanel *)left)->setLayoutSide(true);
	if((flags[left]&0x010000)!=BLANK)
		{connect(left,SIGNAL(cycle(bool)),this,SLOT(leftCycle(bool)));
		left->setSide_DOUBLESIDEPANEL(false);}
	left->show();
	left->allowXMove(x_left);
	}

void PanelController::rightCycle(bool up)
	{
	if((flags[right]&0x010000)!=BLANK)
		{right->disconnect(SIGNAL(cycle(bool)));}
	int idx=right_list.indexOf(right);
	do{
		if(up)
			{idx++;
			if(idx==right_list.size()) idx=0;}
		else
			{idx--;
			if(idx==-1) idx=right_list.size()-1;}
	}while(incompatibleList[left].contains(right_list[idx]));
	right->hide();
	right=right_list[idx];
	right->setSide(true);
	if((flags[right]&LAYOUT_PANEL)!=BLANK) ((LayoutPanel *)right)->setLayoutSide(true);
	if((flags[right]&0x010000)!=BLANK)
		{connect(right,SIGNAL(cycle(bool)),this,SLOT(rightCycle(bool)));
		right->setSide_DOUBLESIDEPANEL(true);}
	right->show();
	right->allowXMove(x_right);
	}

void PanelController::processMove(bool timer,int newX,int y,bool side_right,Panel * p)
	{
	if(timer) 
		{
		if(x_right==x_left)
			{moveBack=true;}
		if(moveBack)
			{
			if(side_right) 
				{newX=x_right+newX;
				if(newX>width-PBG_WIDTH) 
					{newX=width-PBG_WIDTH;
					emit(endTimers());
					moveBack=false;}}
			else 
				{newX=x_left-newX;
				if(newX<0) 
					{newX=0;
					emit(endTimers());
					moveBack=false;}}
			}
		else
			{
			if(side_right) 
				{newX=x_right-newX;
				if(newX<x_left) 
					{newX=x_left;
					emit(endTimers());}}
			else
				{newX=newX+x_left;
				if(newX>x_right) 
					{newX=x_right;
					emit(endTimers());}}
			}
		p->allowXMove(newX);
		if(side_right)x_right=newX;
		else x_left=newX;
		return;
		}
	//if(x_left==x_right&&(!left->isLocked())&&(!right->isLocked())) glued=true;
	if(glued)
		{
		if(newX<0) newX=0;
		if(newX>width-PBG_WIDTH) newX=width-PBG_WIDTH;
		left->allowXMove(newX);
		right->allowXMove(newX);
		x_right=newX; x_left=newX;
		return;
		}
	if(side_right)
		{
		if(newX>width-PBG_WIDTH)
			{
			p->allowXMove(width-PBG_WIDTH);
			x_right=width-PBG_WIDTH; return;
			}
		else if(newX<=x_left)
			{
			p->allowXMove(x_left);
			x_right=x_left;
			if(!left->isLocked()) glued=true;
			return;
			}
		else
			{p->allowXMove(newX);
			x_right=newX;}
		}
	else
		{
		if(newX<0)
			{
			p->allowXMove(0);
			x_right=0; return;
			}
		else if(newX>x_right)
			{
			p->allowXMove(x_right);
			x_left=x_right;
			if(!right->isLocked()) glued=true;
			return;
			}
		else
			{p->allowXMove(newX);
			x_left=newX;}
		}
	}

void PanelController::released(Panel *p)
	{glued=false;}
#endif
