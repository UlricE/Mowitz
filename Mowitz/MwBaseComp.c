/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/extensions/shape.h>

#include "MwPopText.h"
#include "MwBaseCompP.h"
#include "MwBaseConstP.h"
#include "MwNws.h"
#include "MwTraverse.h"
#include <stdio.h>
#include "MwUtils.h"

#define offset(field) XtOffsetOf(MwBaseCompRec, baseComp.field)

static XtResource resources[] = { 
	{
	 XtNfont,
	 XtCFont,
	 XtRFontStruct ,
	 sizeof(XFontStruct *) ,
	 offset(font) ,
	 XtRString ,
	 (XtPointer) XtDefaultFont 
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
	 XtNhelp_text,
	 XtCHelp_text,
	 XtRString,
	 sizeof(String),
	 offset(help_text),
	 XtRImmediate,
	 (XtPointer) NULL
	},
	{
	 XtNlabel,
	 XtCLabel,
	 XtRString,
	 sizeof(String),
	 offset(label),
	 XtRImmediate,
	 (XtPointer) NULL
	},
	{
	 XtNjustify,
	 XtCJustify,
	 XtRNwsJustify,
	 sizeof(int),
	 offset(justify),
	 XtRImmediate,
	 (XtPointer) XtCcenter
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
	 XtOffsetOf(MwBaseCompRec , core.border_width),
	 XtRImmediate,
	 (XtPointer) 0
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
	 (XtPointer) 0 ,
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
	 XtNtraverse ,
	 XtCTraverse ,
	 XtRBoolean ,
	 sizeof(Boolean) ,
	 offset(traverse) ,
	 XtRImmediate ,
	 (XtPointer) True ,
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
static Boolean AcceptFocus(Widget, Time *);

static void GetInternalDimension(Widget,Position *,Position *,Dimension *,Dimension *);
static void SetInternalDimension(Widget,Dimension,Dimension);

static Widget Traverse(Widget, int, Time *);
static void  TraverseTo(Widget, Widget, Time *);
static void  TraverseOut(Widget);
static Widget TraverseInside(Widget, int, Time *);
static void  ShowHelp(Widget, XEvent *, String *, Cardinal *);
static void  HideHelp(Widget, XEvent *, String *, Cardinal *);
static void  _ShowHelp(XtPointer, XtIntervalId *);
static void  _FocusIn(Widget, XEvent *, String *, Cardinal *);
static void  _FocusOut(Widget, XEvent *, String *, Cardinal *);
static void  HighlightBorder(Widget);
static void  UnhighlightBorder(Widget);
static void  TraverseForward(Widget, XEvent *, String *, Cardinal *);
static void  TraverseBackward(Widget, XEvent *, String *, Cardinal *);
static void  FocusCurrent(Widget, XEvent *, String *, Cardinal *);
static void  ChangeManaged(Widget);

static XtActionsRec action [] = {
        {"show_help",ShowHelp},
        {"hide_help",HideHelp},
        {"focusIn",_FocusIn},
        {"focusOut",_FocusOut},
	{"focusCurrent" , FocusCurrent},
        {"traverseForward" , TraverseForward},
        {"traverseBackward" , TraverseBackward},        
        };

static char trans_tab [] = 
        "<Enter>: show_help()\n\
	 <Leave>: hide_help()\n\
	 ~Shift<Key>Tab: traverseForward()\n\
	 Shift<Key>Tab: traverseBackward()\n\
	 <FocusIn>: focusIn()\n\
	 <FocusOut>: focusOut()\n\
	 <BtnDown>: hide_help() focusCurrent()\n\
	 <KeyDown>: hide_help()";

MwBaseCompClassRec mwBaseCompClassRec = {
/* core */
   {
    /* superclass            */ (WidgetClass) &compositeClassRec,
    /* class_name            */ "MwBaseComp",
    /* widget_size           */ sizeof(MwBaseCompRec),
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
/* composite */
   {
    /* geometry_manager	     */ XtInheritGeometryManager,
    /* change_managed	     */ ChangeManaged,
    /* insert_child	     */ XtInheritInsertChild,
    /* delete_child	     */ XtInheritDeleteChild,
    /* extension	     */ NULL
   },
/* baseComp */
   {
    /* get_internal_dimension  */ GetInternalDimension,
    /* set_internal_dimension  */ SetInternalDimension,
    /* traverse		       */ Traverse,
    /* traverseTo	       */ TraverseTo,
    /* traverseOut	       */ TraverseOut,
    /* traverseInside	       */ TraverseInside,
    /* highlightBorder	       */ HighlightBorder,
    /* unhighlightBorder       */ UnhighlightBorder,
   },
};

WidgetClass mwBaseCompWidgetClass = (WidgetClass) &mwBaseCompClassRec;

#define ForAllChildren(cw, child) \
        for ( (child) = (cw)->composite.children ; \
                (child) < ((cw)->composite.children + \
                (cw)->composite.num_children ) ; \
                (child)++ )

static void ClassInitialize(void)
{
	_InitializeWidgetSet();

	XtSetTypeConverter(XtRString , XtRBox_type , cvtStringToBoxType ,
			NULL , 0 , XtCacheNone , NULL);

	XtSetTypeConverter(XtRBox_type , XtRString , cvtBoxTypeToString ,
			NULL , 0 , XtCacheNone , NULL);
           
	XtSetTypeConverter(XtRString , XtRNwsJustify , cvtStringToJustify ,
			NULL , 0 , XtCacheNone , NULL);

}

static void ResolveInheritance(WidgetClass class)
{
	MwBaseCompWidgetClass c = (MwBaseCompWidgetClass) class;
	MwBaseCompWidgetClass super;
	static CompositeClassExtensionRec extension_rec = {
		NULL, NULLQUARK, XtCompositeExtensionVersion,
		sizeof(CompositeClassExtensionRec), True};
	CompositeClassExtensionRec *ext;

  	ext = MwMalloc(sizeof(*ext));
	*ext = extension_rec;
	ext->next_extension = c->composite_class.extension;
	c->composite_class.extension = ext;

	if (class == mwBaseCompWidgetClass) return;
	super = (MwBaseCompWidgetClass)class->core_class.superclass;

	if (c->baseComp_class.get_internal_dimension == XtInheritGetInternalDimension)
		c->baseComp_class.get_internal_dimension =
			super->baseComp_class.get_internal_dimension;

	if (c->baseComp_class.set_internal_dimension == XtInheritSetInternalDimension)
		c->baseComp_class.set_internal_dimension =
			super->baseComp_class.set_internal_dimension;

	if (c->baseComp_class.traverse == XtInheritTraverse)
		c->baseComp_class.traverse =
			super->baseComp_class.traverse;

	if (c->baseComp_class.traverseTo == XtInheritTraverseTo)
		c->baseComp_class.traverseTo =
			super->baseComp_class.traverseTo;

	if (c->baseComp_class.traverseOut == XtInheritTraverseOut)
		c->baseComp_class.traverseOut =
			super->baseComp_class.traverseOut;

	if (c->baseComp_class.traverseInside == XtInheritTraverseInside)
		c->baseComp_class.traverseInside =
			super->baseComp_class.traverseInside;

	if (c->baseComp_class.highlightBorder == XtInheritHighlightBorder)
		c->baseComp_class.highlightBorder =
			super->baseComp_class.highlightBorder;

	if (c->baseComp_class.unhighlightBorder == XtInheritUnhighlightBorder)
		c->baseComp_class.unhighlightBorder =
			super->baseComp_class.unhighlightBorder;

}

static void Initialize(Widget req_widget, Widget new_widget,
		ArgList args, Cardinal *num_args)
{
	MwBaseCompWidget nw = (MwBaseCompWidget) new_widget;
	XColor	dark,light,bg;
	Display *dpy=XtDisplay(new_widget);
	XGCValues gc_res;
	XtGCMask  gc_mask;

	bg.pixel = nw->baseComp.box_color;
	
	XQueryColor(dpy,DefaultColormap(dpy,DefaultScreen(dpy)),&bg);
	
	LightColor2(new_widget, light, nw->baseComp.top_shadow_contrast);
	DarkColor2(new_widget, dark, nw->baseComp.bot_shadow_contrast);

	nw->baseComp.light = light.pixel;
	nw->baseComp.dark = dark.pixel;

	gc_res.foreground = nw->baseComp.box_color;
	gc_res.font = nw->baseComp.font->fid;

	gc_mask = GCForeground | GCFont;

	nw->baseComp.gc = XCreateGC(dpy,DefaultRootWindow(dpy),
			gc_mask, &gc_res);

	if (nw->baseComp.help_text)
	{
		nw->baseComp.help_text = MwStrdup(nw->baseComp.help_text);
		nw->baseComp.hlp = XtVaCreatePopupShell("___help" , mwPopTextWidgetClass ,
				new_widget , XtNtext , nw->baseComp.help_text,NULL);
	}

	if (nw->baseComp.label) nw->baseComp.label = MwStrdup(nw->baseComp.label);

	if (nw->baseComp.box_type == XtCshadow_box) 
	{
		nw->baseComp.have_shape = True;
	}
	else 
	{
		nw->baseComp.have_shape = False;
	}
	nw->baseComp.timer = (XtIntervalId) 0;
	nw->baseComp.current_focused = NULL;
	nw->baseComp.focused = False;
	nw->baseComp.traverse_direction = MW_TRAVERSE_FIRST;

}

static void Realize(Widget w, Mask *valueMask, XSetWindowAttributes *attributes)
{
	MwBaseCompWidget cw = (MwBaseCompWidget) w;

	if ((attributes->cursor = cw->baseComp.cursor) != None)
		*valueMask |= CWCursor;

	compositeClassRec.core_class.realize(w,valueMask,attributes);

	if (cw->baseComp.box_type == XtCshadow_box)
	{
		Region region;
		XPoint points[8];

		points[0].x = 0;
		points[0].y = 0;
		points[1].x = cw->core.width - 2 * cw->baseComp.box_width;
		points[1].y = 0;
		points[2].x = cw->core.width - 2 * cw->baseComp.box_width;
		points[2].y = 2 * cw->baseComp.box_width;
		points[3].x = cw->core.width;
		points[3].y = 2 * cw->baseComp.box_width;
		points[4].x = cw->core.width;
		points[4].y = cw->core.height;
		points[5].x = 2 * cw->baseComp.box_width;
		points[5].y = cw->core.height;
		points[6].x = 2 * cw->baseComp.box_width;
		points[6].y = cw->core.height - 2 * cw->baseComp.box_width;
		points[7].x = 0;
		points[7].y = cw->core.height - 2 * cw->baseComp.box_width;


		region = XPolygonRegion(points , XtNumber(points) , EvenOddRule);

		XShapeCombineRegion(XtDisplay(w) , XtWindow(w) , ShapeBounding ,
			0 , 0 , region , ShapeSet);

		XDestroyRegion(region);
	}
}

static void Destroy(Widget w)
{
	MwBaseCompWidget cw = (MwBaseCompWidget) w;

        if (cw->baseComp.help_text)
                MwFree(cw->baseComp.help_text);

        XFreeGC(XtDisplay(w),cw->baseComp.gc);
}

static void Redisplay(Widget w, XEvent *event, Region region)
{
	MwBaseCompWidget cw = (MwBaseCompWidget) w;
	Display *dpy = XtDisplay(w);
	Window win = XtWindow (w);
	cw->core.border_width=0;
	if (cw->baseComp.label &&
		(cw->baseComp.box_type == XtCno_box ||
		 cw->baseComp.box_type == XtCsimple_box ||
		 cw->baseComp.box_type == XtCup_box ||
		 cw->baseComp.box_type == XtCdown_box ||
		 cw->baseComp.box_type == XtCshadow_box ))
		 
	{
		Position x;

		switch(cw->baseComp.justify)
		{
			case XtCleft:
				x = 4 * cw->baseComp.box_width + cw->baseComp.bd_width;
				break;

			case XtCright:
				x = cw->core.width - cw->baseComp.bd_width -
					XTextWidth(cw->baseComp.font , 
					cw->baseComp.label , strlen(cw->baseComp.label))
					- 4 * cw->baseComp.box_width -
					(cw->baseComp.box_type == XtCshadow_box ?
					2 * cw->baseComp.box_width : 0);
				break;

			case XtCcenter:
			default:
				x = (cw->core.width  - 2 * cw->baseComp.bd_width -
					XTextWidth(cw->baseComp.font , 
					cw->baseComp.label , 
					strlen(cw->baseComp.label))) / 2;
				break;

				
		}
		XSetForeground(dpy , cw->baseComp.gc , cw->baseComp.box_color);
		XSetBackground(dpy , cw->baseComp.gc , cw->baseComp.box_color);
		XFillRectangle(dpy , win , cw->baseComp.gc ,
			cw->baseComp.bd_width , cw->baseComp.bd_width ,
			cw->core.width - cw->baseComp.bd_width,
			cw->baseComp.font->max_bounds.descent +
			cw->baseComp.font->max_bounds.ascent + 
			2 * cw->baseComp.box_width );
        
		XSetForeground(dpy , cw->baseComp.gc , cw->baseComp.foreground);
		XDrawImageString(dpy , win , cw->baseComp.gc , x ,
			cw->baseComp.font->max_bounds.ascent + 
			cw->baseComp.box_width + cw->baseComp.bd_width,
			cw->baseComp.label , strlen(cw->baseComp.label));
	}
	switch (cw->baseComp.box_type)
	{
		case XtCno_box:
		    break;

		case XtCsimple_box:
		    X_DrawSimpleRawFrame(dpy , win , cw->baseComp.bd_width ,
		        cw->baseComp.bd_width ,
		        cw->core.width - 2 * cw->baseComp.bd_width,
			cw->core.height - 2 * cw->baseComp.bd_width ,
			cw->baseComp.box_width , 
			cw->baseComp.box_color);
		    break;

		case XtCup_box:
		    X_DrawSimple3DFrame(dpy , win , cw->baseComp.bd_width ,
		        cw->baseComp.bd_width , 
		        cw->core.width - 2 * cw->baseComp.bd_width ,
			cw->core.height - 2 * cw->baseComp.bd_width ,
			cw->baseComp.box_width ,
			cw->baseComp.light , cw->baseComp.dark);
		    break;

		case XtCdown_box:
		    X_DrawSimple3DFrame(dpy , win , cw->baseComp.bd_width ,
		        cw->baseComp.bd_width ,
		        cw->core.width - 2 * cw->baseComp.bd_width,
			cw->core.height - 2 * cw->baseComp.bd_width ,
			cw->baseComp.box_width ,
			cw->baseComp.dark , cw->baseComp.light);
		    break;

		case XtCframein_box:
		   if (cw->baseComp.label )
		   {
			Position x;
	    		char *p = MwMalloc(strlen(cw->baseComp.label)+3);
		    	sprintf(p , " %s " ,cw->baseComp.label);

			switch(cw->baseComp.justify)
			{
				case XtCleft:
					x = 4 * cw->baseComp.box_width +
						cw->baseComp.bd_width;
					break;

				case XtCright:
					x = cw->core.width - cw->baseComp.bd_width -
						XTextWidth(cw->baseComp.font , 
							p , strlen(p)) - 
						4 * cw->baseComp.box_width;
					break;

				case XtCcenter:
				default:
					x = (cw->core.width - 2 * cw->baseComp.bd_width - 
						XTextWidth(cw->baseComp.font , 
							p , strlen(p))) / 2;
					break;
				
			}

			X_DrawSimple3DFrame(dpy , win , cw->baseComp.bd_width ,
				(cw->baseComp.box_width +
				cw->baseComp.font->max_bounds.descent +
				cw->baseComp.font->max_bounds.ascent) / 2 +
				cw->baseComp.bd_width,
				cw->core.width - 2 * cw->baseComp.bd_width ,
				cw->core.height - (cw->baseComp.box_width +
				cw->baseComp.font->max_bounds.descent +
				cw->baseComp.font->max_bounds.ascent) / 2 
				- 2 * cw->baseComp.bd_width, 
				cw->baseComp.box_width / 2 ,
				cw->baseComp.dark , cw->baseComp.light);

			X_DrawSimple3DFrame(dpy , win ,
				cw->baseComp.box_width / 2 + cw->baseComp.bd_width,
				cw->baseComp.box_width / 2 +
				(cw->baseComp.box_width + 
				cw->baseComp.font->max_bounds.descent +
				cw->baseComp.font->max_bounds.ascent) / 2 +
				cw->baseComp.bd_width,
				cw->core.width - 2 * (cw->baseComp.box_width / 2) 
				- 2 * cw->baseComp.bd_width,
				cw->core.height - 2 * (cw->baseComp.box_width / 2) - 
				(cw->baseComp.box_width +
				cw->baseComp.font->max_bounds.descent +
				cw->baseComp.font->max_bounds.ascent) / 2
				- 2 * cw->baseComp.bd_width,
				cw->baseComp.box_width / 2 ,
				cw->baseComp.light , cw->baseComp.dark);

			XSetForeground(dpy , cw->baseComp.gc , cw->baseComp.foreground);
			XSetBackground(dpy , cw->baseComp.gc , cw->core.background_pixel);
			XDrawImageString(dpy , win , cw->baseComp.gc ,
				x ,
				cw->baseComp.box_width +
				cw->baseComp.font->max_bounds.ascent +
				cw->baseComp.bd_width,
				p , strlen(p));

			MwFree(p);
		    }
		    else
		    {
			X_DrawSimple3DFrame(dpy , win , cw->baseComp.bd_width ,
				cw->baseComp.bd_width ,
				cw->core.width - 2 * cw->baseComp.bd_width,
				cw->core.height - 2 * cw->baseComp.bd_width ,
				cw->baseComp.box_width / 2 ,
				cw->baseComp.dark , cw->baseComp.light);

			X_DrawSimple3DFrame(dpy , win , 
				cw->baseComp.box_width / 2 + cw->baseComp.bd_width,
				cw->baseComp.box_width / 2 + cw->baseComp.bd_width,
				cw->core.width - 2 * (cw->baseComp.box_width / 2)
				- 2 * cw->baseComp.bd_width,
				cw->core.height - 2 * (cw->baseComp.box_width / 2)
				- 2 * cw->baseComp.bd_width,
				cw->baseComp.box_width / 2 ,
				cw->baseComp.light , cw->baseComp.dark);
		    }
		    break;

		case XtCframeout_box:
		   if (cw->baseComp.label )
		   {
			Position x;
	    		char *p = MwMalloc(strlen(cw->baseComp.label)+3);
		    	sprintf(p , " %s " ,cw->baseComp.label);

			switch(cw->baseComp.justify)
			{
				case XtCleft:
					x = 4 * cw->baseComp.box_width + cw->baseComp.bd_width;
					break;

				case XtCright:
					x = cw->core.width - cw->baseComp.bd_width -
						XTextWidth(cw->baseComp.font , 
							p , strlen(p)) - 
						4 * cw->baseComp.box_width;
					break;

				case XtCcenter:
				default:
					x = (cw->core.width - 2 * cw->baseComp.bd_width - 
						XTextWidth(cw->baseComp.font , 
							p , strlen(p))) / 2;
					break;
				
			}


			X_DrawSimple3DFrame(dpy , win , cw->baseComp.bd_width ,
				(cw->baseComp.box_width +
				cw->baseComp.font->max_bounds.descent +
				cw->baseComp.font->max_bounds.ascent) / 2 + cw->baseComp.bd_width,
				cw->core.width - 2 * cw->baseComp.bd_width,
				cw->core.height - (cw->baseComp.box_width +
				cw->baseComp.font->max_bounds.descent +				
				cw->baseComp.font->max_bounds.ascent) / 2
				- 2 * cw->baseComp.bd_width ,
				cw->baseComp.box_width / 2 ,
				cw->baseComp.light , cw->baseComp.dark);

			X_DrawSimple3DFrame(dpy , win , 
				cw->baseComp.box_width / 2 + cw->baseComp.bd_width,
				cw->baseComp.box_width / 2 +
				(cw->baseComp.box_width +
				cw->baseComp.font->max_bounds.descent +
				cw->baseComp.font->max_bounds.ascent) / 2 +
				cw->baseComp.bd_width,
				cw->core.width - 2 * (cw->baseComp.box_width / 2)
				- 2 * cw->baseComp.bd_width,
				cw->core.height - 2 * (cw->baseComp.box_width / 2) -
				(cw->baseComp.box_width +
				cw->baseComp.font->max_bounds.descent +
				cw->baseComp.font->max_bounds.ascent) / 2
				- 2 * cw->baseComp.bd_width,
				cw->baseComp.box_width / 2 ,
				cw->baseComp.dark , cw->baseComp.light);

			XSetForeground(dpy , cw->baseComp.gc , cw->baseComp.foreground);
			XSetBackground(dpy , cw->baseComp.gc , cw->core.background_pixel);
			XDrawImageString(dpy , win , cw->baseComp.gc ,
				x , cw->baseComp.box_width +
				cw->baseComp.font->max_bounds.ascent +
				cw->baseComp.bd_width,
				p , strlen(p));

			MwFree(p);
		    }
		    else
		    {
			X_DrawSimple3DFrame(dpy , win , cw->baseComp.bd_width ,
				cw->baseComp.bd_width ,
				cw->core.width - 2 * cw->baseComp.bd_width,
				cw->core.height - 2 * cw->baseComp.bd_width ,
				cw->baseComp.box_width / 2 ,
				cw->baseComp.light , cw->baseComp.dark);
			X_DrawSimple3DFrame(dpy , win ,
				cw->baseComp.box_width / 2 + cw->baseComp.bd_width ,
				cw->baseComp.box_width / 2 + cw->baseComp.bd_width , 
				cw->core.width - 2 * (cw->baseComp.box_width / 2) 
				- 2 * cw->baseComp.bd_width,
				cw->core.height - 2 * (cw->baseComp.box_width / 2) 
				- 2 * cw->baseComp.bd_width, 
				cw->baseComp.box_width / 2 ,
				cw->baseComp.dark , cw->baseComp.light);
		    }
		    break;

		case XtCshadow_box:
		   {
			XPoint points[6];

			points[0].x = 2 * cw->baseComp.box_width + cw->baseComp.bd_width;
			points[0].y = cw->core.height - cw->baseComp.bd_width;
			points[1].x = 2 * cw->baseComp.box_width + cw->baseComp.bd_width;
			points[1].y = cw->core.height - 2 * cw->baseComp.box_width - cw->baseComp.bd_width;
			points[2].x = cw->core.width - 2 * cw->baseComp.box_width - cw->baseComp.bd_width;
			points[2].y = cw->core.height - 2 * cw->baseComp.box_width - cw->baseComp.bd_width;
			points[3].x = cw->core.width - 2 * cw->baseComp.box_width - cw->baseComp.bd_width;
			points[3].y = 2 * cw->baseComp.box_width + cw->baseComp.bd_width;
			points[4].x = cw->core.width - cw->baseComp.bd_width;
			points[4].y = 2 * cw->baseComp.box_width + cw->baseComp.bd_width;
			points[5].x = cw->core.width - cw->baseComp.bd_width;
			points[5].y = cw->core.height - cw->baseComp.bd_width;

			X_DrawSimpleRawFrame (dpy , win , cw->baseComp.bd_width , 
				cw->baseComp.bd_width ,
				cw->core.width - (2 * cw->baseComp.box_width)
				- 2 * cw->baseComp.bd_width,
				cw->core.height - (2 * cw->baseComp.box_width)
				- 2 * cw->baseComp.bd_width,
				cw->baseComp.box_width , cw->baseComp.box_color );

			XSetForeground(dpy , cw->baseComp.gc , cw->baseComp.dark);

			XFillPolygon(dpy , win , cw->baseComp.gc , points , 
				XtNumber(points),Nonconvex , CoordModeOrigin);
			
			break;
		    }
	}

       if (cw->baseComp.focused) 
               ((MwBaseCompWidgetClass)w->core.widget_class)
               		->baseComp_class.highlightBorder(w);

	if (!XtIsSensitive(w)) Xt_SetInsensitive(w);
}

static void Resize(Widget w)
{
	MwBaseCompWidget cw = (MwBaseCompWidget) w;

	if (!XtIsRealized(w)) return;

	if (cw->baseComp.have_shape)
	{
		if (cw->baseComp.box_type == XtCshadow_box)
		{
			Region region;
			XPoint points[8];

			points[0].x = 0;
			points[0].y = 0;
			points[1].x = cw->core.width - 2 * cw->baseComp.box_width;
			points[1].y = 0;
			points[2].x = cw->core.width - 2 * cw->baseComp.box_width;
			points[2].y = 2 * cw->baseComp.box_width;
			points[3].x = cw->core.width;
			points[3].y = 2 * cw->baseComp.box_width;
			points[4].x = cw->core.width;
			points[4].y = cw->core.height;
			points[5].x = 2 * cw->baseComp.box_width;
			points[5].y = cw->core.height;
			points[6].x = 2 * cw->baseComp.box_width;
			points[6].y = cw->core.height - 2 * cw->baseComp.box_width;
			points[7].x = 0;
			points[7].y = cw->core.height - 2 * cw->baseComp.box_width;
	
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


#define WidgetValuesDiffer(w1,w2,component) (w1 -> baseComp.component != \
                                             w2 -> baseComp.component)

static Boolean SetValues(Widget current, Widget request, Widget new_widget,
		ArgList args, Cardinal *num_args)
{
	MwBaseCompWidget cw = (MwBaseCompWidget) current;
	MwBaseCompWidget nw = (MwBaseCompWidget) new_widget;
	Widget w = (Widget) new_widget;
	Boolean redraw=False;
	
	if WidgetValuesDiffer( cw , nw , cursor)
	{
		XDefineCursor(XtDisplay(current),XtWindow(current),nw->baseComp.cursor);
	}

	if (WidgetValuesDiffer( cw , nw , box_type) ||
		WidgetValuesDiffer( cw , nw , box_width))
	{
		if (cw->baseComp.box_type == XtCshadow_box && 
			nw->baseComp.box_type != XtCshadow_box)
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
		if (nw->baseComp.box_type == XtCshadow_box &&
			cw->baseComp.box_type != XtCshadow_box)
		{
			Region region;
			XPoint points[8];

			points[0].x = 0;
			points[0].y = 0;
			points[1].x = cw->core.width - 2 * cw->baseComp.box_width;
			points[1].y = 0;
			points[2].x = cw->core.width - 2 * cw->baseComp.box_width;
			points[2].y = 2 * cw->baseComp.box_width;
			points[3].x = cw->core.width;
			points[3].y = 2 * cw->baseComp.box_width;
			points[4].x = cw->core.width;
			points[4].y = cw->core.height;
			points[5].x = 2 * cw->baseComp.box_width;
			points[5].y = cw->core.height;
			points[6].x = 2 * cw->baseComp.box_width;
			points[6].y = cw->core.height - 2 * cw->baseComp.box_width;
			points[7].x = 0;
			points[7].y = cw->core.height - 2 * cw->baseComp.box_width;

			region = XPolygonRegion(points , XtNumber(points) , EvenOddRule);

			XShapeCombineRegion(XtDisplay(w) , XtWindow(w) , ShapeBounding ,
				0 , 0 , region , ShapeSet);

			XDestroyRegion(region);

			nw->baseComp.have_shape=True;

		}
		redraw = True;
	}

	if WidgetValuesDiffer( cw , nw , help_text)
	{
		if (cw->baseComp.help_text)
		{
			MwFree(cw->baseComp.help_text);
			cw->baseComp.help_text = NULL;
			XtDestroyWidget(cw->baseComp.hlp);
		}
		if (nw->baseComp.help_text)
		{
			nw->baseComp.help_text = MwStrdup(nw->baseComp.help_text);
			nw->baseComp.hlp = XtVaCreatePopupShell("___help" , mwPopTextWidgetClass ,
				new_widget , XtNtext , nw->baseComp.help_text,NULL);
		}
	}

	
	if WidgetValuesDiffer( cw , nw , label )
	{
		if (cw->baseComp.label)
		{
			MwFree(cw->baseComp.label);
			cw->baseComp.label = NULL;
		}
		if (nw->baseComp.label)
			nw->baseComp.label = MwStrdup(nw->baseComp.label);

		redraw = True;
	}

	if WidgetValuesDiffer( cw , nw , font )
	{
		XSetFont(XtDisplay(w) , nw->baseComp.gc , nw->baseComp.font->fid);
		
		redraw = True;
	}

	return redraw;
}

static void ChangeManaged(Widget w)
{
	MwBaseCompWidget cw = (MwBaseCompWidget) w;
	Widget *child;

	if (constraintClassRec.composite_class.change_managed)
		constraintClassRec.composite_class.change_managed(w);

	if (cw->baseComp.current_focused)
	{
		ForAllChildren(cw , child)
		{
			if (!XtIsManaged(*child) &&
				*child == cw->baseComp.current_focused)
			{
				((MwBaseCompWidgetClass)w->core.widget_class)
					->baseComp_class.traverseOut(w);				
			}
		}
	}

}


static void GetInternalDimension(Widget w, Position *x, Position *y,
		Dimension *width, Dimension *height)
{
	MwBaseCompWidget cw = (MwBaseCompWidget) w;
	
	switch (cw->baseComp.box_type)
	{
		case XtCno_box:
			*x = cw->baseComp.bd_width;
			*width = cw->core.width - 2 * cw->baseComp.bd_width;
			*y = cw->baseComp.bd_width;
			*height = cw->core.height - 2 * cw->baseComp.bd_width;
			break;

		case XtCsimple_box:
		case XtCup_box:
		case XtCdown_box:
			*x = cw->baseComp.box_width + cw->baseComp.bd_width;
			*width = cw->core.width - 2 * cw->baseComp.box_width
				- 2 * cw->baseComp.bd_width;
			*y = cw->baseComp.box_width + cw->baseComp.bd_width;
			*height = cw->core.height - 2 * cw->baseComp.box_width 
				- 2 * cw->baseComp.bd_width;
			break;

		case XtCframein_box:
		case XtCframeout_box:
			*x = 2 * (cw->baseComp.box_width / 2) + cw->baseComp.bd_width;
			*y = 2 * (cw->baseComp.box_width / 2) + cw->baseComp.bd_width;
			*width = cw->core.width - 4 * (cw->baseComp.box_width /2) 
				- 2 * cw->baseComp.bd_width;
			*height = cw->core.height - 4 * (cw->baseComp.box_width /2) 
				- 2 * cw->baseComp.bd_width;
			break;

		case XtCshadow_box:
			*x = cw->baseComp.box_width + cw->baseComp.bd_width;
			*y = cw->baseComp.box_width + cw->baseComp.bd_width;
			*width = cw->core.width - 4 * cw->baseComp.box_width 
				- 2 * cw->baseComp.bd_width;
			*height = cw->core.height - 4 * cw->baseComp.box_width 
				- 2 * cw->baseComp.bd_width;
			break;
	}

	if (cw->baseComp.label)
	{
		*y += cw->baseComp.font->max_bounds.descent +
			cw->baseComp.font->max_bounds.ascent;

		*height -= cw->baseComp.font->max_bounds.descent +
			cw->baseComp.font->max_bounds.ascent;
	}
}

static void SetInternalDimension(Widget w, Dimension width, Dimension height)
{
	MwBaseCompWidget cw = (MwBaseCompWidget) w;

	switch (cw->baseComp.box_type)
	{
		case XtCno_box:
			cw->core.width = width + 2 * cw->baseComp.bd_width;
			cw->core.height = height + 2 * cw->baseComp.bd_width;
			break;
			
		case XtCsimple_box:
		case XtCup_box:
		case XtCdown_box:
			cw->core.width = width + 2 * cw->baseComp.box_width 
				+ 2 * cw->baseComp.bd_width;
			cw->core.height = height + 2 * cw->baseComp.box_width
				+ 2 * cw->baseComp.bd_width;
			break;
			
		case XtCframein_box:
		case XtCframeout_box:	
			cw->core.width = width + 4 * (cw->baseComp.box_width /2)
				+ 2 * cw->baseComp.bd_width;
			cw->core.height = height + 4 * (cw->baseComp.box_width /2)
				+ 2 * cw->baseComp.bd_width;
			break;
			
		case XtCshadow_box:
			cw->core.width = width + 4 * cw->baseComp.box_width
				+ 2 * cw->baseComp.bd_width;
			cw->core.height = height + 4 * cw->baseComp.box_width
				+ 2 * cw->baseComp.bd_width;
			break;
	}		
	if (cw->baseComp.label)
	{
		cw->core.height = cw->core.height + 
			cw->baseComp.font->max_bounds.descent +
			cw->baseComp.font->max_bounds.ascent + 
			2 * cw->baseComp.box_width;	
	}
}


static void ShowHelp(Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
	MwBaseCompWidget cw = (MwBaseCompWidget) w;

	if (cw->baseComp.help_text)
	{
		cw->baseComp.timer = XtAppAddTimeOut(XtWidgetToApplicationContext(
					(Widget)w),cw->baseComp.help_show_delay ,
					_ShowHelp , w);
	}
}

static void _ShowHelp(XtPointer client_data, XtIntervalId *timer)
{
	MwBaseCompWidget cw = (MwBaseCompWidget) client_data;
	Position x,y;
	Display *dpy = XtDisplay((Widget) cw);
	Dimension width,height,
		dwidth = DisplayWidth(dpy,DefaultScreen(dpy)) ,
		dheight = DisplayHeight(dpy,DefaultScreen(dpy)) ;

	cw->baseComp.timer = (XtIntervalId) 0;

	XtVaGetValues(cw->baseComp.hlp , XtNwidth , &width , XtNheight , &height , NULL);
	
	XtTranslateCoords((Widget) cw , cw->core.width / 2 ,
		cw->core.height , &x ,&y);

	if ( (x + width) > dwidth ) x = x - width;
	
	if ( (y + height + 5) > dheight ) y = y - cw->core.height - 5 - height;
	else y = y + 5;
	
	XtVaSetValues(cw->baseComp.hlp,XtNx,x,XtNy,y,NULL);

	XtPopup(cw->baseComp.hlp,XtGrabNone);

}

static void HideHelp(Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
	MwBaseCompWidget cw = (MwBaseCompWidget) w;

	if (cw->baseComp.help_text)
	{
		XtPopdown(cw->baseComp.hlp); 
		if (cw->baseComp.timer)
			XtRemoveTimeOut(cw->baseComp.timer);
		cw->baseComp.timer = (XtIntervalId) 0;

	}
}

static Widget TraverseInside(Widget w , int where , Time *time)
{
	MwBaseCompWidget cw = (MwBaseCompWidget) w;
	int i;

	switch (where)
	{
		case MW_TRAVERSE_FIRST:
			cw->baseComp.current_focused = NULL;
			for(i = 0  ; i < cw->composite.num_children ; i++)
			{
			    	MwSetTraverseDirection(cw->composite.children[i] , where);
				if (XtCallAcceptFocus(cw->composite.children[i] , time))
				{
					cw->baseComp.current_focused = 
						cw->composite.children[i];
					break;
				}
			}
			break;
		case MW_TRAVERSE_LAST:
			cw->baseComp.current_focused = NULL;
			for(i = cw->composite.num_children - 1 ; i >= 0 ; i--)
			{
			    	MwSetTraverseDirection(cw->composite.children[i] , where);
				if (XtCallAcceptFocus(cw->composite.children[i] , time))
				{
					cw->baseComp.current_focused = 
						cw->composite.children[i];
					break;
				}
			}
			break;
		case MW_TRAVERSE_PREV:
		    if (cw->composite.num_children)
		    {
			for(i = 0 ;  (i < cw->composite.num_children) &&
				(cw->baseComp.current_focused != 
				cw->composite.children[i]) ; i++);

			if (!(i < cw->composite.num_children) ||
				!cw->baseComp.current_focused)
			{
			    cw->baseComp.current_focused = NULL;
			    for(i = cw->composite.num_children - 1  ; i >= 0 ; i--)
			    {
			    	MwSetTraverseDirection(cw->composite.children[i] , where);
				if (XtCallAcceptFocus(cw->composite.children[i] , time))
				{
					cw->baseComp.current_focused = 
						cw->composite.children[i];
					break;
				}
			    }	
			}
			else
			{
			    cw->baseComp.current_focused = NULL;
			    for( i = i - 1 ; i >= 0 ; i--)
			    {
			    	MwSetTraverseDirection(cw->composite.children[i] , where);
				if (XtCallAcceptFocus(cw->composite.children[i] , time))
				{
					cw->baseComp.current_focused = 
						cw->composite.children[i];
					break;
				}
			    }
			}
		    }
		    else
		    {
			    cw->baseComp.current_focused = NULL;
		    }
		    break;
		case MW_TRAVERSE_NEXT:
		    if (cw->composite.num_children)
		    {
			for(i = 0 ;  (i < cw->composite.num_children) &&
				(cw->baseComp.current_focused != 
				cw->composite.children[i])  ; i++);

			if (!(i < cw->composite.num_children) || 
				!cw->baseComp.current_focused)
			{
			    cw->baseComp.current_focused = NULL;
			    for(i = 0 ; i < cw->composite.num_children ; i++)
			    {
			    	MwSetTraverseDirection(cw->composite.children[i] , where);
				if (XtCallAcceptFocus(cw->composite.children[i] , time))
				{
					cw->baseComp.current_focused = 
						cw->composite.children[i];
					break;
				}
			    }			
			}
			else
			{
			    cw->baseComp.current_focused = NULL;
			    for ( i = i + 1 ; i < cw->composite.num_children ; i++)
			    {
			    	MwSetTraverseDirection(cw->composite.children[i] , where);
				if (XtCallAcceptFocus(cw->composite.children[i] , time))
				{
					cw->baseComp.current_focused = 
						cw->composite.children[i];
					break;
				}
			    }
			}
		    }
		    else
		    {
			    cw->baseComp.current_focused = NULL;
		    }
		    break;
		case MW_TRAVERSE_ACTUAL:
		    if (cw->composite.num_children)
		    {
			for(i = 0 ;  i < cw->composite.num_children &&
				cw->baseComp.current_focused != 
				cw->composite.children[i]  ; i++);

			if (i < cw->composite.num_children &&
				cw->baseComp.current_focused)
			{
			    	MwSetTraverseDirection(cw->composite.children[i] , where);
				XtCallAcceptFocus(cw->composite.children[i] , time);
			}
		    }
		    else
		    {
			    cw->baseComp.current_focused = NULL;
		    }
		    break;
		default: XtWarning("Unknown direction");
	}

	return cw->baseComp.current_focused;		
}

static Widget Traverse(Widget w , int where , Time *time)
{
	MwBaseCompWidget cw = (MwBaseCompWidget) w;
	Widget parent = XtParent(w);
	Widget cld;

	cld = ((MwBaseCompWidgetClass)w->core.widget_class)->
			baseComp_class.traverseInside(w , where , time);

	if (cld)
	{
		TraverseTo(w , cld , time);
	}
	else
	{
	    switch (where)
	    {
		case MW_TRAVERSE_FIRST:
			if (XtIsSubclass(parent, mwBaseCompWidgetClass))
			      	((MwBaseCompWidgetClass)parent->core.widget_class)->
			      		baseComp_class.traverse(parent , 
			      		MW_TRAVERSE_PREV , time);
			else if (XtIsSubclass(parent, mwBaseConstWidgetClass))
		        	((MwBaseConstWidgetClass)parent->core.widget_class)->
		        		baseConst_class.traverse(parent , 
		        		MW_TRAVERSE_PREV , time);
			break;
		case MW_TRAVERSE_LAST:
			if (XtIsSubclass(parent, mwBaseCompWidgetClass))
			      	((MwBaseCompWidgetClass)parent->core.widget_class)->
			      		baseComp_class.traverse(parent , 
			      		MW_TRAVERSE_NEXT , time);
			else if (XtIsSubclass(parent, mwBaseConstWidgetClass))
		        	((MwBaseConstWidgetClass)parent->core.widget_class)->
		        		baseConst_class.traverse(parent , 
		        		MW_TRAVERSE_NEXT , time);
			break;
		case MW_TRAVERSE_PREV:
			if (XtIsSubclass(parent, mwBaseCompWidgetClass))
			      	((MwBaseCompWidgetClass)parent->core.widget_class)->
			      		baseComp_class.traverse(parent , 
			      		MW_TRAVERSE_PREV , time);
			else if (XtIsSubclass(parent, mwBaseConstWidgetClass))
		        	((MwBaseConstWidgetClass)parent->core.widget_class)->
		        		baseConst_class.traverse(parent , 
		        		MW_TRAVERSE_PREV , time);
			else ((MwBaseCompWidgetClass)cw->core.widget_class)->
		        		baseComp_class.traverse(w , 
		        		MW_TRAVERSE_LAST , time);
			break;
		case MW_TRAVERSE_NEXT:
			if (XtIsSubclass(parent, mwBaseCompWidgetClass))
			      	((MwBaseCompWidgetClass)parent->core.widget_class)->
			      		baseComp_class.traverse(parent , 
			      		MW_TRAVERSE_NEXT , time);
			else if (XtIsSubclass(parent, mwBaseConstWidgetClass))
		        	((MwBaseConstWidgetClass)parent->core.widget_class)->
		        		baseConst_class.traverse(parent , 
		        		MW_TRAVERSE_NEXT , time);
			else ((MwBaseCompWidgetClass)cw->core.widget_class)->
		        		baseComp_class.traverse(w , 
		        		MW_TRAVERSE_FIRST , time);
			break;
		case MW_TRAVERSE_ACTUAL:
			break;
		default: XtWarning("Unknown direction");
	    }
	}

	return cw->baseComp.current_focused;
}

static void TraverseTo(Widget w , Widget child , Time *time)
{
	MwBaseCompWidget cw = (MwBaseCompWidget) w;
	Widget parent = XtParent(w);
	Widget cld = cw->baseComp.current_focused;

	cw->baseComp.current_focused = child;

	if (cld != child)
	{
		if (cld)
		{
			if (XtIsSubclass(cld , mwBaseCompWidgetClass))
				((MwBaseCompWidgetClass)cld->core.widget_class)->
				baseComp_class.traverseOut(cld);
			else if (XtIsSubclass(cld , mwBaseConstWidgetClass))
				((MwBaseConstWidgetClass)cld->core.widget_class)->
				baseConst_class.traverseOut(cld);
		}
	}

	if (XtIsSubclass(parent, mwBaseCompWidgetClass))
		((MwBaseCompWidgetClass)parent->core.widget_class)->baseComp_class.
        		traverseTo(parent , w , time);
	else if (XtIsSubclass(parent, mwBaseConstWidgetClass))
		((MwBaseConstWidgetClass)parent->core.widget_class)->baseConst_class.
        		traverseTo(parent , w , time);
	
}

static void TraverseOut(Widget w)
{
	MwBaseCompWidget cw = (MwBaseCompWidget) w;
	Widget cld = cw->baseComp.current_focused;


	if (cld)
	{
		if (XtIsSubclass(cld , mwBaseCompWidgetClass))
			((MwBaseCompWidgetClass)cld->core.widget_class)->
			baseComp_class.traverseOut(cld);
		else if (XtIsSubclass(cld, mwBaseConstWidgetClass))
			((MwBaseConstWidgetClass)cld->core.widget_class)->
			baseConst_class.traverseOut(cld);
	}

	cw->baseComp.current_focused = NULL;
}


static Boolean AcceptFocus(Widget w , Time *time)
{
	MwBaseCompWidget cw = (MwBaseCompWidget) w;

	if (!XtIsRealized(w) || !XtIsSensitive(w) || !cw->core.visible ||
		!cw->core.ancestor_sensitive || cw->core.being_destroyed ||
		!XtIsManaged(w))
		return False;

	if (cw->baseComp.traverse)
	{
		if (cw->baseComp.current_focused)
		{
			return (((MwBaseCompWidgetClass)cw->core.widget_class)->
		        	baseComp_class.traverseInside(w , MW_TRAVERSE_ACTUAL , time) != NULL);
		}
		else
		{
			return (((MwBaseCompWidgetClass)cw->core.widget_class)->
		        	baseComp_class.traverseInside(w , 
		        	cw->baseComp.traverse_direction , time) != NULL);
		}
	}
	else
	{
		if (Xt_IsUp(w))
		{
			XSetInputFocus(XtDisplay(w), XtWindow(w), RevertToParent , *time);
			((MwBaseCompWidgetClass)cw->core.widget_class)
				->baseComp_class.highlightBorder(w);
		}
		return True;
	}
}

#define focus_detail(detail) (detail ==NotifyAncestor ?"NotifyAncestor":detail ==NotifyVirtual ?"NotifyVirtual":detail ==NotifyInferior ?"NotifyInferior":detail ==NotifyNonlinear ?"NotifyNonlinear":detail ==NotifyNonlinearVirtual ?"NotifyNonlinearVirtual":detail ==NotifyPointer ?"NotifyPointer":detail ==NotifyPointerRoot ?"NotifyPointerRoot":detail ==NotifyDetailNone ?"NotifyDetailNone":"???")

static void _FocusIn(Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
	MwBaseCompWidget cw = (MwBaseCompWidget) w;
	MwBaseCompWidgetClass wclass = (MwBaseCompWidgetClass) cw->core.widget_class;
	Time time = CurrentTime;

	if (event->xfocus.detail == NotifyNonlinear ||
		event->xfocus.detail == NotifyAncestor ||
		event->xfocus.detail == NotifyInferior)
	{
		if (cw->baseComp.traverse)
		{
			if (cw->baseComp.current_focused)
			{
				((MwBaseCompWidgetClass)cw->core.widget_class)->
		        		baseComp_class.
		        		traverseInside(w , MW_TRAVERSE_ACTUAL , &time);
			}
			else
			{
				((MwBaseCompWidgetClass)cw->core.widget_class)->
		        		baseComp_class.traverseInside(w , 
		        		cw->baseComp.traverse_direction , &time);
			}
		}
		else
		{
			wclass->baseComp_class.highlightBorder(w);
			cw->baseComp.focused = True;
		}
	}
}

static void _FocusOut(Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
	MwBaseCompWidget cw = (MwBaseCompWidget) w;
	MwBaseCompWidgetClass wclass = (MwBaseCompWidgetClass) cw->core.widget_class;

	if (cw->baseComp.focused)
	{
		wclass->baseComp_class.unhighlightBorder(w);
		cw->baseComp.focused = False;
	}

}

static void HighlightBorder(Widget w)
{
	MwBaseCompWidget cw = (MwBaseCompWidget) w;

	X_DrawSimpleRawFrame(XtDisplay(w) , XtWindow(w) , 0 , 0 , cw->core.width ,
		cw->core.height , cw->baseComp.bd_width , cw->baseComp.bd_color);
}

static void UnhighlightBorder(Widget w)
{
	MwBaseCompWidget cw = (MwBaseCompWidget) w;

	X_DrawSimpleRawFrame(XtDisplay(w) , XtWindow(w) , 0 , 0 , cw->core.width ,
		cw->core.height , cw->baseComp.bd_width , cw->core.background_pixel);
}

static void FocusCurrent(Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
	MwBaseCompWidget cw = (MwBaseCompWidget) w;
	Widget parent = XtParent(w);

	if (cw->baseComp.focused) return;

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
