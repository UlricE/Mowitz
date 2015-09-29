/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/extensions/shape.h>
#include <stdio.h>

#include "MwPopText.h"
#include "MwBaseConstP.h"
#include "MwBaseCompP.h"
#include <X11/ShellP.h>
#include "MwNws.h"
#include "MwTraverse.h"
#include "MwUtils.h"

#define offset(field) XtOffsetOf(MwBaseConstRec, baseConst.field)

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
	 XtOffsetOf(MwBaseConstRec , core.border_width),
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
static Widget Traverse(Widget, int, Time *);
static void TraverseTo(Widget, Widget, Time *);
static void TraverseOut(Widget);
static Widget TraverseInside(Widget, int, Time *);
static Boolean AcceptFocus(Widget, Time *);
static void  HighlightBorder(Widget);
static void  UnhighlightBorder(Widget);
static void  TraverseForward(Widget, XEvent *, String *, Cardinal *);
static void  TraverseBackward(Widget, XEvent *, String *, Cardinal *);
static void  FocusCurrent(Widget, XEvent *, String *, Cardinal *);
static void  ChangeManaged(Widget);

static void GetInternalDimension(Widget,Position *,Position *,Dimension *,Dimension *);
static void SetInternalDimension(Widget,Dimension,Dimension);

static void  ShowHelp(Widget, XEvent *, String *, Cardinal *);
static void  HideHelp(Widget, XEvent *, String *, Cardinal *);
static void  _ShowHelp(XtPointer, XtIntervalId *);
static void  _FocusIn(Widget, XEvent *, String *, Cardinal *);
static void  _FocusOut(Widget, XEvent *, String *, Cardinal *);

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

MwBaseConstClassRec mwBaseConstClassRec = {
/* core */
   {
    /* superclass            */ (WidgetClass) &constraintClassRec,
    /* class_name            */ "MwBaseConst",
    /* widget_size           */ sizeof(MwBaseConstRec),
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
/* constraint */
   {
    /* subresourses       */   NULL,
    /* subresource_count  */   0,
    /* constraint_size    */   sizeof(MwBaseConstConstraintsRec),
    /* initialize         */   NULL,
    /* destroy            */   NULL,
    /* set_values         */   NULL,
    /* extension          */   NULL
   },
/* baseConst */
   {
    /* get_internal_dimension  */ GetInternalDimension,
    /* set_internal_dimension  */ SetInternalDimension,
    /* traverse		       */ Traverse,
    /* traverseTo	       */ TraverseTo,
    /* traverseOut	       */ TraverseOut,
    /* traverseInside          */ TraverseInside,
    /* highlightBorder	       */ HighlightBorder,
    /* unhighlightBorder       */ UnhighlightBorder,
   },
};

WidgetClass mwBaseConstWidgetClass = (WidgetClass) &mwBaseConstClassRec;

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
	MwBaseConstWidgetClass c = (MwBaseConstWidgetClass) class;
	MwBaseConstWidgetClass super;
	static ConstraintClassExtensionRec extension_rec = {
		NULL, NULLQUARK, XtConstraintExtensionVersion,
		sizeof(ConstraintClassExtensionRec), NULL};
	ConstraintClassExtensionRec *ext;

  	ext = MwMalloc(sizeof(*ext));
	*ext = extension_rec;
	ext->next_extension = c->constraint_class.extension;
	c->constraint_class.extension = ext;

	if (class == mwBaseConstWidgetClass) return;
	super = (MwBaseConstWidgetClass)class->core_class.superclass;

	if (c->baseConst_class.get_internal_dimension == XtInheritGetInternalDimension)
		c->baseConst_class.get_internal_dimension =
			super->baseConst_class.get_internal_dimension;

	if (c->baseConst_class.set_internal_dimension == XtInheritSetInternalDimension)
		c->baseConst_class.set_internal_dimension =
			super->baseConst_class.set_internal_dimension;

	if (c->baseConst_class.traverse == XtInheritTraverse)
		c->baseConst_class.traverse =
			super->baseConst_class.traverse;

	if (c->baseConst_class.traverseTo == XtInheritTraverseTo)
		c->baseConst_class.traverseTo =
			super->baseConst_class.traverseTo;

	if (c->baseConst_class.traverseOut == XtInheritTraverseOut)
		c->baseConst_class.traverseOut =
			super->baseConst_class.traverseOut;

	if (c->baseConst_class.traverseInside == XtInheritTraverseInside)
		c->baseConst_class.traverseInside =
			super->baseConst_class.traverseInside;

	if (c->baseConst_class.highlightBorder == XtInheritHighlightBorder)
		c->baseConst_class.highlightBorder =
			super->baseConst_class.highlightBorder;

	if (c->baseConst_class.unhighlightBorder == XtInheritUnhighlightBorder)
		c->baseConst_class.unhighlightBorder =
			super->baseConst_class.unhighlightBorder;

}

static void Initialize(Widget req_widget, Widget new_widget,
		ArgList args, Cardinal *num_args)
{
	MwBaseConstWidget nw = (MwBaseConstWidget) new_widget;
	XColor	dark,light,bg;
	Display *dpy=XtDisplay(new_widget);
	XGCValues gc_res;
	XtGCMask  gc_mask;

	bg.pixel = nw->baseConst.box_color;
	
	XQueryColor(dpy,DefaultColormap(dpy,DefaultScreen(dpy)),&bg);
	
	LightColor2(new_widget, light, nw->baseConst.top_shadow_contrast);
	DarkColor2(new_widget, dark, nw->baseConst.bot_shadow_contrast);

	nw->baseConst.light = light.pixel;
	nw->baseConst.dark = dark.pixel;

	gc_res.foreground = nw->baseConst.box_color;
	gc_res.font = nw->baseConst.font->fid;

	gc_mask = GCForeground | GCFont;

	nw->baseConst.gc = XCreateGC(dpy,DefaultRootWindow(dpy),
			gc_mask, &gc_res);

	if (nw->baseConst.help_text)
	{
		nw->baseConst.help_text = MwStrdup(nw->baseConst.help_text);
		nw->baseConst.hlp = XtVaCreatePopupShell("___help" , mwPopTextWidgetClass ,
				new_widget , XtNtext , nw->baseConst.help_text,NULL);
	}

	if (nw->baseConst.label)
	{
		nw->baseConst.label = MwStrdup(nw->baseConst.label);
	}

	if (nw->baseConst.box_type == XtCshadow_box) 
	{
		nw->baseConst.have_shape = True;
	}
	else 
	{
		nw->baseConst.have_shape = False;
	}
	nw->baseConst.timer = (XtIntervalId) 0;
	nw->baseConst.current_focused = NULL;
	nw->baseConst.focused = False;
	nw->baseConst.traverse_direction = MW_TRAVERSE_FIRST;
}

static void Realize(Widget w, Mask *valueMask, XSetWindowAttributes *attributes)
{
	MwBaseConstWidget cw = (MwBaseConstWidget) w;

	if ((attributes->cursor = cw->baseConst.cursor) != None)
		*valueMask |= CWCursor;

	compositeClassRec.core_class.realize(w,valueMask,attributes);

	if (cw->baseConst.box_type == XtCshadow_box)
	{
		Region region;
		XPoint points[8];

		points[0].x = 0;
		points[0].y = 0;
		points[1].x = cw->core.width - 2 * cw->baseConst.box_width;
		points[1].y = 0;
		points[2].x = cw->core.width - 2 * cw->baseConst.box_width;
		points[2].y = 2 * cw->baseConst.box_width;
		points[3].x = cw->core.width;
		points[3].y = 2 * cw->baseConst.box_width;
		points[4].x = cw->core.width;
		points[4].y = cw->core.height;
		points[5].x = 2 * cw->baseConst.box_width;
		points[5].y = cw->core.height;
		points[6].x = 2 * cw->baseConst.box_width;
		points[6].y = cw->core.height - 2 * cw->baseConst.box_width;
		points[7].x = 0;
		points[7].y = cw->core.height - 2 * cw->baseConst.box_width;

		region = XPolygonRegion(points , XtNumber(points) , EvenOddRule);

		XShapeCombineRegion(XtDisplay(w) , XtWindow(w) , ShapeBounding ,
			0 , 0 , region , ShapeSet);

		XDestroyRegion(region);
	}
}

static void Destroy(Widget w)
{
	MwBaseConstWidget cw = (MwBaseConstWidget) w;

        if (cw->baseConst.help_text)
                MwFree(cw->baseConst.help_text);

        XFreeGC(XtDisplay(w),cw->baseConst.gc);
}

static void Redisplay(Widget w, XEvent *event, Region region)
{
	MwBaseConstWidget cw = (MwBaseConstWidget) w;
	Display *dpy = XtDisplay(w);
	Window win = XtWindow (w);
	cw->core.border_width=0;
	if (cw->baseConst.label &&
		(cw->baseConst.box_type == XtCno_box ||
		 cw->baseConst.box_type == XtCsimple_box ||
		 cw->baseConst.box_type == XtCup_box ||
		 cw->baseConst.box_type == XtCdown_box ||
		 cw->baseConst.box_type == XtCshadow_box ))
		 
	{
		Position x;

		switch(cw->baseConst.justify)
		{
			case XtCleft:
				x = 4 * cw->baseConst.box_width + cw->baseConst.bd_width;
				break;

			case XtCright:
				x = cw->core.width - cw->baseConst.bd_width -
					XTextWidth(cw->baseConst.font , 
					cw->baseConst.label , strlen(cw->baseConst.label))
					- 4 * cw->baseConst.box_width -
					(cw->baseConst.box_type == XtCshadow_box ?
					2 * cw->baseConst.box_width : 0);
				break;

			case XtCcenter:
			default:
				x = (cw->core.width - 2 * cw->baseConst.bd_width -
					XTextWidth(cw->baseConst.font , 
					cw->baseConst.label , 
					strlen(cw->baseConst.label))) / 2;
				break;

				
		}
		XSetForeground(dpy , cw->baseConst.gc , cw->baseConst.box_color);
		XSetBackground(dpy , cw->baseConst.gc , cw->baseConst.box_color);
		XFillRectangle(dpy , win , cw->baseConst.gc ,
			cw->baseConst.bd_width , cw->baseConst.bd_width ,
			cw->core.width - cw->baseConst.bd_width ,
			cw->baseConst.font->max_bounds.descent +
			cw->baseConst.font->max_bounds.ascent + 
			2 * cw->baseConst.box_width );
        
		XSetForeground(dpy , cw->baseConst.gc , cw->baseConst.foreground);
		XDrawImageString(dpy , win , cw->baseConst.gc , x ,
			cw->baseConst.font->max_bounds.ascent + 
			cw->baseConst.box_width + cw->baseConst.bd_width,
			cw->baseConst.label , strlen(cw->baseConst.label));
	}
	switch (cw->baseConst.box_type)
	{
		case XtCno_box:
		    break;

		case XtCsimple_box:
		    X_DrawSimpleRawFrame(dpy , win , cw->baseConst.bd_width ,
		        cw->baseConst.bd_width ,
		        cw->core.width - 2 * cw->baseConst.bd_width,
			cw->core.height - 2 * cw->baseConst.bd_width,
			cw->baseConst.box_width , 
			cw->baseConst.box_color);
		    break;

		case XtCup_box:
		    X_DrawSimple3DFrame(dpy , win , cw->baseConst.bd_width ,
			cw->baseConst.bd_width ,
			cw->core.width - 2 * cw->baseConst.bd_width,
			cw->core.height - 2 * cw->baseConst.bd_width,
			cw->baseConst.box_width ,
			cw->baseConst.light , cw->baseConst.dark);
		    break;

		case XtCdown_box:
		    X_DrawSimple3DFrame(dpy , win , cw->baseConst.bd_width ,
		        cw->baseConst.bd_width ,
		        cw->core.width - 2 * cw->baseConst.bd_width,
			cw->core.height - 2 * cw->baseConst.bd_width,
			cw->baseConst.box_width ,
			cw->baseConst.dark , cw->baseConst.light);
		    break;

		case XtCframein_box:
		   if (cw->baseConst.label )
		   {
			Position x;
	    		char *p = MwMalloc(strlen(cw->baseConst.label)+3);
		    	sprintf(p , " %s " ,cw->baseConst.label);

			switch(cw->baseConst.justify)
			{
				case XtCleft:
					x = 4 * cw->baseConst.box_width +
						cw->baseConst.bd_width;
					break;

				case XtCright:
					x = cw->core.width - cw->baseConst.bd_width -
						XTextWidth(cw->baseConst.font , 
							p , strlen(p)) - 
						4 * cw->baseConst.box_width;
					break;

				case XtCcenter:
				default:
					x = (cw->core.width - 2 * cw->baseConst.bd_width
						- XTextWidth(cw->baseConst.font , 
							p , strlen(p))) / 2;
					break;
				
			}

			X_DrawSimple3DFrame(dpy , win , cw->baseConst.bd_width ,
				(cw->baseConst.box_width +
				cw->baseConst.font->max_bounds.descent +
				cw->baseConst.font->max_bounds.ascent) / 2 + 
				cw->baseConst.bd_width,
				cw->core.width - 2 * cw->baseConst.bd_width,
				cw->core.height - (cw->baseConst.box_width +
				cw->baseConst.font->max_bounds.descent +
				cw->baseConst.font->max_bounds.ascent) / 2 
				- 2 * cw->baseConst.bd_width, 
				cw->baseConst.box_width / 2 ,
				cw->baseConst.dark , cw->baseConst.light);

			X_DrawSimple3DFrame(dpy , win , 
				cw->baseConst.box_width / 2 + cw->baseConst.bd_width,
				cw->baseConst.box_width / 2 +
				(cw->baseConst.box_width + 
				cw->baseConst.font->max_bounds.descent +
				cw->baseConst.font->max_bounds.ascent) / 2 +
				cw->baseConst.bd_width,
				cw->core.width - 2 * (cw->baseConst.box_width / 2)
				- 2 * cw->baseConst.bd_width,
				cw->core.height - 2 * (cw->baseConst.box_width / 2) - 
				(cw->baseConst.box_width +
				cw->baseConst.font->max_bounds.descent +
				cw->baseConst.font->max_bounds.ascent) / 2 
				- 2 * cw->baseConst.bd_width,
				cw->baseConst.box_width / 2 ,
				cw->baseConst.light , cw->baseConst.dark);

			XSetForeground(dpy , cw->baseConst.gc , cw->baseConst.foreground);
			XSetBackground(dpy , cw->baseConst.gc , cw->core.background_pixel);
			XDrawImageString(dpy , win , cw->baseConst.gc ,
				x ,
				cw->baseConst.box_width +
				cw->baseConst.font->max_bounds.ascent + 
				cw->baseConst.bd_width,
				p , strlen(p));

			MwFree(p);
		    }
		    else
		    {
			X_DrawSimple3DFrame(dpy , win , cw->baseConst.bd_width ,
				cw->baseConst.bd_width ,
				cw->core.width - 2 * cw->baseConst.bd_width,
				cw->core.height , cw->baseConst.box_width / 2 
				- 2 * cw->baseConst.bd_width,
				cw->baseConst.dark , cw->baseConst.light);
			X_DrawSimple3DFrame(dpy , win ,
				cw->baseConst.box_width / 2 + cw->baseConst.bd_width,
				cw->baseConst.box_width / 2 + cw->baseConst.bd_width,
				cw->core.width - 2 * (cw->baseConst.box_width / 2)
				- 2 * cw->baseConst.bd_width,
				cw->core.height - 2 * (cw->baseConst.box_width / 2)
				- 2 * cw->baseConst.bd_width,
				cw->baseConst.box_width / 2,
				cw->baseConst.light , cw->baseConst.dark);
		    }
		    break;

		case XtCframeout_box:
		   if (cw->baseConst.label )
		   {
			Position x;
	    		char *p = MwMalloc(strlen(cw->baseConst.label)+3);
		    	sprintf(p , " %s " ,cw->baseConst.label);

			switch(cw->baseConst.justify)
			{
				case XtCleft:
					x = 4 * cw->baseConst.box_width +
						cw->baseConst.bd_width;
					break;

				case XtCright:
					x = cw->core.width - cw->baseConst.bd_width -
						XTextWidth(cw->baseConst.font , 
							p , strlen(p)) - 
						4 * cw->baseConst.box_width;
					break;

				case XtCcenter:
				default:
					x = (cw->core.width - 2 * cw->baseConst.bd_width - 
						XTextWidth(cw->baseConst.font , 
							p , strlen(p))) / 2;
					break;
				
			}


			X_DrawSimple3DFrame(dpy , win , cw->baseConst.bd_width ,
				(cw->baseConst.box_width +
				cw->baseConst.font->max_bounds.descent +
				cw->baseConst.font->max_bounds.ascent) / 2 +
				cw->baseConst.bd_width,
				cw->core.width - 2 * cw->baseConst.bd_width,
				cw->core.height - (cw->baseConst.box_width +
				cw->baseConst.font->max_bounds.descent +				
				cw->baseConst.font->max_bounds.ascent) / 2 
				- 2 * cw->baseConst.bd_width,
				cw->baseConst.box_width / 2 ,
				cw->baseConst.light , cw->baseConst.dark);
			X_DrawSimple3DFrame(dpy , win , 
				cw->baseConst.box_width / 2 + cw->baseConst.bd_width,
				cw->baseConst.box_width / 2 +
				(cw->baseConst.box_width +
				cw->baseConst.font->max_bounds.descent +
				cw->baseConst.font->max_bounds.ascent) / 2 +
				cw->baseConst.bd_width,
				cw->core.width - 2 * (cw->baseConst.box_width / 2) 
				- 2 * cw->baseConst.bd_width,
				cw->core.height - 2 * (cw->baseConst.box_width / 2) -
				(cw->baseConst.box_width +
				cw->baseConst.font->max_bounds.descent +
				cw->baseConst.font->max_bounds.ascent) / 2
				- 2 * cw->baseConst.bd_width,
				cw->baseConst.box_width / 2 ,
				cw->baseConst.dark , cw->baseConst.light);

			XSetForeground(dpy , cw->baseConst.gc , cw->baseConst.foreground);
			XSetBackground(dpy , cw->baseConst.gc , cw->core.background_pixel);
			XDrawImageString(dpy , win , cw->baseConst.gc ,
				x ,
				cw->baseConst.box_width +
				cw->baseConst.font->max_bounds.ascent +
				cw->baseConst.bd_width,
				p , strlen(p));

			MwFree(p);
		    }
		    else
		    {
			X_DrawSimple3DFrame(dpy , win , cw->baseConst.bd_width , 
				cw->baseConst.bd_width , 
				cw->core.width - 2 * cw->baseConst.bd_width,
				cw->core.height - 2 * cw->baseConst.bd_width,
				cw->baseConst.box_width / 2 ,
				cw->baseConst.light , cw->baseConst.dark);
			X_DrawSimple3DFrame(dpy , win ,
				cw->baseConst.box_width / 2 + cw->baseConst.bd_width,
				cw->baseConst.box_width / 2 + cw->baseConst.bd_width , 
				cw->core.width - 2 * (cw->baseConst.box_width / 2)
				- 2 * cw->baseConst.bd_width,
				cw->core.height - 2 * (cw->baseConst.box_width / 2)
				- 2 * cw->baseConst.bd_width, 
				cw->baseConst.box_width / 2 ,
				cw->baseConst.dark , cw->baseConst.light);
		    }
		    break;

		case XtCshadow_box:
		   {
			XPoint points[6];


			points[0].x = 2 * cw->baseConst.box_width + 
				cw->baseConst.bd_width;
			points[0].y = cw->core.height - cw->baseConst.bd_width;
			points[1].x = 2 * cw->baseConst.box_width + 
				cw->baseConst.bd_width;
			points[1].y = cw->core.height - 2 * cw->baseConst.box_width - 
				cw->baseConst.bd_width;
			points[2].x = cw->core.width - 2 * cw->baseConst.box_width -
				cw->baseConst.bd_width;
			points[2].y = cw->core.height - 2 * cw->baseConst.box_width -
				cw->baseConst.bd_width;
			points[3].x = cw->core.width - 2 * cw->baseConst.box_width -
				cw->baseConst.bd_width;
			points[3].y = 2 * cw->baseConst.box_width + 
				cw->baseConst.bd_width;
			points[4].x = cw->core.width - cw->baseConst.bd_width;
			points[4].y = 2 * cw->baseConst.box_width + 
				cw->baseConst.bd_width;
			points[5].x = cw->core.width - cw->baseConst.bd_width;
			points[5].y = cw->core.height - cw->baseConst.bd_width;

			X_DrawSimpleRawFrame (dpy , win , cw->baseConst.bd_width ,
				cw->baseConst.bd_width ,
				cw->core.width - (2 * cw->baseConst.box_width) 
				- 2 * cw->baseConst.bd_width,
				cw->core.height - (2 * cw->baseConst.box_width) 
				- 2 * cw->baseConst.bd_width,
				cw->baseConst.box_width , cw->baseConst.box_color );

			XSetForeground(dpy , cw->baseConst.gc , cw->baseConst.dark);

			XFillPolygon(dpy , win , cw->baseConst.gc , points , 
				XtNumber(points),Nonconvex , CoordModeOrigin);
			
			break;
		    }
	}

        if (cw->baseConst.focused) 
               ((MwBaseConstWidgetClass)w->core.widget_class)
               		->baseConst_class.highlightBorder(w);

	if (!XtIsSensitive(w)) Xt_SetInsensitive(w);
}

static void Resize(Widget w)
{
	MwBaseConstWidget cw = (MwBaseConstWidget) w;

	if (!XtIsRealized(w)) return;

	if (cw->baseConst.have_shape)
	{
		if (cw->baseConst.box_type == XtCshadow_box)
		{
			Region region;
			XPoint points[8];

			points[0].x = 0;
			points[0].y = 0;
			points[1].x = cw->core.width - 2 * cw->baseConst.box_width;
			points[1].y = 0;
			points[2].x = cw->core.width - 2 * cw->baseConst.box_width;
			points[2].y = 2 * cw->baseConst.box_width;
			points[3].x = cw->core.width;
			points[3].y = 2 * cw->baseConst.box_width;
			points[4].x = cw->core.width;
			points[4].y = cw->core.height;
			points[5].x = 2 * cw->baseConst.box_width;
			points[5].y = cw->core.height;
			points[6].x = 2 * cw->baseConst.box_width;
			points[6].y = cw->core.height - 2 * cw->baseConst.box_width;
			points[7].x = 0;
			points[7].y = cw->core.height - 2 * cw->baseConst.box_width;
	
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


#define WidgetValuesDiffer(w1,w2,component) (w1 -> baseConst.component != \
                                             w2 -> baseConst.component)

static Boolean SetValues(Widget current, Widget request, Widget new_widget,
		ArgList args, Cardinal *num_args)
{
	MwBaseConstWidget cw = (MwBaseConstWidget) current;
	MwBaseConstWidget nw = (MwBaseConstWidget) new_widget;
	Widget w = (Widget) new_widget;
	Boolean redraw=False;
	
	if WidgetValuesDiffer( cw , nw , cursor)
	{
		XDefineCursor(XtDisplay(current),XtWindow(current),nw->baseConst.cursor);
	}

	if (WidgetValuesDiffer( cw , nw , box_type) ||
		WidgetValuesDiffer( cw , nw , box_width))
	{
		if (cw->baseConst.box_type == XtCshadow_box && 
			nw->baseConst.box_type != XtCshadow_box)
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
		if (nw->baseConst.box_type == XtCshadow_box &&
			cw->baseConst.box_type != XtCshadow_box)
		{
			Region region;
			XPoint points[8];

			points[0].x = 0;
			points[0].y = 0;
			points[1].x = cw->core.width - 2 * cw->baseConst.box_width;
			points[1].y = 0;
			points[2].x = cw->core.width - 2 * cw->baseConst.box_width;
			points[2].y = 2 * cw->baseConst.box_width;
			points[3].x = cw->core.width;
			points[3].y = 2 * cw->baseConst.box_width;
			points[4].x = cw->core.width;
			points[4].y = cw->core.height;
			points[5].x = 2 * cw->baseConst.box_width;
			points[5].y = cw->core.height;
			points[6].x = 2 * cw->baseConst.box_width;
			points[6].y = cw->core.height - 2 * cw->baseConst.box_width;
			points[7].x = 0;
			points[7].y = cw->core.height - 2 * cw->baseConst.box_width;

			region = XPolygonRegion(points , XtNumber(points) , EvenOddRule);

			XShapeCombineRegion(XtDisplay(w) , XtWindow(w) , ShapeBounding ,
				0 , 0 , region , ShapeSet);

			XDestroyRegion(region);

			nw->baseConst.have_shape=True;

		}
		redraw = True;
	}

	if WidgetValuesDiffer( cw , nw , help_text)
	{
		if (cw->baseConst.help_text)
		{
			XtDestroyWidget(nw->baseConst.hlp);
			MwFree(cw->baseConst.help_text);
			cw->baseConst.help_text = NULL;
		}
		if (nw->baseConst.help_text)
		{
			nw->baseConst.help_text = MwStrdup(nw->baseConst.help_text);
			nw->baseConst.hlp = XtVaCreatePopupShell("___help" , mwPopTextWidgetClass ,
				new_widget , XtNtext , nw->baseConst.help_text,NULL);
		}
	}

	if WidgetValuesDiffer( cw , nw , label )
	{
		if (cw->baseConst.label)
		{
			MwFree(cw->baseConst.label);
			cw->baseConst.label = NULL;
		}
		if (nw->baseConst.label) 
			nw->baseConst.label = MwStrdup(nw->baseConst.label);

		redraw = True;
	}

	if WidgetValuesDiffer( cw , nw , font )
	{
		XSetFont(XtDisplay(w) , nw->baseConst.gc , nw->baseConst.font->fid);
		
		redraw = True;
	}

	
	return redraw;
}

static void ChangeManaged(Widget w)
{
	MwBaseConstWidget cw = (MwBaseConstWidget) w;
	Widget *child;

	if (constraintClassRec.composite_class.change_managed)
		constraintClassRec.composite_class.change_managed(w);

	if (cw->baseConst.current_focused)
	{
		ForAllChildren(cw , child)
		{
			if (!XtIsManaged(*child) &&
				*child == cw->baseConst.current_focused)
			{
				((MwBaseConstWidgetClass)w->core.widget_class)
					->baseConst_class.traverseOut(w);				
			}
		}
	}
}

static void GetInternalDimension(Widget w, Position *x, Position *y,
		Dimension *width, Dimension *height)
{
	MwBaseConstWidget cw = (MwBaseConstWidget) w;
	
	switch (cw->baseConst.box_type)
	{
		case XtCno_box:
			*x = cw->baseConst.bd_width;
			*width = cw->core.width - 2 * cw->baseConst.bd_width;
			*y = cw->baseConst.bd_width;
			*height = cw->core.height - 2 * cw->baseConst.bd_width;
			break;

		case XtCsimple_box:
		case XtCup_box:
		case XtCdown_box:
			*x = cw->baseConst.box_width + cw->baseConst.bd_width;
			*width = cw->core.width - 2 * cw->baseConst.box_width
				 - 2 * cw->baseConst.bd_width;
			*y = cw->baseConst.box_width + cw->baseConst.bd_width;
			*height = cw->core.height - 2 * cw->baseConst.box_width
				 - 2 * cw->baseConst.bd_width;
			break;

		case XtCframein_box:
		case XtCframeout_box:
			*x = 2 * (cw->baseConst.box_width / 2) + cw->baseConst.bd_width;
			*y = 2 * (cw->baseConst.box_width / 2) + cw->baseConst.bd_width;
			*width = cw->core.width - 4 * (cw->baseConst.box_width /2)
				 - 2 * cw->baseConst.bd_width;
			*height = cw->core.height - 4 * (cw->baseConst.box_width /2)
				 - 2 * cw->baseConst.bd_width;
			break;

		case XtCshadow_box:
			*x = cw->baseConst.box_width + cw->baseConst.bd_width;
			*y = cw->baseConst.box_width + cw->baseConst.bd_width;
			*width = cw->core.width - 4 * cw->baseConst.box_width
				 - 2 * cw->baseConst.bd_width;
			*height = cw->core.height - 4 * cw->baseConst.box_width
				 - 2 * cw->baseConst.bd_width;
			break;
	}

	if (cw->baseConst.label)
	{
		*y += cw->baseConst.font->max_bounds.descent +
			cw->baseConst.font->max_bounds.ascent;

		*height -= cw->baseConst.font->max_bounds.descent +
			cw->baseConst.font->max_bounds.ascent;
	}
}

static void SetInternalDimension(Widget w, Dimension width, Dimension height)
{
	MwBaseConstWidget cw = (MwBaseConstWidget) w;

	switch (cw->baseConst.box_type)
	{
		case XtCno_box:
			cw->core.width = width + 2 * cw->baseConst.bd_width;
			cw->core.height = height + 2 * cw->baseConst.bd_width;
			break;
			
		case XtCsimple_box:
		case XtCup_box:
		case XtCdown_box:
			cw->core.width = width + 2 * cw->baseConst.box_width +
				 2 * cw->baseConst.bd_width;
			cw->core.height = height + 2 * cw->baseConst.box_width +
				 2 * cw->baseConst.bd_width;
			break;
			
		case XtCframein_box:
		case XtCframeout_box:	
			cw->core.width = width + 4 * (cw->baseConst.box_width /2) + 
				2 * cw->baseConst.bd_width;
			cw->core.height = height + 4 * (cw->baseConst.box_width /2) +
				 2 * cw->baseConst.bd_width;
			break;
			
		case XtCshadow_box:
			cw->core.width = width + 4 * cw->baseConst.box_width +
				2 * cw->baseConst.bd_width;
			cw->core.height = height + 4 * cw->baseConst.box_width +
				 2 * cw->baseConst.bd_width;
			break;
	}		
	if (cw->baseConst.label)
	{
		cw->core.height = cw->core.height + (2 * cw->baseConst.box_width +
			cw->baseConst.font->max_bounds.descent +
			cw->baseConst.font->max_bounds.ascent);	
	}
}


static void ShowHelp(Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
	MwBaseConstWidget cw = (MwBaseConstWidget) w;

	if (cw->baseConst.help_text)
	{
		cw->baseConst.timer = XtAppAddTimeOut(XtWidgetToApplicationContext(
					(Widget)w),cw->baseConst.help_show_delay ,
					_ShowHelp , w);
	}
}

static void _ShowHelp(XtPointer client_data, XtIntervalId *timer)
{
	MwBaseConstWidget cw = (MwBaseConstWidget) client_data;
	Position x,y;
	Display *dpy = XtDisplay((Widget) cw);
	Dimension width,height,
		dwidth = DisplayWidth(dpy,DefaultScreen(dpy)) ,
		dheight = DisplayHeight(dpy,DefaultScreen(dpy)) ;

	cw->baseConst.timer = (XtIntervalId) 0;

	XtVaGetValues(cw->baseConst.hlp , XtNwidth , &width , XtNheight , &height , NULL);
	
	XtTranslateCoords((Widget) cw , cw->core.width / 2 ,
		cw->core.height , &x ,&y);

	if ( (x + width) > dwidth ) x = x - width;
	
	if ( (y + height + 5) > dheight ) y = y - cw->core.height - 5 - height;
	else y = y + 5;
	
	XtVaSetValues(cw->baseConst.hlp,XtNx,x,XtNy,y,NULL);

	XtPopup(cw->baseConst.hlp,XtGrabNone);
}

static void HideHelp(Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
	MwBaseConstWidget cw = (MwBaseConstWidget) w;

	if (cw->baseConst.help_text)
	{
		XtPopdown(cw->baseConst.hlp); 
		if (cw->baseConst.timer)
			XtRemoveTimeOut(cw->baseConst.timer);
		cw->baseConst.timer = (XtIntervalId) 0;
	}
}

static Widget TraverseInside(Widget w , int where , Time *time)
{
	MwBaseConstWidget cw = (MwBaseConstWidget) w;
	int i;

	switch (where)
	{
		case MW_TRAVERSE_FIRST:
			cw->baseConst.current_focused = NULL;
			for(i = 0  ; i < cw->composite.num_children ; i++)
			{
			    	MwSetTraverseDirection(cw->composite.children[i] , where);
				if (XtCallAcceptFocus(cw->composite.children[i] , time))
				{
					cw->baseConst.current_focused = 
						cw->composite.children[i];
					break;
				}
			}
			break;
		case MW_TRAVERSE_LAST:
			cw->baseConst.current_focused = NULL;
			for(i = cw->composite.num_children - 1 ; i >= 0 ; i--)
			{
			    	MwSetTraverseDirection(cw->composite.children[i] , where);
				if (XtCallAcceptFocus(cw->composite.children[i] , time))
				{
					cw->baseConst.current_focused = 
						cw->composite.children[i];
					break;
				}
			}
			break;
		case MW_TRAVERSE_PREV:
		    if (cw->composite.num_children)
		    {
			for(i = 0 ;  (i < cw->composite.num_children) &&
				(cw->baseConst.current_focused != 
				cw->composite.children[i]) ; i++);

			if (!(i < cw->composite.num_children) ||
				!cw->baseConst.current_focused)
			{
			    cw->baseConst.current_focused = NULL;
			    for(i = cw->composite.num_children - 1  ; i >= 0 ; i--)
			    {
			    	MwSetTraverseDirection(cw->composite.children[i] , where);
				if (XtCallAcceptFocus(cw->composite.children[i] , time))
				{
					cw->baseConst.current_focused = 
						cw->composite.children[i];
					break;
				}
			    }	
			}
			else
			{
			    cw->baseConst.current_focused = NULL;
			    for( i = i - 1 ; i >= 0 ; i--)
			    {
			    	MwSetTraverseDirection(cw->composite.children[i] , where);
				if (XtCallAcceptFocus(cw->composite.children[i] , time))
				{
					cw->baseConst.current_focused = 
						cw->composite.children[i];
					break;
				}
			    }
			}
		    }
		    else
		    {
		    	cw->baseConst.current_focused = NULL;
		    }
		    break;
		case MW_TRAVERSE_NEXT:
		    if (cw->composite.num_children)
		    {
			for(i = 0 ;  (i < cw->composite.num_children) &&
				(cw->baseConst.current_focused != 
				cw->composite.children[i])  ; i++);

			if (!(i < cw->composite.num_children) ||
				!cw->baseConst.current_focused)
			{
			    cw->baseConst.current_focused = NULL;
			    for(i = 0 ; i < cw->composite.num_children ; i++)
			    {
			    	MwSetTraverseDirection(cw->composite.children[i] , where);
				if (XtCallAcceptFocus(cw->composite.children[i] , time))
				{
					cw->baseConst.current_focused = 
						cw->composite.children[i];
					break;
				}
			    }			
			}
			else
			{
			    cw->baseConst.current_focused = NULL;
			    for ( i = i + 1 ; i < cw->composite.num_children ; i++)
			    {
			    	MwSetTraverseDirection(cw->composite.children[i] , where);
				if (XtCallAcceptFocus(cw->composite.children[i] , time))
				{
					cw->baseConst.current_focused = 
						cw->composite.children[i];
					break;
				}
			    }
			}
		    }
		    else
		    {
		    	cw->baseConst.current_focused = NULL;
		    }
		    break;
		case MW_TRAVERSE_ACTUAL:
		    if (cw->composite.num_children)
		    {
			for(i = 0 ;  i < cw->composite.num_children &&
				cw->baseConst.current_focused != 
				cw->composite.children[i]  ; i++);

			if (i < cw->composite.num_children &&
				cw->baseConst.current_focused)
			{
			    	MwSetTraverseDirection(cw->composite.children[i] , where);
				XtCallAcceptFocus(cw->composite.children[i] , time);
			}
		    }
		    else
		    {
		    	cw->baseConst.current_focused = NULL;		    	
		    }
		    break;
		default: XtWarning("Unknown direction");
	}

	return cw->baseConst.current_focused;		
}

static Widget Traverse(Widget w , int where , Time *time)
{
	MwBaseConstWidget cw = (MwBaseConstWidget) w;
	Widget parent = XtParent(w);
	Widget cld;

	cld = ((MwBaseConstWidgetClass)w->core.widget_class)->
			baseConst_class.traverseInside(w , where , time);

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
			else ((MwBaseConstWidgetClass)cw->core.widget_class)->
		        		baseConst_class.traverse(w , 
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
			else ((MwBaseConstWidgetClass)cw->core.widget_class)->
		        		baseConst_class.traverse(w , 
		        		MW_TRAVERSE_FIRST , time);
			break;
		case MW_TRAVERSE_ACTUAL:
			break;
		default: XtWarning("Unknown direction");
	    }
	}

	return cw->baseConst.current_focused;
}

static void TraverseTo(Widget w , Widget child , Time *time)
{
	MwBaseConstWidget cw = (MwBaseConstWidget) w;
	Widget parent = XtParent(w);
	Widget cld = cw->baseConst.current_focused;

	cw->baseConst.current_focused = child;

	if (cld != child)
	{
		if (cld)
		{
			if (XtIsSubclass(cld , mwBaseCompWidgetClass))
				((MwBaseCompWidgetClass)cld->core.widget_class)->
				baseComp_class.traverseOut(cld);
			else if (XtIsSubclass(cld, mwBaseConstWidgetClass))
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
	MwBaseConstWidget cw = (MwBaseConstWidget) w;
	Widget cld = cw->baseConst.current_focused;


	if (cld)
	{
		if (XtIsSubclass(cld , mwBaseCompWidgetClass))
			((MwBaseCompWidgetClass)cld->core.widget_class)->
			baseComp_class.traverseOut(cld);
		else if (XtIsSubclass(cld, mwBaseConstWidgetClass))
			((MwBaseConstWidgetClass)cld->core.widget_class)->
			baseConst_class.traverseOut(cld);
	}

	cw->baseConst.current_focused = NULL;
}


static Boolean AcceptFocus(Widget w , Time *time)
{
	MwBaseConstWidget cw = (MwBaseConstWidget) w;

	if (!XtIsRealized(w) || !XtIsSensitive(w) || !cw->core.visible ||
		!cw->core.ancestor_sensitive || cw->core.being_destroyed ||
		!XtIsManaged(w))
		return False;

	if (cw->baseConst.traverse)
	{
		if (cw->baseConst.current_focused)
		{
			return (((MwBaseConstWidgetClass)cw->core.widget_class)->
		        	baseConst_class.traverseInside(w , MW_TRAVERSE_ACTUAL , time) != NULL);
		}
		else
		{
			return (((MwBaseConstWidgetClass)cw->core.widget_class)->
		        	baseConst_class.traverseInside(w , 
		        	cw->baseConst.traverse_direction , time) != NULL);
		}
	}
	else
	{
		if (Xt_IsUp(w))
		{
			XSetInputFocus(XtDisplay(w), XtWindow(w), RevertToParent , *time);
			((MwBaseConstWidgetClass)cw->core.widget_class)
				->baseConst_class.highlightBorder(w);
		}

		return True;
	}
}

#define focus_detail(detail) (detail ==NotifyAncestor ?"NotifyAncestor":detail ==NotifyVirtual ?"NotifyVirtual":detail ==NotifyInferior ?"NotifyInferior":detail ==NotifyNonlinear ?"NotifyNonlinear":detail ==NotifyNonlinearVirtual ?"NotifyNonlinearVirtual":detail ==NotifyPointer ?"NotifyPointer":detail ==NotifyPointerRoot ?"NotifyPointerRoot":detail ==NotifyDetailNone ?"NotifyDetailNone":"???")

static void _FocusIn(Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
	MwBaseConstWidget cw = (MwBaseConstWidget) w;
	MwBaseConstWidgetClass wclass = (MwBaseConstWidgetClass) cw->core.widget_class;
	Time time = CurrentTime;

	if (event->xfocus.detail == NotifyNonlinear ||
		event->xfocus.detail == NotifyAncestor ||
		event->xfocus.detail == NotifyInferior)
	{
		if (cw->baseConst.traverse)
		{
			if (cw->baseConst.current_focused)
			{
				((MwBaseConstWidgetClass)cw->core.widget_class)->
		        		baseConst_class.
		        		traverseInside(w , MW_TRAVERSE_ACTUAL , &time);
			}
			else
			{
				((MwBaseConstWidgetClass)cw->core.widget_class)->
		        		baseConst_class.traverseInside(w , 
		        		cw->baseConst.traverse_direction , &time);
			}
		}
		else
		{
			wclass->baseConst_class.highlightBorder(w);
			cw->baseConst.focused = True;
		}
	}
}

static void _FocusOut(Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
	MwBaseConstWidget cw = (MwBaseConstWidget) w;
	MwBaseConstWidgetClass wclass = (MwBaseConstWidgetClass) cw->core.widget_class;

	if (cw->baseConst.focused)
	{
		wclass->baseConst_class.unhighlightBorder(w);
		cw->baseConst.focused = False;
	}

}

static void HighlightBorder(Widget w)
{
	MwBaseConstWidget cw = (MwBaseConstWidget) w;

	X_DrawSimpleRawFrame(XtDisplay(w) , XtWindow(w) , 0 , 0 , cw->core.width ,
		cw->core.height , cw->baseConst.bd_width , cw->baseConst.bd_color);
}

static void UnhighlightBorder(Widget w)
{
	MwBaseConstWidget cw = (MwBaseConstWidget) w;

	X_DrawSimpleRawFrame(XtDisplay(w) , XtWindow(w) , 0 , 0 , cw->core.width ,
		cw->core.height , cw->baseConst.bd_width , cw->core.background_pixel);
}

static void FocusCurrent(Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
	MwBaseConstWidget cw = (MwBaseConstWidget) w;
	Widget parent = XtParent(w);

	if (cw->baseConst.focused) return;

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
