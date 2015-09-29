/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include "MwBaseMEP.h"
#include "MwNws.h"

#define offset(field) XtOffsetOf(MwBaseMERec, baseME.field)

static XtResource resources[] = {
	{
	 XtNwid,
	 XtCWid,
	 XtRInt,
	 sizeof(int),
	 offset(wid),
	 XtRImmediate,
	 (XtPointer) 0
	},
	{
	 XtNcallback ,
	 XtCCallback ,
	 XtRCallback ,
	 sizeof(XtCallbackList) ,
	 offset(activate) ,
	 XtRCallback ,
	 (XtPointer) NULL
	},
	{
	 XtNforeground,
	 XtCForeground,
	 XtRPixel,
	 sizeof(Pixel),
	 offset(foreground),
	 XtRString,
	 (XtPointer) XtDefaultForeground
	},
	{
	 XtNbackground,
	 XtCBackground,
	 XtRPixel,
	 sizeof(Pixel),
	 offset(background),
	 XtRString,
	 (XtPointer) XtDefaultBackground
	},
	{
	 XtNbox_type,
	 XtCBox_type,
	 XtRBox_type,
	 sizeof(int),
	 offset(box_type),
	 XtRImmediate,
	 (XtPointer) XtCup_box
	},
	{
	 XtNbox_width,
	 XtCBox_width,
	 XtRInt,
	 sizeof(int),
	 offset(box_width),
	 XtRImmediate,
	 (XtPointer) 2
	},
	{
	 XtNbox_intensity,
	 XtCBox_intensity,
	 XtRInt,
	 sizeof(int),
	 offset(box_intensity),
	 XtRImmediate,
	 (XtPointer) 10000
	},
	{
	 XtNbox_color,
	 XtCBox_color,
	 XtRPixel,
	 sizeof(Pixel),
	 offset(box_color),
	 XtRString,
	 (XtPointer) XtDefaultForeground
	},
	{
	 XtNactiv_fg,
	 XtCActiv_fg,
	 XtRPixel,
	 sizeof(Pixel),
	 offset(activ_fg),
	 XtRString,
	 (XtPointer) XtDefaultForeground
	},
	{
	 XtNactiv_bg,
	 XtCActiv_bg,
	 XtRPixel,
	 sizeof(Pixel),
	 offset(activ_bg),
	 XtRString,
	 (XtPointer) XtDefaultBackground
	},
	{
                XtNtopShadowContrast,
                XtCTopShadowContrast,
                XtRInt,
                sizeof(int),
                offset(top_shadow_contrast),
                XtRImmediate,
                (XtPointer)20
        },
        {
                XtNbottomShadowContrast,
                XtCBottomShadowContrast,
                XtRInt,
                sizeof(int),
                offset(bot_shadow_contrast),
                XtRImmediate,
                (XtPointer)40
        },
};

static void Activate (Widget);
static void Enternotify (Widget);
static void Leavenotify (Widget);

static void Initialize (Widget, Widget, ArgList, Cardinal *);
static Boolean SetValues (Widget, Widget, Widget, ArgList, Cardinal *);
static void Redisplay (Widget, XEvent *, Region);
static void Destroy (Widget);
static void ClassInitialize(void);
static void ResolveInheritance(WidgetClass);

static void GetInternalDimension(Widget,Position *,Position *,Dimension *,Dimension *);
static void SetInternalDimension(Widget,Dimension,Dimension);

MwBaseMEClassRec mwBaseMEClassRec = {
/* core */
   {
    /* superclass            */ (WidgetClass) &rectObjClassRec,
    /* class_name            */ "MwBaseME",
    /* widget_size           */ sizeof(MwBaseMERec),
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
    /* destroy               */ Destroy,
    /* resize                */ XtInheritResize,
    /* expose                */ Redisplay,
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
/* mwBaseME */
   {
    /* get_internal_dimension  */ GetInternalDimension,
    /* set_internal_dimension  */ SetInternalDimension,
    /* enternotify	       */ Enternotify,
    /* leavenotify	       */ Leavenotify,
    /* activate		       */ Activate,
   },
};

WidgetClass mwBaseMEObjectClass = (WidgetClass) &mwBaseMEClassRec;




static void ClassInitialize(void)
{
    _InitializeWidgetSet();

    XtSetTypeConverter(XtRString, XtRBox_type, cvtStringToBoxType,
                       NULL, 0, XtCacheNone, NULL);

    XtSetTypeConverter(XtRBox_type, XtRString, cvtBoxTypeToString,
                       NULL, 0, XtCacheNone, NULL);
}

static void ResolveInheritance(WidgetClass class)
{
	MwBaseMEObjectClass c = (MwBaseMEObjectClass) class;
	MwBaseMEObjectClass super;

	if (class == mwBaseMEObjectClass) return;
	super = (MwBaseMEObjectClass)c->rect_class.superclass;

	if (c->baseME_class.get_internal_dimension == XtInheritGetInternalDimension)
		c->baseME_class.get_internal_dimension =
			super->baseME_class.get_internal_dimension;

	if (c->baseME_class.set_internal_dimension == XtInheritSetInternalDimension)
		c->baseME_class.set_internal_dimension =
			super->baseME_class.set_internal_dimension;

	if (c->baseME_class.activate == XtInheritActivate)
		c->baseME_class.activate =
			super->baseME_class.activate;

	if (c->baseME_class.enternotify == XtInheritEnternotify)
		c->baseME_class.enternotify =
			super->baseME_class.enternotify;

	if (c->baseME_class.leavenotify == XtInheritLeavenotify)
		c->baseME_class.leavenotify =
			super->baseME_class.leavenotify;


}

static void Initialize(Widget req_widget, Widget new_widget,
		ArgList args, Cardinal *num_args)
{
	MwBaseMEObject nw = (MwBaseMEObject) new_widget;
	XColor	dark,light,bg;
	Display *dpy=XtDisplayOfObject(new_widget);
	XGCValues gc_res;
	XtGCMask  gc_mask;

	bg.pixel = nw->baseME.box_color;
	
	XQueryColor(dpy,DefaultColormap(dpy,DefaultScreen(dpy)),&bg);
	
	LightColor2(XtParent(new_widget), light, nw->baseME.top_shadow_contrast);
	DarkColor2(XtParent(new_widget), dark, nw->baseME.bot_shadow_contrast);

	nw->baseME.light = light.pixel;
	nw->baseME.dark = dark.pixel;

	gc_res.foreground = nw->baseME.foreground;
	gc_res.background = nw->baseME.background;

	gc_mask = GCForeground | GCBackground;

	nw->baseME.gc = XCreateGC(dpy,DefaultRootWindow(dpy),
		gc_mask, &gc_res);

	nw->baseME.entered = False;	
}

static void Destroy(Widget w)
{
	MwBaseMEObject cw = (MwBaseMEObject) w;

	XFreeGC(XtDisplayOfObject(w),cw->baseME.gc);
}

static void Redisplay(Widget w, XEvent *event, Region region)
{
	MwBaseMEObject cw = (MwBaseMEObject) w;
	Display *dpy = XtDisplayOfObject(w);
	Window win = XtWindowOfObject (w);

	if (!XtIsRealized(w)) return;

	if (!cw->baseME.entered)
	{
		XSetForeground(dpy , cw->baseME.gc , cw->baseME.background);
		XFillRectangle(dpy , win , cw->baseME.gc , cw->rectangle.x ,
			cw->rectangle.y , cw->rectangle.width , cw->rectangle.height );
	}
	else
	{		
		XSetForeground(dpy , cw->baseME.gc , cw->baseME.background /* Ulric was here activ_bg*/);
		XFillRectangle(dpy , win , cw->baseME.gc , cw->rectangle.x ,
			cw->rectangle.y , cw->rectangle.width , cw->rectangle.height );

	
 	    switch (cw->baseME.box_type)
	    {
		case XtCno_box:
		case XtCshadow_box:
			break;

		case XtCsimple_box:
			X_DrawSimpleRawFrame(dpy , win , cw->rectangle.x , 
					cw->rectangle.y , 
					cw->rectangle.width , cw->rectangle.height , 
					cw->baseME.box_width , cw->baseME.box_color);
			break;

		case XtCup_box:
			X_DrawSoft3DFrame(dpy , win , cw->rectangle.x , 
				cw->rectangle.y , cw->rectangle.width , 
				cw->rectangle.height , cw->baseME.box_width ,
				cw->baseME.light , cw->baseME.dark,
				XtParent(w)->core.background_pixel);
			break;

		case XtCdown_box:
			X_DrawSimple3DFrame(dpy , win , cw->rectangle.x , 
				cw->rectangle.y , cw->rectangle.width , 
				cw->rectangle.height , cw->baseME.box_width ,
				cw->baseME.dark , cw->baseME.light);
			break;

		case XtCframein_box:
			X_DrawSimple3DFrame(dpy , win , cw->rectangle.x , 
				cw->rectangle.y ,
				cw->rectangle.width , cw->rectangle.height , 
				cw->baseME.box_width / 2 , cw->baseME.dark ,
				cw->baseME.light);
			X_DrawSimple3DFrame(dpy , win , cw->rectangle.x +
				cw->baseME.box_width / 2 , cw->rectangle.y +
				cw->baseME.box_width / 2 ,
				cw->rectangle.width - 2 * (cw->baseME.box_width / 2),
				cw->rectangle.height - 2 * (cw->baseME.box_width / 2),
				cw->baseME.box_width / 2 ,
				cw->baseME.light , cw->baseME.dark);
			break;

		case XtCframeout_box:
			X_DrawSimple3DFrame(dpy , win , cw->rectangle.x , 
				cw->rectangle.y ,
				cw->rectangle.width , cw->rectangle.height , 
				cw->baseME.box_width / 2 , cw->baseME.light , 
				cw->baseME.dark);
			X_DrawSimple3DFrame(dpy , win ,cw->baseME.box_width / 2 +
				cw->rectangle.x , cw->rectangle.y +
				cw->baseME.box_width / 2 , 
				cw->rectangle.width - 2 * (cw->baseME.box_width / 2),
				cw->rectangle.height - 2 * (cw->baseME.box_width / 2), 
				cw->baseME.box_width / 2 ,
				cw->baseME.dark , cw->baseME.light);
			break;
  	    }
  	}
	
	if (!XtIsSensitive(w)) Xt_SetInsensitive(w);
}

#define WidgetValuesDiffer(w1,w2,component) (w1 -> baseME.component != \
                                             w2 -> baseME.component)

static Boolean SetValues(Widget current, Widget request, Widget new_widget,
		ArgList args, Cardinal *num_args)
{
	MwBaseMEObject cw = (MwBaseMEObject) current;
	MwBaseMEObject nw = (MwBaseMEObject) new_widget;
	Boolean redraw=False;

	if (WidgetValuesDiffer(cw , nw , foreground) ||
		WidgetValuesDiffer(cw , nw , background) ||
		WidgetValuesDiffer(cw , nw , box_type))
	{
		redraw = True;
	}

        if ((nw->rectangle.sensitive != cw->rectangle.sensitive) || 
                (nw->rectangle.ancestor_sensitive != cw->rectangle.ancestor_sensitive))
        {
        	cw->baseME.entered = False;
                redraw = True;
        }

	return True;
}

static void GetInternalDimension(Widget w, Position *x, Position *y,
		Dimension *width, Dimension *height)
{
	MwBaseMEObject cw = (MwBaseMEObject) w;
	
	switch (cw->baseME.box_type)
	{
		case XtCshadow_box:
		case XtCno_box:
			*x = 0;
			*y = 0;
			*width = cw->rectangle.width;
			*height = cw->rectangle.height;
			break;

		case XtCsimple_box:
		case XtCup_box:
		case XtCdown_box:
			*x = cw->baseME.box_width;
			*y = cw->baseME.box_width;
			*width = cw->rectangle.width - 2 * cw->baseME.box_width;
			*height = cw->rectangle.height - 2 * cw->baseME.box_width;
			break;

		case XtCframein_box:
		case XtCframeout_box:
			*x = 2 * (cw->baseME.box_width / 2);
			*y = 2 * (cw->baseME.box_width / 2);
			*width = cw->rectangle.width - 4 * (cw->baseME.box_width /2);
			*height = cw->rectangle.height - 4 * (cw->baseME.box_width /2);
			break;
	}
}

static void SetInternalDimension(Widget w, Dimension width, Dimension height)
{
	MwBaseMEObject cw = (MwBaseMEObject) w;

	switch (cw->baseME.box_type)
	{
		case XtCshadow_box:
		case XtCno_box:
			cw->rectangle.width=width;
			cw->rectangle.height=height;
			break;
			
		case XtCsimple_box:
		case XtCup_box:
		case XtCdown_box:
			cw->rectangle.width = width + 2 * cw->baseME.box_width;
			cw->rectangle.height = height + 2 * cw->baseME.box_width;
			break;
			
		case XtCframein_box:
		case XtCframeout_box:	
			cw->rectangle.width = width + 4 * (cw->baseME.box_width /2);
			cw->rectangle.height = height + 4 * (cw->baseME.box_width /2);
			break;
	}		
}

static void Activate(Widget w)
{
	if (XtIsSensitive(w))
		XtCallCallbackList(w , ((MwBaseMEObject)w)->baseME.activate , NULL);
}

static void Enternotify(Widget w)
{
	MwBaseMEObject cw = (MwBaseMEObject) w;
	MwBaseMEObjectClass class = (MwBaseMEObjectClass)
			((MwBaseMEObject)w)->object.widget_class;

	if (XtIsSensitive(w))
	{
		cw->baseME.entered = True;
		class->rect_class.expose(w , NULL , NULL);
	}
}

static void Leavenotify(Widget w)
{
	MwBaseMEObject cw = (MwBaseMEObject) w;
	MwBaseMEObjectClass class = (MwBaseMEObjectClass)
			((MwBaseMEObject)w)->object.widget_class;

	if (XtIsSensitive(w))
	{
		cw->baseME.entered = False;
		class->rect_class.expose(w , NULL , NULL);
	}
}
