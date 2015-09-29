/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/extensions/shape.h>
#include "MwUtils.h"


#include <Mowitz/MwPopText.h>
#include <Mowitz/MwBaseP.h>
#include <Mowitz/MwBaseConstP.h>
#include <Mowitz/MwBaseCompP.h>
#include <Mowitz/MwNws.h>
#include <Mowitz/MwTraverse.h>

#define offset(field) XtOffsetOf(MwBaseRec, base.field)

static XtResource resources[] = { 
	{
	 XtNuser_data,
	 XtCUser_data,
	 XtRPointer,
	 sizeof(void *),
	 offset(user_data),
	 XtRImmediate,
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
	 XtNwid,
	 XtCWid,
	 XtRInt,
	 sizeof(int),
	 offset(wid),
	 XtRImmediate,
	 (XtPointer) 0
	},
	{
	 XtNhelp_text,
	 XtCHelp_text,
	 XtRString,
	 sizeof(String),
	 offset(help_text),
	 XtRImmediate,
	 (XtPointer) NULL
	},
	{
	 XtNcursor,
	 XtCCursor,
	 XtRCursor,
	 sizeof(Cursor),
	 offset(cursor),
	 XtRString,
	 (XtPointer) None
	},
	{
	 XtNbox_type,
	 XtCBox_type,
	 XtRBox_type,
	 sizeof(int),
	 offset(box_type),
	 XtRImmediate,
	 (XtPointer) XtCsimple_box
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
	 XtNborderWidth,
	 XtCBorderWidth,
	 XtRDimension,
	 sizeof(Dimension),
	 XtOffsetOf(MwBaseRec , core.border_width),
	 XtRImmediate,
	 (XtPointer) 0
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
	 XtNhighlight_on_enter,
	 XtCHighlight_on_enter,
	 XtRBoolean,
	 sizeof(Boolean),
	 offset(highlight_on_enter),
	 XtRImmediate,
	 (XtPointer) False
	},
	{
	 XtNhelp_show_delay,
	 XtCHelp_show_delay,
	 XtRInt,
	 sizeof(int),
	 offset(help_show_delay),
	 XtRImmediate,
	 (XtPointer) 500
	},
	{
	 XtNbd_width ,
	 XtCBd_width ,
	 XtRInt ,
	 sizeof(int) ,
	 offset(bd_width) ,
	 XtRImmediate ,
	 (XtPointer) 1 ,
	},
	{
	 XtNbd_color ,
	 XtCBd_color ,
	 XtRPixel ,
	 sizeof(Pixel),
	 offset(bd_color),
	 XtRString,
	 (XtPointer) XtDefaultForeground
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

static void Initialize (Widget, Widget, ArgList, Cardinal *);
static Boolean SetValues (Widget, Widget, Widget, ArgList, Cardinal *);
static void Redisplay (Widget, XEvent *, Region);
static void Destroy (Widget);
static void Realize (Widget, Mask *, XSetWindowAttributes *);
static void ClassInitialize(void);
static void ResolveInheritance(WidgetClass);
static void Resize (Widget);

static void GetInternalDimension(Widget,Position *,Position *,Dimension *,Dimension *);
static void SetInternalDimension(Widget,Dimension,Dimension);

static void  Highlight(Widget, XEvent *, String *, Cardinal *);
static void  _Highlight(Widget, XEvent *, String *, Cardinal *);
static void  Unhighlight(Widget, XEvent *, String *, Cardinal *);
static void  _Unhighlight(Widget, XEvent *, String *, Cardinal *);
static void  ShowHelp(Widget, XEvent *, String *, Cardinal *);
static void  HideHelp(Widget, XEvent *, String *, Cardinal *);
static void  _ShowHelp(XtPointer, XtIntervalId *);
static void  _FocusIn(Widget, XEvent *, String *, Cardinal *);
static void  _FocusOut(Widget, XEvent *, String *, Cardinal *);
static void  FocusCurrent(Widget, XEvent *, String *, Cardinal *);
static void  TraverseForward(Widget, XEvent *, String *, Cardinal *);
static void  TraverseBackward(Widget, XEvent *, String *, Cardinal *);
static Boolean AcceptFocus(Widget, Time *);
static void  HighlightBorder(Widget);
static void  UnhighlightBorder(Widget);

static XtActionsRec action [] = {
        {"highlight",Highlight},
        {"unhighlight",Unhighlight},
        {"show_help",ShowHelp},
        {"hide_help",HideHelp},
        {"focusIn",_FocusIn},
        {"focusOut",_FocusOut},
	{"focusCurrent" , FocusCurrent},
        {"traverseForward",TraverseForward},
        {"traverseBackward",TraverseBackward},
        };

static char trans_tab [] = 
        "<Enter>: highlight() show_help()\n\
	 <Leave>: unhighlight() hide_help()\n\
	 ~Shift<Key>Tab: traverseForward()\n\
	 Shift<Key>Tab: traverseBackward()\n\
	 <BtnDown>: hide_help() focusCurrent()\n\
	 <KeyDown>: hide_help()\n\
	 <FocusIn>: focusIn()\n\
	 <FocusOut>: focusOut()\n\
	 ";

MwBaseClassRec mwBaseClassRec = {
/* core */
   {
    /* superclass            */ (WidgetClass) &widgetClassRec,
    /* class_name            */ "MwBase",
    /* widget_size           */ sizeof(MwBaseRec),
    /* class_initialize      */ ClassInitialize,
    /* class_part_initialize */ ResolveInheritance,
    /* class_inited          */ FALSE,
    /* initialize            */ (XtInitProc) Initialize,
    /* initialize_hook       */ NULL,
    /* realize               */ Realize,
    /* actions               */ action,
    /* num_actions           */ XtNumber(action),
    /* resources             */ resources,
    /* num_resources         */ XtNumber(resources),
    /* xrm_class             */ NULLQUARK,
    /* compress_motion       */ False,
    /* compress_exposure     */ False,
    /* compress_enterleave   */ False,
    /* visible_interest      */ FALSE,
    /* destroy               */ Destroy,
    /* resize                */ Resize,
    /* expose                */ Redisplay,
    /* set_values            */ (XtSetValuesFunc) SetValues,
    /* set_values_hook       */ NULL,
    /* set_values_almost     */ XtInheritSetValuesAlmost,
    /* get_values_hook       */ NULL,
    /* accept_focus          */ AcceptFocus,
    /* version               */ XtVersion,
    /* callback_private      */ NULL,
    /* tm_table              */ trans_tab,
    /* query_geometry        */ XtInheritQueryGeometry,
    /* display_accelerator   */ XtInheritDisplayAccelerator,
    /* extension             */ NULL
   },
/* base */
   {
    /* get_internal_dimension  */ GetInternalDimension,
    /* set_internal_dimension  */ SetInternalDimension,
    /* highlight	       */ _Highlight,
    /* unhighlight	       */ _Unhighlight,
    /* highlightBorder	       */ HighlightBorder,
    /* unhighlightBorder       */ UnhighlightBorder,
   },
};

WidgetClass mwBaseWidgetClass = (WidgetClass) &mwBaseClassRec;

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
	MwBaseWidgetClass c = (MwBaseWidgetClass) class;
	MwBaseWidgetClass super;

	if (class == mwBaseWidgetClass) return;
	super = (MwBaseWidgetClass)class->core_class.superclass;

	if (c->base_class.get_internal_dimension == XtInheritGetInternalDimension)
		c->base_class.get_internal_dimension =
			super->base_class.get_internal_dimension;

	if (c->base_class.set_internal_dimension == XtInheritSetInternalDimension)
		c->base_class.set_internal_dimension =
			super->base_class.set_internal_dimension;

	if (c->base_class.highlight == XtInheritHighlight)
		c->base_class.highlight =
			super->base_class.highlight;

	if (c->base_class.unhighlight == XtInheritUnhighlight)
		c->base_class.unhighlight =
			super->base_class.unhighlight;

	if (c->base_class.highlightBorder == XtInheritHighlightBorder)
		c->base_class.highlightBorder =
			super->base_class.highlightBorder;

	if (c->base_class.unhighlightBorder == XtInheritUnhighlightBorder)
		c->base_class.unhighlightBorder =
			super->base_class.unhighlightBorder;

}

static void Initialize(Widget req_widget, Widget new_widget,
		ArgList args, Cardinal *num_args)
{
	MwBaseWidget nw = (MwBaseWidget) new_widget;
	XColor	dark,light,bg;
	Display *dpy=XtDisplay(new_widget);
	XGCValues gc_res;
	XtGCMask  gc_mask;

	bg.pixel = nw->base.box_color;
	
	XQueryColor(dpy,DefaultColormap(dpy,DefaultScreen(dpy)),&bg);
	
	LightColor2(new_widget, light, nw->base.top_shadow_contrast);
	DarkColor2(new_widget, dark, nw->base.bot_shadow_contrast);

	nw->base.light = light.pixel;
	nw->base.dark = dark.pixel;

	gc_res.foreground = nw->base.box_color;

	gc_mask = GCForeground ;

	nw->base.gc = XCreateGC(dpy,DefaultRootWindow(dpy),
			gc_mask, &gc_res);

	if (nw->base.help_text)
	{
		nw->base.help_text = MwStrdup(nw->base.help_text);
		nw->base.hlp = XtVaCreatePopupShell("___help" , mwPopTextWidgetClass ,
					new_widget , XtNtext , nw->base.help_text,NULL);
	}

	if (nw->base.box_type == XtCshadow_box) 
	{
		nw->base.have_shape = True;
	}
	else 
	{
		nw->base.have_shape = False;
	}

	nw->base.timer = (XtIntervalId) 0;

	nw->base.entered = False;
	nw->base.focused = False;
}

static void Realize(Widget w, Mask *valueMask, XSetWindowAttributes *attributes)
{
	MwBaseWidget cw = (MwBaseWidget) w;

	if ((attributes->cursor = cw->base.cursor) != None)
		*valueMask |= CWCursor;

	coreClassRec.core_class.realize(w,valueMask,attributes);

	if (cw->base.box_type == XtCshadow_box)
	{
		Region region;
		XPoint points[8];

		points[0].x = 0;
		points[0].y = 0;
		points[1].x = cw->core.width - 2 * cw->base.box_width;
		points[1].y = 0;
		points[2].x = cw->core.width - 2 * cw->base.box_width;
		points[2].y = 2 * cw->base.box_width;
		points[3].x = cw->core.width;
		points[3].y = 2 * cw->base.box_width;
		points[4].x = cw->core.width;
		points[4].y = cw->core.height;
		points[5].x = 2 * cw->base.box_width;
		points[5].y = cw->core.height;
		points[6].x = 2 * cw->base.box_width;
		points[6].y = cw->core.height - 2 * cw->base.box_width;
		points[7].x = 0;
		points[7].y = cw->core.height - 2 * cw->base.box_width;

		region = XPolygonRegion(points , XtNumber(points) , EvenOddRule);

		XShapeCombineRegion(XtDisplay(w) , XtWindow(w) , ShapeBounding ,
			0 , 0 , region , ShapeSet);

		XDestroyRegion(region);
	}
}

static void Destroy(Widget w)
{
	MwBaseWidget cw = (MwBaseWidget) w;

	if (cw->base.help_text)
	{
		MwFree(cw->base.help_text);
	}

	XFreeGC(XtDisplay(w),cw->base.gc);
}

static void Redisplay(Widget w, XEvent *event, Region region)
{
	MwBaseWidget cw = (MwBaseWidget) w;
	Display *dpy = XtDisplay(w);
	Window win = XtWindow (w);
	
	switch (cw->base.box_type)
	{
		case XtCno_box:
			break;

		case XtCsimple_box:
			X_DrawSimpleRawFrame(dpy , win , cw->base.bd_width ,
				 	cw->base.bd_width ,
				 	cw->core.width  - 2 * cw->base.bd_width,
					cw->core.height - 2 * cw->base.bd_width,
					cw->base.box_width ,
					cw->base.box_color);
			break;

		case XtCup_box:
			X_DrawSoft3DFrame(dpy , win , cw->base.bd_width ,
				cw->base.bd_width ,
				cw->core.width - 2 * cw->base.bd_width,
				cw->core.height - 2 * cw->base.bd_width,
				cw->base.box_width ,
				cw->base.light , cw->base.dark,
				w->core.background_pixel);
			break;

		case XtCdown_box:
			X_DrawSoft3DFrame(dpy , win , cw->base.bd_width ,
				cw->base.bd_width , 
				cw->core.width - 2 * cw->base.bd_width,
				cw->core.height - 2 * cw->base.bd_width,
				-cw->base.box_width ,
				cw->base.light, cw->base.dark,
				w->core.background_pixel);
			break;

		case XtCframein_box:
			X_DrawSimple3DFrame(dpy , win , cw->base.bd_width ,
				cw->base.bd_width ,
				cw->core.width - 2 * cw->base.bd_width,
				cw->core.height - 2 * cw->base.bd_width,
				cw->base.box_width / 2 ,
				cw->base.dark , cw->base.light);
			X_DrawSimple3DFrame(dpy , win , 
				cw->base.box_width / 2 + cw->base.bd_width,
				cw->base.box_width / 2 + cw->base.bd_width,
				cw->core.width - 2 * (cw->base.box_width / 2) 
				- 2 * cw->base.bd_width,
				cw->core.height - 2 * (cw->base.box_width / 2) 
				- 2 * cw->base.bd_width,
				cw->base.box_width / 2 ,
				cw->base.light , cw->base.dark);
			break;

		case XtCframeout_box:
			X_DrawSimple3DFrame(dpy , win , cw->base.bd_width ,
				cw->base.bd_width ,
				cw->core.width - 2 * cw->base.bd_width,
				cw->core.height - 2 * cw->base.bd_width,
				cw->base.box_width / 2 ,
				cw->base.light , cw->base.dark);
			X_DrawSimple3DFrame(dpy , win ,
				cw->base.box_width / 2 + cw->base.bd_width ,
				cw->base.box_width / 2 + cw->base.bd_width, 
				cw->core.width - 2 * (cw->base.box_width / 2) 
				- 2 * cw->base.bd_width,
				cw->core.height - 2 * (cw->base.box_width / 2) 
				- 2 * cw->base.bd_width, 
				cw->base.box_width / 2 ,
				cw->base.dark , cw->base.light);
			break;

		case XtCshadow_box:
			{
			XPoint points[6];
			
			points[0].x = 2 * cw->base.box_width + cw->base.bd_width;
			points[0].y = cw->core.height - cw->base.bd_width;
			points[1].x = 2 * cw->base.box_width + cw->base.bd_width;
			points[1].y = cw->core.height - 2 * cw->base.box_width - cw->base.bd_width;
			points[2].x = cw->core.width - 2 * cw->base.box_width - cw->base.bd_width;
			points[2].y = cw->core.height - 2 * cw->base.box_width - cw->base.bd_width;
			points[3].x = cw->core.width - 2 * cw->base.box_width - cw->base.bd_width;
			points[3].y = 2 * cw->base.box_width + cw->base.bd_width;
			points[4].x = cw->core.width - cw->base.bd_width;
			points[4].y = 2 * cw->base.box_width + cw->base.bd_width;
			points[5].x = cw->core.width - cw->base.bd_width;
			points[5].y = cw->core.height - cw->base.bd_width;

			X_DrawSimpleRawFrame(dpy , win , cw->base.bd_width ,
				cw->base.bd_width ,
				cw->core.width - (2 * cw->base.box_width) 
				- 2 * cw->base.bd_width,
				cw->core.height - (2 * cw->base.box_width) 
				- 2 * cw->base.bd_width,
				cw->base.box_width , cw->base.box_color );

			XSetForeground(dpy , cw->base.gc , cw->base.dark);

			XFillPolygon(dpy , win , cw->base.gc , points , XtNumber(points),
				 Nonconvex , CoordModeOrigin);
			
			break;
		}
	}
	if (cw->base.focused) 
		((MwBaseWidgetClass)w->core.widget_class)->base_class.highlightBorder(w);

	if (!XtIsSensitive(w)) Xt_SetInsensitive(w);
}

static void Resize(Widget w)
{
	MwBaseWidget cw = (MwBaseWidget) w;

	if (!XtIsRealized(w)) return;

	if (cw->base.have_shape)
	{
		if (cw->base.box_type == XtCshadow_box)
		{
			Region region;
			XPoint points[8];
	
			points[0].x = 0;
			points[0].y = 0;
			points[1].x = cw->core.width - 2 * cw->base.box_width;
			points[1].y = 0;
			points[2].x = cw->core.width - 2 * cw->base.box_width ;
			points[2].y = 2 * cw->base.box_width;
			points[3].x = cw->core.width;
			points[3].y = 2 * cw->base.box_width;
			points[4].x = cw->core.width;
			points[4].y = cw->core.height;
			points[5].x = 2 * cw->base.box_width;
			points[5].y = cw->core.height;
			points[6].x = 2 * cw->base.box_width;
			points[6].y = cw->core.height - 2 * cw->base.box_width;
			points[7].x = 0;
			points[7].y = cw->core.height - 2 * cw->base.box_width;


			region = XPolygonRegion(points , XtNumber(points) , EvenOddRule);

			XShapeCombineRegion(XtDisplay(w) , XtWindow(w) , ShapeBounding ,
				0 , 0 , region , ShapeSet);

			XDestroyRegion(region);

		} else {
			Region region;
			XPoint points[4];

			points[0].x = 0;
			points[0].y = 0;
			points[1].x = cw->core.width;
			points[1].y = 0;
			points[2].x = cw->core.width;
			points[2].y = cw->core.height;
			points[3].x = 0;
			points[3].y = cw->core.height;

			region = XPolygonRegion(points , XtNumber(points) , EvenOddRule);

			XShapeCombineRegion(XtDisplay(w) , XtWindow(w) , ShapeBounding ,
				0 , 0 , region , ShapeSet);

			XDestroyRegion(region);			
		}
	}
}


#define WidgetValuesDiffer(w1,w2,component) (w1 -> base.component != \
                                             w2 -> base.component)

static Boolean SetValues(Widget current, Widget request, Widget new_widget,
		ArgList args, Cardinal *num_args)
{
	MwBaseWidget cw = (MwBaseWidget) current;
	MwBaseWidget nw = (MwBaseWidget) new_widget;
	Widget w = (Widget) new_widget;
	Boolean redraw=False;
	
	if WidgetValuesDiffer( cw , nw , cursor)
	{
		XDefineCursor(XtDisplay(current),XtWindow(current),nw->base.cursor);
	}

	if (WidgetValuesDiffer( cw , nw , box_type) ||
		WidgetValuesDiffer( cw , nw , box_width))
	{
		if (cw->base.box_type == XtCshadow_box && 
			nw->base.box_type != XtCshadow_box)
		{
			Region region;
			XPoint points[4];

			points[0].x = 0;
			points[0].y = 0;
			points[1].x = cw->core.width;
			points[1].y = 0;
			points[2].x = cw->core.width;
			points[2].y = cw->core.height;
			points[3].x = 0;
			points[3].y = cw->core.height;

			region = XPolygonRegion(points , XtNumber(points) , EvenOddRule);

			XShapeCombineRegion(XtDisplay(w) , XtWindow(w) , ShapeBounding ,
				0 , 0 , region , ShapeSet);

			XDestroyRegion(region);
				
		}
		if (nw->base.box_type == XtCshadow_box &&
			cw->base.box_type != XtCshadow_box)
		{
			Region region;
			XPoint points[8];

			points[0].x = 0;
			points[0].y = 0;
			points[1].x = cw->core.width - 2 * cw->base.box_width;
			points[1].y = 0;
			points[2].x = cw->core.width - 2 * cw->base.box_width;
			points[2].y = 2 * cw->base.box_width;
			points[3].x = cw->core.width;
			points[3].y = 2 * cw->base.box_width;
			points[4].x = cw->core.width;
			points[4].y = cw->core.height;
			points[5].x = 2 * cw->base.box_width;
			points[5].y = cw->core.height;
			points[6].x = 2 * cw->base.box_width;
			points[6].y = cw->core.height - 2 * cw->base.box_width;
			points[7].x = 0;
			points[7].y = cw->core.height - 2 * cw->base.box_width;

	
			region = XPolygonRegion(points , XtNumber(points) , EvenOddRule);

			XShapeCombineRegion(XtDisplay(w) , XtWindow(w) , ShapeBounding ,
				0 , 0 , region , ShapeSet);

			XDestroyRegion(region);

			nw->base.have_shape=True;

		}
		redraw = True;
	}

	if WidgetValuesDiffer( cw , nw , help_text )
	{
		if (cw->base.help_text)
		{
			XtDestroyWidget(cw->base.hlp);
			MwFree(cw->base.help_text);
			cw->base.help_text = NULL;
		}
		if (nw->base.help_text)  
		{
			nw->base.help_text = MwStrdup(nw->base.help_text);
			nw->base.hlp = XtVaCreatePopupShell("___help" , mwPopTextWidgetClass ,
					new_widget , XtNtext , nw->base.help_text,NULL);
		}
		
	}

	if ((nw->core.sensitive != cw->core.sensitive) || 
		(nw->core.ancestor_sensitive != cw->core.ancestor_sensitive))
	{
		nw->base.focused = False;
		_Unhighlight(new_widget, NULL, NULL, NULL);
		redraw = True;
	}

	return redraw;
}

static void GetInternalDimension(Widget w, Position *x, Position *y,
		Dimension *width, Dimension *height)
{
	MwBaseWidget cw = (MwBaseWidget) w;
	
	switch (cw->base.box_type)
	{
		case XtCno_box:
			*x = cw->base.bd_width;
			*y = cw->base.bd_width;
			*width = cw->core.width - 2 * cw->base.bd_width;
			*height = cw->core.height - 2 * cw->base.bd_width;
			break;

		case XtCsimple_box:
		case XtCup_box:
		case XtCdown_box:
			*x = cw->base.box_width + cw->base.bd_width;
			*y = cw->base.box_width + cw->base.bd_width;
			*width = cw->core.width - 2 * cw->base.box_width - 2 * cw->base.bd_width;
			*height = cw->core.height - 2 * cw->base.box_width - 2 * cw->base.bd_width;
			break;

		case XtCframein_box:
		case XtCframeout_box:
			*x = 2 * (cw->base.box_width / 2) + cw->base.bd_width;
			*y = 2 * (cw->base.box_width / 2) + cw->base.bd_width;
			*width = cw->core.width - 4 * (cw->base.box_width /2) - 2 * cw->base.bd_width;
			*height = cw->core.height - 4 * (cw->base.box_width /2) - 2 * cw->base.bd_width;
			break;

		case XtCshadow_box:
			*x = cw->base.box_width + cw->base.bd_width;
			*y = cw->base.box_width + cw->base.bd_width;
			*width = cw->core.width - 4 * cw->base.box_width - 2 * cw->base.bd_width;
			*height = cw->core.height - 4 * cw->base.box_width - 2 * cw->base.bd_width;
			break;
	}
}

static void SetInternalDimension(Widget w, Dimension width, Dimension height)
{
	MwBaseWidget cw = (MwBaseWidget) w;

	switch (cw->base.box_type)
	{
		case XtCno_box:
			cw->core.width=width + 2 * cw->base.bd_width;
			cw->core.height=height + 2 * cw->base.bd_width;
			break;
			
		case XtCsimple_box:
		case XtCup_box:
		case XtCdown_box:
			cw->core.width = width + 2 * cw->base.box_width + 2 * cw->base.bd_width;
			cw->core.height = height + 2 * cw->base.box_width + 2 * cw->base.bd_width;
			break;
			
		case XtCframein_box:
		case XtCframeout_box:	
			cw->core.width = width + 4 * (cw->base.box_width /2) + 2 * cw->base.bd_width;
			cw->core.height = height + 4 * (cw->base.box_width /2) + 2 * cw->base.bd_width;
			break;
			
		case XtCshadow_box:
			cw->core.width = width + 4 * cw->base.box_width + 2 * cw->base.bd_width;
			cw->core.height = height + 4 * cw->base.box_width + 2 * cw->base.bd_width;
			break;
	}		
}

static void Highlight(Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
	MwBaseWidget cw = (MwBaseWidget) w;
	MwBaseWidgetClass wclass = (MwBaseWidgetClass) cw->core.widget_class;


	if (wclass->base_class.highlight)
		wclass->base_class.highlight(w, event, params, num_params);

	cw->base.entered = True;

}

static void _Highlight(Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
	MwBaseWidget cw = (MwBaseWidget) w;

	if (cw->base.highlight_on_enter && !cw->base.entered &&
		(cw->base.activ_bg != cw->core.background_pixel ||
		cw->base.activ_fg != cw->base.foreground))
	{
		Dimension width,height;
		Position x,y;

		mwBaseClassRec.base_class.get_internal_dimension(w,&x,&y,&width,&height);
		
		XSetWindowBackground(XtDisplay(w) , XtWindow(w) ,
			cw->base.activ_bg);

		XClearArea(XtDisplay(w) , XtWindow(w) , x , y , width , height , True);
	}
}

static void Unhighlight(Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
	MwBaseWidget cw = (MwBaseWidget) w;
	MwBaseWidgetClass wclass = (MwBaseWidgetClass) cw->core.widget_class;


	if (wclass->base_class.unhighlight)
		wclass->base_class.unhighlight(w, event, params, num_params);

	cw->base.entered = False;
}

static void _Unhighlight(Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
	MwBaseWidget cw = (MwBaseWidget) w;
	
	if (cw->base.highlight_on_enter && cw->base.entered &&
		(cw->base.activ_bg !=cw->core.background_pixel ||
		cw->base.activ_fg != cw->base.foreground))
	{
		Dimension width,height;
		Position x,y;

		mwBaseClassRec.base_class.get_internal_dimension(w,&x,&y,&width,&height);

		XSetWindowBackground(XtDisplay(w) , XtWindow(w) , 
			cw->core.background_pixel);

		XClearArea(XtDisplay(w) , XtWindow(w) , x , y , width , height , True);
	}

}

static void ShowHelp(Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
	MwBaseWidget cw = (MwBaseWidget) w;

	if (cw->base.help_text)
	{
		cw->base.timer = XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)w),
				cw->base.help_show_delay , _ShowHelp , w);
	}
}

static void _ShowHelp(XtPointer client_data, XtIntervalId *timer)
{
	MwBaseWidget cw = (MwBaseWidget) client_data;
	Position x,y;
	Display *dpy = XtDisplay((Widget) cw);
	Dimension width,height,
		dwidth = DisplayWidth(dpy,DefaultScreen(dpy)) ,
		dheight = DisplayHeight(dpy,DefaultScreen(dpy)) ;

	cw->base.timer = (XtIntervalId) 0;

	XtVaGetValues(cw->base.hlp , XtNwidth , &width , XtNheight , &height , NULL);
	
	XtTranslateCoords((Widget) cw , cw->core.width / 2 ,
		cw->core.height , &x ,&y);

	if ( (x + width) > dwidth ) x = x - width;
	
	if ( (y + height + 5) > dheight ) y = y - cw->core.height - 5 - height;
	else y = y + 5;
	
	XtVaSetValues(cw->base.hlp,XtNx,x,XtNy,y,NULL);

	XtPopup(cw->base.hlp,XtGrabNone);

}

static void HideHelp(Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
	MwBaseWidget cw = (MwBaseWidget) w;

	if (cw->base.help_text)
	{
		XtPopdown(cw->base.hlp); 
		if (cw->base.timer)
			XtRemoveTimeOut(cw->base.timer);
		cw->base.timer = (XtIntervalId) 0;
	}
}

static Boolean AcceptFocus(Widget w , Time *time)
{
	MwBaseWidget cw = (MwBaseWidget) w;
	MwBaseWidgetClass wclass = (MwBaseWidgetClass) cw->core.widget_class;

	if (!XtIsRealized(w) || !XtIsSensitive(w) || !cw->core.visible ||
		!cw->core.ancestor_sensitive || cw->core.being_destroyed ||
		!XtIsManaged(w))
		return False;

	if (Xt_IsUp(w))
	{
		XSetInputFocus(XtDisplay(w), XtWindow(w), RevertToParent, *time);
		wclass->base_class.highlightBorder(w);
	}
	return True;
}

#define focus_detail(detail) (detail ==NotifyAncestor ?"NotifyAncestor":detail ==NotifyVirtual ?"NotifyVirtual":detail ==NotifyInferior ?"NotifyInferior":detail ==NotifyNonlinear ?"NotifyNonlinear":detail ==NotifyNonlinearVirtual ?"NotifyNonlinearVirtual":detail ==NotifyPointer ?"NotifyPointer":detail ==NotifyPointerRoot ?"NotifyPointerRoot":detail ==NotifyDetailNone ?"NotifyDetailNone":"???")

static void _FocusIn(Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
	MwBaseWidget cw = (MwBaseWidget) w;
	MwBaseWidgetClass wclass = (MwBaseWidgetClass) cw->core.widget_class;

	if ((event->xfocus.detail == NotifyNonlinear ||
		event->xfocus.detail == NotifyAncestor ||
		event->xfocus.detail == NotifyInferior) &&
		!cw->base.focused)
	{
		wclass->base_class.highlightBorder(w);
		cw->base.focused = True;
	}
}

static void _FocusOut(Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
	MwBaseWidget cw = (MwBaseWidget) w;
	MwBaseWidgetClass wclass = (MwBaseWidgetClass) cw->core.widget_class;

	if (cw->base.focused)
	{
		wclass->base_class.unhighlightBorder(w);
		cw->base.focused = False;
	}	
}

static void FocusCurrent(Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
	MwBaseWidget cw = (MwBaseWidget) w;
	Widget parent = XtParent(w);

	if (cw->base.focused) return;

	if (XtCallAcceptFocus(w , &event->xbutton.time))
	{
		if (XtIsSubclass(parent, mwBaseCompWidgetClass))
		        ((MwBaseCompWidgetClass)parent->core.widget_class)->baseComp_class.
        			traverseTo(parent , w , &event->xbutton.time);
		else if (XtIsSubclass(parent, mwBaseConstWidgetClass))
		        ((MwBaseConstWidgetClass)parent->core.widget_class)->baseConst_class.
        			traverseTo(parent , w , &event->xbutton.time);
	}
}

static void TraverseForward(Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
	Widget parent = XtParent(w);
	
	if (XtIsSubclass(parent, mwBaseCompWidgetClass))
	        ((MwBaseCompWidgetClass)parent->core.widget_class)->baseComp_class.
        		traverse(parent , MW_TRAVERSE_NEXT , &event->xkey.time);
	else if (XtIsSubclass(parent, mwBaseConstWidgetClass))
	        ((MwBaseConstWidgetClass)parent->core.widget_class)->baseConst_class.
        		traverse(parent , MW_TRAVERSE_NEXT , &event->xkey.time);

}

static void TraverseBackward(Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
	Widget parent = XtParent(w);

	if (XtIsSubclass(parent, mwBaseCompWidgetClass))
	        ((MwBaseCompWidgetClass)parent->core.widget_class)->baseComp_class.
        		traverse(parent , MW_TRAVERSE_PREV , &event->xkey.time);
        else if (XtIsSubclass(parent, mwBaseConstWidgetClass))
	        ((MwBaseConstWidgetClass)parent->core.widget_class)->baseConst_class.
        		traverse(parent , MW_TRAVERSE_PREV , &event->xkey.time);
}

static void HighlightBorder(Widget w)
{
	MwBaseWidget cw = (MwBaseWidget) w;

	X_DrawSimpleRawFrame(XtDisplay(w) , XtWindow(w) , 0 , 0 , cw->core.width ,
		cw->core.height , cw->base.bd_width , cw->base.bd_color);
}

static void UnhighlightBorder(Widget w)
{
	MwBaseWidget cw = (MwBaseWidget) w;

	X_DrawSimpleRawFrame(XtDisplay(w) , XtWindow(w) , 0 , 0 , cw->core.width ,
		cw->core.height , cw->base.bd_width , cw->core.background_pixel);
}
