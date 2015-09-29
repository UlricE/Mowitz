/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "MwUtils.h"

#include	"MwMenuP.h"
#include	"MwNws.h"
#define HIDE_PROTO 1
#include	"MwSubMEP.h"
#undef HIDE_PROTO

/* pixmapy */
#include	"pixmaps/trion.xpm"
#include	"pixmaps/trioff.xpm"


#define offset(field) XtOffsetOf(MwSubMERec,subME.field)

static XtResource resources [] = {
	{
	 XtNmenu_name ,
	 XtCMenu_name ,
	 XtRString ,
	 sizeof(String) ,
	 offset(menu_name) ,
	 XtRImmediate ,
	 (XtPointer) "menu"
	},
	{
	 XtNdelay_to_show_submenu ,
	 XtCDelay_to_show_submenu ,
	 XtRInt ,
	 sizeof(int) ,
	 offset(delay_to_show_submenu) ,
	 XtRImmediate ,
	 (XtPointer) 350
	},
};

static void Destroy(Widget);
static void ClassInitialize(void);
static void Initialize(Widget, Widget, ArgList, Cardinal *);

static void Activate(Widget);
static void Enter(Widget);
static void Leave(Widget);

static void popup_menu(XtPointer, XtIntervalId *);
static void popdown_menu(Widget, XtPointer, XtPointer);

MwSubMEClassRec mwSubMEClassRec = {
/* core */
   {
    /* superclass            */ (WidgetClass) &mwLabelMEClassRec,
    /* class_name            */ "MwSubME",
    /* widget_size           */ sizeof(MwSubMERec),
    /* class_initialize      */ ClassInitialize,
    /* class_part_initialize */ NULL,
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
    /* destroy               */ Destroy,
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
    /* enternotify	       */ Enter,
    /* leavenotify	       */ Leave,
    /* activate		       */ Activate,
   },
/* labelME */
   {
   /* empty		       */ 0,
   },
/* checkME */
   {
   /* empty		       */ 0,
   }
};

WidgetClass mwSubMEObjectClass = (WidgetClass) &mwSubMEClassRec;

static void ClassInitialize(void)
{
	_InitializeWidgetSet();
}

static Icon * get_icon(MwSubMEObject w, char **icon_data)
{
	XColor top , bottom , bc;
	Icon *iconfin = MwMalloc(sizeof(Icon));
	Display *dpy = XtDisplayOfObject((Widget) w);
	static XpmColorSymbol	tbl[4];
	XpmAttributes		attributes;
	int res;

	bc.pixel=w->baseME.background;

	XQueryColor(dpy,DefaultColormap(dpy,DefaultScreen(dpy)),&bc);

	LightColor2(XtParent((Widget)w), top, w->baseME.top_shadow_contrast);
	DarkColor2(XtParent((Widget)w), bottom, w->baseME.bot_shadow_contrast);

	tbl[0].name="topShadowColor";
	tbl[0].value=NULL;
	tbl[0].pixel=top.pixel;
	tbl[1].name="bottomShadowColor";
	tbl[1].value=NULL;
	tbl[1].pixel=bottom.pixel;
	tbl[2].name="foreground";
	tbl[2].value=NULL;
	tbl[2].pixel=w->baseME.activ_fg;
	tbl[3].name="background";
	tbl[3].value=NULL;
	tbl[3].pixel=w->baseME.background;
	

	iconfin->shape=None;
	attributes.valuemask = XpmReturnInfos | XpmColorSymbols;
	attributes.colorsymbols = tbl;
	attributes.numsymbols = 4;


	res = XpmCreatePixmapFromData (dpy , DefaultRootWindow(dpy) , icon_data , 
				&iconfin->pixmap , &iconfin->shape , &attributes);

	iconfin->width = attributes.width;
	iconfin->height = attributes.height;

	return iconfin;
}

static void Initialize(Widget req_widget, Widget new_widget,
		ArgList args, Cardinal *num_args)
{
	MwSubMEObject nw = (MwSubMEObject) new_widget;

	nw->subME.is_menu_up = False;
	nw->subME.i_on=get_icon(nw , trion);
	nw->subME.i_off=get_icon(nw , trioff);

	XtVaSetValues(new_widget , XtNright_icon , nw->subME.i_off , NULL);

	nw->subME.is_menu_up = False;

	mwLabelMEClassRec.rect_class.initialize(req_widget,new_widget,args,num_args);
}

static void Destroy (Widget w)
{
	MwSubMEObject cw = (MwSubMEObject) w;
	Display *dpy = XtDisplayOfObject(w);

	XFreePixmap(dpy , cw->subME.i_on->pixmap);
	XFreePixmap(dpy , cw->subME.i_on->shape);
	XFreePixmap(dpy , cw->subME.i_off->pixmap);
	XFreePixmap(dpy , cw->subME.i_off->shape);

}

static void Activate (Widget w)
{
	MwSubMEObject cw = (MwSubMEObject) w;

	XtRemoveTimeOut(cw->subME.timer);

	((MwMenuWidgetClass)((MwMenuWidget)XtParent(w))->core.widget_class)
		->menu_class.popdown_all(XtParent(w));
}

static void Enter (Widget w)
{
	MwSubMEObject cw = (MwSubMEObject) w;

	cw->subME.timer = XtAppAddTimeOut(XtWidgetToApplicationContext(w),
		cw->subME.delay_to_show_submenu , popup_menu , w);

	XtVaSetValues(w , XtNright_icon , cw->subME.i_on , NULL);

	mwLabelMEClassRec.baseME_class.enternotify(w);
}

static void Leave (Widget w)
{
	MwSubMEObject cw = (MwSubMEObject) w;

	XtRemoveTimeOut(cw->subME.timer);

	if (!cw->subME.is_menu_up)
	{
		XtVaSetValues(w , XtNright_icon , cw->subME.i_off,NULL);

		mwLabelMEClassRec.baseME_class.leavenotify(w);
	}
}

static void popup_menu(XtPointer client_data , XtIntervalId *timer)
{
        MwSubMEObject cw = (MwSubMEObject) client_data;
	Display *dpy = XtDisplayOfObject((Widget)cw);
        Widget menu = NULL;
        Widget parent = (Widget) cw;
	Position x,y;
	Dimension width , height ,
		dwidth = DisplayWidth(dpy,DefaultScreen(dpy)) ,
		dheight = DisplayHeight(dpy,DefaultScreen(dpy)) ;

	while (parent != NULL && menu == NULL)
	{
		if ((menu = XtNameToWidget(parent , cw->subME.menu_name)) == NULL)
			parent = XtParent(parent);

	}

	if (menu == NULL) return;


	XtAddCallback(menu , XtNpopdownCallback , popdown_menu , (XtPointer)cw);
	
	if (!XtIsRealized(menu))  XtRealizeWidget(menu);

	XtVaGetValues(menu , XtNwidth , &width , XtNheight , &height , NULL);

	XtTranslateCoords(XtParent((Widget)cw) , cw->rectangle.width ,
		cw->rectangle.y , &x ,&y);

	if ( (x + width) > dwidth ) x = dwidth - width;

	if ( (y + height ) > dheight ) y = dheight - height;

	if ( y < 0 ) y = 0;

	XtVaSetValues(menu,XtNx,x,XtNy,y,NULL);

	XtPopup(menu,XtGrabNonexclusive);

	cw->subME.is_menu_up = True;

}

static void popdown_menu(Widget w, XtPointer client_data, XtPointer call_data)
{
	MwSubMEObject cw = (MwSubMEObject) client_data;

	XtRemoveCallback(w , XtNpopdownCallback , popdown_menu , client_data);

	cw->subME.is_menu_up = False;

	((MwSubMEObjectClass)cw->object.widget_class)->baseME_class.leavenotify((Widget)cw);
}

