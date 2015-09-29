/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#include <stdio.h>

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "MwUtils.h"

#include "MwCheckMEP.h"
#include "MwNws.h"

/* pixmapy */
#include        "pixmaps/mdoff.xpm"
#include        "pixmaps/mdon.xpm"
#include        "pixmaps/mroff.xpm"
#include        "pixmaps/mron.xpm"
#include        "pixmaps/mcoff.xpm"
#include        "pixmaps/mcon.xpm"
#include        "pixmaps/mchoff.xpm"
#include        "pixmaps/mchon.xpm"
#include        "pixmaps/mc2on.xpm"
#include        "pixmaps/mc2off.xpm"
#include        "pixmaps/mcron.xpm"
#include        "pixmaps/mcroff.xpm"


#define offset(field) XtOffsetOf(MwCheckMERec,checkME.field)

static XtResource resources [] = {
	{
	 XtNset ,
	 XtCSet ,
	 XtRCallback ,
	 sizeof(XtCallbackList) ,
	 offset(set) ,
	 XtRCallback ,
	 (XtPointer) NULL
	},
	{
	 XtNswitch ,
	 XtCSwitch ,
	 XtRCallback ,
	 sizeof(XtCallbackList) ,
	 offset(switchcb) ,
	 XtRCallback ,
	 (XtPointer) NULL
	},
	{
	 XtNreset ,
	 XtCReset ,
	 XtRCallback ,
	 sizeof(XtCallbackList) ,
	 offset(reset) ,
	 XtRCallback ,
	 (XtPointer) NULL
	},
	{
	 XtNon , 
	 XtCOn ,
	 XtRBool ,
	 sizeof(Boolean) ,
	 offset(on) ,
	 XtRString ,
	 (XtPointer) False
	},
	{
	 XtNtype ,
	 XtCType ,
	 XtRCheck_type ,
	 sizeof(int) ,
	 offset(type) ,
	 XtRImmediate ,
	 (XtPointer) XtCdiamond
	},
};

static void Destroy(Widget);
static void ClassInitialize (void);
static void Initialize(Widget, Widget, ArgList, Cardinal *);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);

static void Activate(Widget);

MwCheckMEClassRec mwCheckMEClassRec = {
/* core */
   {
    /* superclass            */ (WidgetClass) &mwLabelMEClassRec,
    /* class_name            */ "MwCheckME",
    /* widget_size           */ sizeof(MwCheckMERec),
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
    /* set_values            */ (XtSetValuesFunc) SetValues,
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
    /* leavenotify	       */ XtInheritLeavenotify,
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

WidgetClass mwCheckMEObjectClass = (WidgetClass) &mwCheckMEClassRec;

static void ClassInitialize(void)
{
    _InitializeWidgetSet();

    XtSetTypeConverter(XtRString, XtRCheck_type, cvtStringToCheckType,
                       NULL, 0, XtCacheNone, NULL);
}

static Icon * get_icon(MwCheckMEObject w, char **icon_data)
{
	XColor top , bottom , bc;
	Icon *iconfin = MwMalloc(sizeof(Icon));
	Display *dpy = XtDisplayOfObject((Widget) w);
	static XpmColorSymbol	tbl[4];
	XpmAttributes		attributes;

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
	tbl[2].pixel=w->baseME.foreground;
	tbl[3].name="background";
	tbl[3].value=NULL;
	tbl[3].pixel=w->baseME.background;
	

	iconfin->shape=None;
	attributes.valuemask = XpmReturnInfos | XpmColorSymbols;
	attributes.colorsymbols = tbl;
	attributes.numsymbols = 4;


	XpmCreatePixmapFromData (dpy , DefaultRootWindow(dpy) , icon_data , 
				&iconfin->pixmap , &iconfin->shape , &attributes);

	iconfin->width = attributes.width;
	iconfin->height = attributes.height;

	return iconfin;
}

static void Initialize(Widget req_widget, Widget new_widget,
		ArgList args, Cardinal *num_args)
{
	MwCheckMEObject nw = (MwCheckMEObject) new_widget;
	char ** id_on , ** id_off;
	
	switch (nw->checkME.type)
	{
		case XtCrectangle:
			id_on = mron;
			id_off = mroff;
			break;
		case XtCcircle:
			id_on = mcon;
			id_off = mcoff;
			break;
		case XtCcheck:
			id_on = mchon;
			id_off = mchoff;
			break;
		case XtCcircle2:
			id_on = mc2on;
			id_off = mc2off;
			break;
		case XtCcross:
			id_on = mcron;
			id_off = mcroff;
			break;
		case XtCdiamond:
		default :
			id_on = mdon;
			id_off = mdoff;
	}	

	nw->checkME.i_on=get_icon(nw,id_on);
	nw->checkME.i_off=get_icon(nw,id_off);

	if (nw->checkME.on)
	{
		nw->labelME.left_icon = nw->checkME.i_on;
	}
	else 
	{
		nw->labelME.left_icon =  nw->checkME.i_off;
	}

	mwLabelMEClassRec.rect_class.initialize(req_widget,new_widget,args,num_args);
}

static void Destroy (Widget w)
{
	MwCheckMEObject cw = (MwCheckMEObject) w;
	Display *dpy = XtDisplayOfObject(w);

	XFreePixmap(dpy , cw->checkME.i_on->pixmap);
	XFreePixmap(dpy , cw->checkME.i_on->shape);
	XFreePixmap(dpy , cw->checkME.i_off->pixmap);
	XFreePixmap(dpy , cw->checkME.i_off->shape);

}

#define WidgetValuesDiffer(w1,w2,component) (w1 -> checkME.component != \
                                             w2 -> checkME.component)

static Boolean SetValues(Widget current, Widget request, Widget new_widget,
		ArgList args, Cardinal *num_args)
{
	MwCheckMEObject cw = (MwCheckMEObject) current;
	MwCheckMEObject nw = (MwCheckMEObject) new_widget;
	Display *dpy = XtDisplayOfObject((Widget)nw);
	Boolean redraw = False;
	char ** id_on , ** id_off;

	if WidgetValuesDiffer( nw , cw , type)
	{
		switch (nw->checkME.type)
		{
			case XtCrectangle:
				id_on = mron;
				id_off = mroff;
				break;
			case XtCcircle:
				id_on = mcon;
				id_off = mcoff;
				break;
			case XtCcheck:
				id_on = mchon;
				id_off = mchoff;
				break;
			case XtCcircle2:
				id_on = mc2on;
				id_off = mc2off;
				break;
			case XtCcross:
				id_on = mcron;
				id_off = mcroff;
				break;
			case XtCdiamond:
			default :
				id_on = mdoff;
				id_off = mdon;
		}	
		nw->checkME.i_on=get_icon(nw,id_on);
		nw->checkME.i_off=get_icon(nw,id_off);
		XFreePixmap(dpy , cw->checkME.i_on->pixmap);
		XFreePixmap(dpy , cw->checkME.i_on->shape);
		XFreePixmap(dpy , cw->checkME.i_off->pixmap);
		XFreePixmap(dpy , cw->checkME.i_off->shape);
		MwFree((void*)cw->checkME.i_on);
		MwFree((void*)cw->checkME.i_off);

		if (nw->checkME.on)
		{
			nw->labelME.left_icon = nw->checkME.i_on;
		}
		else 
		{
			nw->labelME.left_icon =  nw->checkME.i_off;
		}

		redraw = True;
	}

	if WidgetValuesDiffer( nw , cw , on)
	{
		if (nw->checkME.on)
		{
			nw->labelME.left_icon = nw->checkME.i_on;
		}
		else 
		{
			nw->labelME.left_icon =  nw->checkME.i_off;
		}
	}

	return redraw;
}

static void Activate (Widget w)
{
	MwCheckMEObject cw = (MwCheckMEObject) w;

	mwLabelMEClassRec.baseME_class.activate(w);

	cw->checkME.on = !cw->checkME.on;
	if (cw->checkME.on)
	{
		XtVaSetValues(w , XtNleft_icon , cw->checkME.i_on , NULL);
		XtCallCallbackList(w,cw->checkME.set, NULL);
	}
	else 
	{
		XtVaSetValues(w , XtNleft_icon , cw->checkME.i_off , NULL);
		XtCallCallbackList(w,cw->checkME.reset, NULL);
	}

	XtCallCallbackList(w,cw->checkME.switchcb, NULL);	
}
