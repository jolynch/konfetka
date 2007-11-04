#ifndef LAYOUTPANEL
#define LAYOUTPANEL
class LayoutPanel
	{
	public:
		virtual void setLayoutSide(bool right_side)=0; //true=right, false=left
		virtual ~LayoutPanel() {}
	};
#endif
