/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include	"MwMenuButtonP.h"
#include	"MwNws.h"

static XtResource resources [] = {
	{
	 XtNmenu_name ,
	 XtCMenu_name ,
	 XtRString ,
	 sizeof(String) ,
	 XtOffsetOf(MwMenuButtonRec,mwMenuButton.menu_name) ,
	 XtRImmediate ,
	 (XtPointer) "menu" ,
	},
};

static void MwPopupMenu(Widget, XEvent *, String *, Cardinal *);

static XtActionsRec action [] = {
	{"popup_menu",MwPopupMenu},
	};

static char trans_tab [] =
	"<Btn1Down>: hide_help() activate() popup_menu() \n\
	 <Enter>: enter_leave() highlight() show_help() \n\
	 <Leave>: enter_leave() unhighlight() hide_help() \n\
	 ~Shift<Key>Tab: traverseForward()\n\
	 Shift<Key>Tab: traverseBackward()\n\
	 <Key>Return: KBactivate()\n\
	 <Key>space: KBactivate()\n\
	 <FocusIn>: focusIn()\n\
	 <FocusOut>: focusOut()\n\
	 <BtnDown>: hide_help() \n\
	 <KeyDown>: hide_help()";

static void ClassInitialize(void);

MwMenuButtonClassRec mwMenuButtonClassRec = {
/* core */
   {
    /* superclass            */ (WidgetClass) &mwButtonClassRec,
    /* class_name            */ "MwMenuButton",
    /* widget_size           */ sizeof(MwMenuButtonRec),
    /* class_initialize      */ ClassInitialize,
    /* class_part_initialize */ NULL,
    /* class_inited          */ FALSE,
    /* initialize            */ NULL,
    /* initialize_hook       */ NULL,
    /* realize               */ XtInheritRealize,
    /* actions               */ action,
    /* num_actions           */ XtNumber(action),
    /* resources             */ resources,
    /* num_resources         */ XtNumber(resources),
    /* xrm_class             */ NULLQUARK,
    /* compress_motion       */ TRUE,
    /* compress_exposure     */ TRUE,
    /* compress_enterleave   */ TRUE,
    /* visible_interest      */ FALSE,
    /* destroy               */ NULL,
    /* resize                */ XtInheritResize,
    /* expose                */ XtInheritExpose,
    /* set_values            */ NULL,
    /* set_values_hook       */ NULL,
    /* set_values_almost     */ XtInheritSetValuesAlmost,
    /* get_values_hook       */ NULL,
    /* accept_focus          */ XtInheritAcceptFocus,
    /* version               */ XtVersion,
    /* callback_private      */ NULL,
    /* tm_table              */ trans_tab,
    /* query_geometry        */ XtInheritQueryGeometry,
    /* display_accelerator   */ XtInheritDisplayAccelerator,
    /* extension             */ NULL
   },
/* base */
   {
    /* get_internal_dimension  */ XtInheritGetInternalDimension,
    /* set_internal_dimension  */ XtInheritSetInternalDimension,
    /* highlight	       */ XtInheritHighlight,
    /* unhighlight	       */ XtInheritUnhighlight ,
    /* highlightBorder	       */ XtInheritHighlightBorder,
    /* unhighlightBorder       */ XtInheritUnhighlightBorder,
   },
/* sButton */
   {
    /* empty                 */ 0
   },
/* button */
   {
    /* empty 		     */ 0
   },
/* mwMenuButton */
   {
   /* empty		     */ 0
   },
};

WidgetClass mwMenuButtonWidgetClass = (WidgetClass) & mwMenuButtonClassRec;

static void ClassInitialize(void)
{
	_InitializeWidgetSet();
}

static void MwPopupMenu(Widget w , XEvent *event ,
		String *params , Cardinal *num_params)
{
	MwMenuButtonWidget cw = (MwMenuButtonWidget) w;
	Widget menu = NULL;
	Widget parent = w;
	int x, y;
	Window child;
	Display *dpy = XtDisplay(w);
	Dimension width , height ,
		dwidth = DisplayWidth(dpy,DefaultScreen(dpy)) ,
		dheight = DisplayHeight(dpy,DefaultScreen(dpy)) ;


	while (parent != NULL && menu == NULL)
	{
		if ((menu = XtNameToWidget(parent , cw->mwMenuButton.menu_name)) == NULL)
			parent = XtParent(parent);

	}

	if (menu == NULL) return;

	if (!XtIsRealized(menu))  XtRealizeWidget(menu);

	XtVaGetValues(menu , XtNwidth , &width , XtNheight , &height , NULL);

	XTranslateCoordinates(XtDisplay(cw),
		XtWindow(cw), DefaultRootWindow(XtDisplay(cw)),
		0, cw->core.height, &x, &y, &child);

	if ( (x + width) > dwidth ) x = dwidth - width;

	if ( (y + height ) > dheight ) y = y - cw->core.height - height;

	if ( y < 0 ) y = 0;

	XtVaSetValues(menu,XtNx,x,XtNy,y,NULL);

	XtPopupSpringLoaded(menu);

}
