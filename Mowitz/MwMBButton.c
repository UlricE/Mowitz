/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include "MwMenuBarP.h"
#include "MwNws.h"
#define HIDE_PROTO 1
#include "MwMenuP.h"
#include "MwMBButtonP.h"
#undef HIDE_PROTO

#define offset(field) XtOffsetOf(MwMBButtonRec,mBButton.field)

static XtResource resources [] = {
	{
	 XtNmenu_name ,
	 XtCMenu_name ,
	 XtRString ,
	 sizeof(String) ,
	 offset(menu_name) ,
	 XtRImmediate ,
	 (XtPointer) "menu" ,
	},
};

static void ClassInitialize(void);
static void Initialize (Widget, Widget, ArgList, Cardinal *);
static void PopdownMenu (Widget);
static void Activate (Widget);
static void Leavenotify (Widget);
static void ResolveInheritance (WidgetClass);

static Widget get_menu(Widget);
static void popdown_menu(Widget, XtPointer, XtPointer);

MwMBButtonClassRec mwMBButtonClassRec = {
/* core */
   {
    /* superclass            */ (WidgetClass) &mwLabelMEClassRec,
    /* class_name            */ "MwMBButton",
    /* widget_size           */ sizeof(MwMBButtonRec),
    /* class_initialize      */ ClassInitialize,
    /* class_part_initialize */ ResolveInheritance,
    /* class_inited          */ FALSE,
    /* initialize            */ (XtInitProc) Initialize,
    /* initialize_hook       */ NULL,
    /* rect1                 */ NULL,
    /* rect2                 */ NULL,
    /* rect3                 */ 0,
    /* resources             */ resources,
    /* num_resources         */ XtNumber(resources),
    /* xrm_class             */ NULLQUARK,
    /* rect4                 */ False,
    /* rect5                 */ False,
    /* rect6                 */ False,
    /* rect7                 */ False,
    /* destroy               */ NULL,
    /* resize                */ XtInheritResize,
    /* expose                */ XtInheritExpose,
    /* set_values            */ NULL,
    /* set_values_hook       */ NULL,
    /* set_values_almost     */ XtInheritSetValuesAlmost,
    /* get_values_hook       */ NULL,
    /* rect9                 */ NULL,
    /* version               */ XtVersion,
    /* callback_private      */ NULL,
    /* rect10                */ NULL,
    /* query_geometry        */ XtInheritQueryGeometry,
    /* rect11                */ NULL,
    /* extension             */ NULL
   },
/* baseME */
   {
    /* get_internal_dimension  */ XtInheritGetInternalDimension,
    /* set_internal_dimension  */ XtInheritSetInternalDimension,
    /* enternotify	       */ XtInheritEnternotify,
    /* leavenotify	       */ Leavenotify,
    /* activate		       */ Activate,
   },
/* labelME */
   {
   /* empty		       */ 0,
   },
/* mBButton */
   {
   /* popdown_menu	       */ PopdownMenu ,
   },
};

WidgetClass mwMBButtonObjectClass = (WidgetClass) &mwMBButtonClassRec;

static void ClassInitialize(void)
{
	_InitializeWidgetSet();
}

static void ResolveInheritance(WidgetClass class)
{
        MwMBButtonObjectClass c = (MwMBButtonObjectClass) class;
        MwMBButtonObjectClass super;

        if (class == mwMBButtonObjectClass) return;
        super = (MwMBButtonObjectClass)c->rect_class.superclass;

        if (c->mBButton_class.popdown_menu == XtInheritPopdownMenu)
                c->mBButton_class.popdown_menu =
                        super->mBButton_class.popdown_menu;

}

static void Initialize(Widget req_widget, Widget new_widget,
		ArgList args, Cardinal *num_args)
{
	MwMBButtonObject nw = (MwMBButtonObject) new_widget;

	nw->mBButton.menu = NULL;
}

static void Activate (Widget w)
{
	MwMBButtonObject cw = (MwMBButtonObject) w;
	Display * dpy= XtDisplayOfObject(w);
	int x, y;
	Window child;
	Dimension width , height ,
		dwidth = DisplayWidth(dpy,DefaultScreen(dpy)) ,
		dheight = DisplayHeight(dpy,DefaultScreen(dpy)) ;


        if (!XtIsSensitive(w)) return;
        
	XtCallCallbackList(w , cw->baseME.activate , NULL);

	cw->mBButton.menu = get_menu(w);

	cw->baseME.entered = True;

	((MwMBButtonObjectClass)cw->object.widget_class)->rect_class.expose(w , NULL , NULL);

	if (!cw->mBButton.menu) return;

	XtAddCallback(cw->mBButton.menu , XtNpopdownCallback ,
		popdown_menu , (XtPointer) cw);

	if (!XtIsRealized(cw->mBButton.menu))  XtRealizeWidget(cw->mBButton.menu);

	XtVaGetValues(cw->mBButton.menu , XtNwidth , &width ,
		XtNheight , &height , NULL);

	XTranslateCoordinates(XtDisplay(XtParent((Widget)cw)),
		XtWindow(XtParent((Widget)cw)),
		DefaultRootWindow(XtDisplay(XtParent((Widget)cw))),
		cw->rectangle.x, cw->rectangle.y + cw->rectangle.height,
		&x, &y, &child);

	if ( (x + width) > dwidth ) x = dwidth - width;

	if ( (y + height ) > dheight ) y = dheight - height;

	if ( y < 0 ) y = 0;

	XtVaSetValues(cw->mBButton.menu,XtNx,x,XtNy,y,NULL);

        XtPopupSpringLoaded(cw->mBButton.menu);
}


static void Leavenotify(Widget w)
{
	MwMBButtonObject cw = (MwMBButtonObject) w;


	if (!cw->mBButton.menu)
	{
		mwLabelMEClassRec.baseME_class.leavenotify(w);
	}
}

static void PopdownMenu(Widget w)
{
	MwMBButtonObject cw = (MwMBButtonObject) w;

	if (cw->mBButton.menu)
	{
		((MwMenuWidgetClass)((MwMenuWidget)cw->mBButton.menu)->core.widget_class)->
			menu_class.popdown_all(cw->mBButton.menu);
	}

	cw->baseME.entered = False;

	((MwMBButtonObjectClass)cw->object.widget_class)
		->rect_class.expose(w , NULL , NULL);

	cw->mBButton.menu = NULL;
}

static void popdown_menu(Widget w, XtPointer client_data, XtPointer call_data)
{
	MwMBButtonObject cw = (MwMBButtonObject) client_data;
	MwMenuBarWidget mw = (MwMenuBarWidget) XtParent((Widget)cw);

	XtRemoveCallback(w , XtNpopdownCallback , popdown_menu , client_data);

	cw->baseME.entered = False;

	((MwMBButtonObjectClass)cw->object.widget_class)
		->rect_class.expose((Widget)cw , NULL , NULL);

	cw->mBButton.menu = NULL;

	((MwMenuBarWidgetClass)mw->core.widget_class)->menuBar_class.unactivate((Widget)mw);

}

static Widget get_menu(Widget w)
{
	MwMBButtonObject cw = (MwMBButtonObject) w;
        Widget menu = NULL;
        Widget parent = w;

        while (parent != NULL && menu == NULL)
        {
                if ((menu = XtNameToWidget(parent , cw->mBButton.menu_name)) == NULL)
                        parent = XtParent(parent);
        }

	return menu;
}
