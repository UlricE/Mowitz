/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#include <stdio.h>

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Converters.h>
#include <Mowitz/MwUtils.h>

#include "MwMenuP.h"
#include "MwNws.h"

#define HIDE_PROTO 1
#include "MwBaseMEP.h"
#undef HIDE_PROTO

#define PDTIME 500

#define offset(field) XtOffsetOf(MwMenuRec, menu.field)

static XtResource resources[] = { 
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
	 XtNborderWidth,
	 XtCBorderWidth,
	 XtRInt,
	 sizeof(Dimension),
	 XtOffsetOf(MwMenuRec,core.border_width),
	 XtRImmediate,
	 (XtPointer) 0
	},
	{
	 XtNbacking_store ,
	 XtCBacking_store ,
	 XtRBackingStore ,
	 sizeof(int) ,
	 offset(backing_store) ,
	 XtRImmediate ,
	 (XtPointer) (Always + WhenMapped + NotUseful)
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
static void ClassInitialize(void);
static void ResolveInheritance(WidgetClass);
static void Resize (Widget);
static XtGeometryResult GeometryManager(Widget,
		XtWidgetGeometry *, XtWidgetGeometry *);
static void ChangeManaged(Widget);

static void GetPositionEntry (Widget, int, int, XEvent *, MwBaseMEObject **);
static void GrabAll (Widget, XtPointer, XtPointer);
static void timerCB (XtPointer, XtIntervalId *);

static void PopdownSubmenu(Widget);
static void PopdownAll(Widget);
static void InsertSubmenu(Widget);

static void GetInternalDimension(Widget,Position *,Position *,Dimension *,Dimension *);
static void SetInternalDimension(Widget,Dimension,Dimension);

static Dimension get_height(MwMenuWidget);
static Dimension get_max_width(MwMenuWidget);
static void popdown_menu(Widget);

static void Button_Press(Widget, XEvent *, String *, Cardinal *);
static void Button_Release(Widget, XEvent *, String *, Cardinal *);
static void Motion(Widget, XEvent *, String *, Cardinal *);
static void Enter(Widget, XEvent *, String *, Cardinal *);
static void Leave(Widget, XEvent *, String *, Cardinal *);

static XtActionsRec action [] = {
	{"buttonpress",Button_Press},
	{"buttonrelease",Button_Release},
	{"motion",Motion},
	{"enter",Enter},
	{"leave",Leave}
	};

static char trans_tab [] =
	"<BtnDown>: buttonpress() \n\
	 <BtnUp>: buttonrelease() \n\
	 <Leave>: leave() \n\
	 <Enter>: enter() \n\
	 <Motion>: motion() \n\
	 <PtrMoved>: motion() \n\
	 <MouseMoved>: motion() \n\
	 ";

MwMenuClassRec mwMenuClassRec = {
/* core */
   {
    /* superclass            */ (WidgetClass) &overrideShellClassRec,
    /* class_name            */ "MwMenu",
    /* widget_size           */ sizeof(MwMenuRec),
    /* class_initialize      */ ClassInitialize,
    /* class_part_initialize */ ResolveInheritance,
    /* class_inited          */ FALSE,
    /* initialize            */ (XtInitProc) Initialize,
    /* initialize_hook       */ NULL,
    /* realize               */ XtInheritRealize,
    /* actions               */ action,
    /* num_actions           */ XtNumber(action),
    /* resources             */ resources,
    /* num_resources         */ XtNumber(resources),
    /* xrm_class             */ NULLQUARK,
    /* compress_motion       */ True,
    /* compress_exposure     */ True,
    /* compress_enterleave   */ True,
    /* visible_interest      */ FALSE,
    /* destroy               */ NULL,
    /* resize                */ Resize,
    /* expose                */ Redisplay,
    /* set_values            */ (XtSetValuesFunc) SetValues,
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
/* composite */ 
   {
    /* geometry_manager	    */	GeometryManager,
    /* change_managed	    */	ChangeManaged,
    /* insert_child	    */	XtInheritInsertChild,
    /* delete_child	    */	XtInheritDeleteChild,
    /* extension	    */	NULL
   },
/* shell */
   {
    /* extension	    */	NULL
   },
/* override shell */
   {
    /* extension	    */	NULL
   },
/* menu */
   {
    /* get_internal_dimension  */ GetInternalDimension,
    /* set_internal_dimension  */ SetInternalDimension,
    /* num_submenu	       */ 0,
    /* submenu		       */ NULL,
    /* insert_submenu	       */ InsertSubmenu,
    /* popdown_submenu	       */ PopdownSubmenu,
    /* popdown_all	       */ PopdownAll,
   },
};

WidgetClass mwMenuWidgetClass = (WidgetClass) & mwMenuClassRec;

#define ForAllChildren(cw, childP) \
	for ( (childP) = (MwBaseMEObject *) (cw)->composite.children ; \
		(childP) < (MwBaseMEObject *) ((cw)->composite.children + \
		(cw)->composite.num_children ) ; \
		(childP)++ )

static void ClassInitialize(void)
{
	_InitializeWidgetSet();

	XtSetTypeConverter(XtRString , XtRBox_type , cvtStringToBoxType,
                 NULL , 0 , XtCacheNone , NULL);

	XtSetTypeConverter(XtRString , XtRBackingStore ,
		 (XtTypeConverter)XmuCvtStringToBackingStore ,
                 NULL , 0 , XtCacheNone , NULL);

}


static void ResolveInheritance(WidgetClass class)
{
	MwMenuWidgetClass c = (MwMenuWidgetClass) class;
	MwMenuWidgetClass super;
	static CompositeClassExtensionRec extension_rec = {
		NULL, NULLQUARK, XtCompositeExtensionVersion,
		sizeof(CompositeClassExtensionRec), True};
	CompositeClassExtensionRec *ext;

  	ext = MwMalloc(sizeof(*ext));
	*ext = extension_rec;
	ext->next_extension = c->composite_class.extension;
	c->composite_class.extension = ext;

	c->menu_class.submenu = NULL;
	c->menu_class.num_submenu = 0;

	if (class == mwMenuWidgetClass) return;
	super = (MwMenuWidgetClass)class->core_class.superclass;

	if (c->menu_class.get_internal_dimension == XtInheritGetInternalDimension)
		c->menu_class.get_internal_dimension =
			super->menu_class.get_internal_dimension;

	if (c->menu_class.set_internal_dimension == XtInheritSetInternalDimension)
		c->menu_class.set_internal_dimension =
			super->menu_class.set_internal_dimension;

	if (c->menu_class.popdown_submenu == XtInheritPopdownSubmenu)
		c->menu_class.popdown_submenu =
			super->menu_class.popdown_submenu;

	if (c->menu_class.popdown_all == XtInheritPopdownAll)
		c->menu_class.popdown_all =
			super->menu_class.popdown_all;

}

static void Initialize(Widget req_widget, Widget new_widget,
		ArgList args, Cardinal *num_args)
{
	MwMenuWidget nw = (MwMenuWidget) new_widget;
	XColor	dark,light,bg;
	Display *dpy=XtDisplay(new_widget);

	bg.pixel = nw->menu.box_color;
	
	XQueryColor(dpy,DefaultColormap(dpy,DefaultScreen(dpy)),&bg);
	
	LightColor2(new_widget, light, nw->menu.top_shadow_contrast);
	DarkColor2(new_widget, dark, nw->menu.bot_shadow_contrast);

	nw->menu.light = light.pixel;
	nw->menu.dark = dark.pixel;

	nw->menu.last_selected = NULL;
	
	XtAddCallback(new_widget , XtNpopupCallback, GrabAll , (XtPointer)NULL);

}

static void Redisplay(Widget w, XEvent *event, Region region)
{
	MwMenuWidget cw = (MwMenuWidget) w;
	Display *dpy = XtDisplay(w);
	Window win = XtWindow (w);
	MwBaseMEObject * entry;	
	MwBaseMEObjectClass class;

	if (region == NULL) XClearWindow(dpy , win);

	switch (cw->menu.box_type)
	{
		case XtCno_box:
		case XtCshadow_box:
			break;

		case XtCsimple_box:
			X_DrawSimpleRawFrame(dpy , win , 0 , 0 , cw->core.width ,
					cw->core.height , cw->menu.box_width ,
					cw->menu.box_color);
			break;
		case XtCup_box:
			X_DrawSimple3DFrame(dpy , win , 0 , 0 , cw->core.width ,
				cw->core.height , cw->menu.box_width ,
				cw->menu.light , cw->menu.dark);
			break;
		case XtCdown_box:
			X_DrawSimple3DFrame(dpy , win , 0 , 0 , cw->core.width ,
				cw->core.height , cw->menu.box_width ,
				cw->menu.dark , cw->menu.light);
			break;

		case XtCframein_box:
			X_DrawSimple3DFrame(dpy , win ,0 , 0 , cw->core.width ,
				cw->core.height , cw->menu.box_width / 2 ,
				cw->menu.dark , cw->menu.light);
			X_DrawSimple3DFrame(dpy , win , cw->menu.box_width / 2 ,
				cw->menu.box_width / 2 ,
				cw->core.width - 2 * (cw->menu.box_width / 2),
				cw->core.height - 2 * (cw->menu.box_width / 2),
				cw->menu.box_width / 2 ,
				cw->menu.light , cw->menu.dark);
			break;

		case XtCframeout_box:
			X_DrawSimple3DFrame(dpy , win , 0 , 0 , cw->core.width ,
				cw->core.height , cw->menu.box_width / 2 ,
				cw->menu.light , cw->menu.dark);
			X_DrawSimple3DFrame(dpy , win ,cw->menu.box_width / 2 ,
				cw->menu.box_width / 2 , 
				cw->core.width - 2 * (cw->menu.box_width / 2),
				cw->core.height - 2 * (cw->menu.box_width / 2), 
				cw->menu.box_width / 2 ,
				cw->menu.dark , cw->menu.light);
			break;

	}


	ForAllChildren(cw, entry)
	{
		if (!XtIsManaged ( (Widget) *entry)) continue;

		if (region != NULL) 
			switch(XRectInRegion(region , (*entry)->rectangle.x,
				(*entry)->rectangle.y , (*entry)->rectangle.width ,
				(*entry)->rectangle.height)) 
			{

				case RectangleIn:
				case RectanglePart:
					break;
				default:
					continue;
			}
		class = (MwBaseMEObjectClass) (*entry)->object.widget_class;

		if (class->rect_class.expose != NULL)
			(class->rect_class.expose)( (Widget) *entry, NULL, NULL);
	}
	
	if (!XtIsSensitive(w)) Xt_SetInsensitive(w);
}

static void Resize(Widget w)
{
	MwMenuWidget cw = (MwMenuWidget) w;
	MwBaseMEObject * entry;
	Position x , y;
	Dimension width , height;
	
	mwMenuClassRec.menu_class.get_internal_dimension(w , &x , &y , &width , &height);

	if ( !XtIsRealized(w) ) return;

	ForAllChildren(cw, entry)
		if (XtIsManaged( (Widget) *entry))
			(*entry)->rectangle.width = width;

}

#define WidgetValuesDiffer(w1,w2,component) (w1 -> menu.component != \
                                             w2 -> menu.component)

static Boolean SetValues(Widget current, Widget request, Widget new_widget,
		ArgList args, Cardinal *num_args)
{
	MwMenuWidget cw = (MwMenuWidget) current;
	MwMenuWidget nw = (MwMenuWidget) new_widget;

	Boolean redraw=False;


	if WidgetValuesDiffer( cw , nw , cursor)
	{
		XDefineCursor(XtDisplay(current),XtWindow(current),nw->menu.cursor);
	}

	return redraw;
}

static XtGeometryResult GeometryManager(Widget w ,
		XtWidgetGeometry *request , XtWidgetGeometry *reply)
{
	MwMenuWidget cw = (MwMenuWidget) XtParent(w);
	MwBaseMEObject aentry = (MwBaseMEObject) w;
	XtGeometryResult ret_val;
	Position x,y;
	Dimension height , width;
	MwBaseMEObject * entry;
	
	mwMenuClassRec.menu_class.get_internal_dimension((Widget) cw , 
		&x , &y , &height , &width);

	reply->width = width;

	if (request->request_mode & CWHeight)
	{
		reply->height = request->height;
	}

	if ((request->width < width) && (request->request_mode & CWWidth))
		ret_val = XtGeometryAlmost;
	else
	{
		if (request->request_mode & CWWidth) width = request->width;
		reply->width = width;
		ret_val = XtGeometryYes;
	}

	if (request->request_mode & CWHeight)
	{
		aentry->rectangle.height = request->height;
	}
	
	ForAllChildren(cw , entry)
	{
		if (!XtIsManaged((Widget) * entry)) continue;

		(*entry)->rectangle.width = reply->width;
	}

	height = get_height(cw);

	mwMenuClassRec.menu_class.set_internal_dimension((Widget) cw , width , height);
	
	return ret_val;		
}

static void ChangeManaged(Widget w)
{
	MwMenuWidget cw = (MwMenuWidget) w;
	Dimension height , width;
	Position y , x;
	MwBaseMEObject * entry;

	mwMenuClassRec.menu_class.get_internal_dimension(w , &x , &y , &width , &height);

	width = get_max_width(cw);

	ForAllChildren(cw , entry)
	{
		if (!XtIsManaged((Widget) * entry)) continue;

		(*entry)->rectangle.x = x;
		(*entry)->rectangle.y = y;
		(*entry)->rectangle.width = width;
		y +=(*entry)->rectangle.height;
	}

	height = get_height(cw);

	mwMenuClassRec.menu_class.set_internal_dimension((Widget) cw , width , height);

}

static void PopdownSubmenu(Widget w)
{
	MwMenuWidget cw = (MwMenuWidget) w;
	int i;

	for (i = ((MwMenuWidgetClass) cw->core.widget_class)->menu_class.num_submenu ; 
		i > cw->menu.menu_sub_position ; i--)
	{
		((MwMenuWidgetClass) cw->core.widget_class)->menu_class.num_submenu --;
		popdown_menu(((MwMenuWidgetClass) cw->core.widget_class)->
			menu_class.submenu[i-1]);
	}


	((MwMenuWidgetClass) cw->core.widget_class)->menu_class.submenu = 
		MwRealloc((void *)
		((MwMenuWidgetClass) cw->core.widget_class)->menu_class.submenu ,
		((MwMenuWidgetClass) cw->core.widget_class)->menu_class.num_submenu *
		sizeof(Widget));

}

static void PopdownAll(Widget w)
{
	MwMenuWidget cw = (MwMenuWidget) w;
	int i;


	for (i = ((MwMenuWidgetClass) cw->core.widget_class)->menu_class.num_submenu; 
		i > 0 ; i--)
	{
		popdown_menu(((MwMenuWidgetClass)cw->core.widget_class)->
			menu_class.submenu[i - 1]);
	}

	MwFree((void *)((MwMenuWidgetClass) cw->core.widget_class)->menu_class.submenu);

	((MwMenuWidgetClass) cw->core.widget_class)->menu_class.num_submenu = 0;
	((MwMenuWidgetClass) cw->core.widget_class)->menu_class.submenu = NULL;

}

static void InsertSubmenu(Widget w)
{
	MwMenuWidget cw = (MwMenuWidget) w;

	((MwMenuWidgetClass) cw->core.widget_class)->menu_class.num_submenu ++;

	cw->menu.menu_sub_position = 
		((MwMenuWidgetClass) cw->core.widget_class)->menu_class.num_submenu;


	((MwMenuWidgetClass) cw->core.widget_class)->menu_class.submenu = 
		MwRealloc((void *)
		((MwMenuWidgetClass) cw->core.widget_class)->menu_class.submenu ,
		(cw->menu.menu_sub_position + 1) * sizeof(Widget));


	((MwMenuWidgetClass) cw->core.widget_class)->menu_class.submenu
		[cw->menu.menu_sub_position - 1] = w;
	
}


static void GetInternalDimension(Widget w, Position *x, Position *y,
		Dimension *width, Dimension *height)
{
	MwMenuWidget cw = (MwMenuWidget) w;
	
	switch (cw->menu.box_type)
	{
		case XtCshadow_box:
		case XtCno_box:
			*x = 0;
			*width = cw->core.width;
			*y = 0;
			*height = cw->core.height;
			break;

		case XtCsimple_box:
		case XtCup_box:
		case XtCdown_box:
			*x = cw->menu.box_width;
			*width = cw->core.width - 2 * cw->menu.box_width;
			*y = cw->menu.box_width;
			*height = cw->core.height - 2 * cw->menu.box_width;
			break;

		case XtCframein_box:
		case XtCframeout_box:
			*x = 2 * (cw->menu.box_width / 2);
			*y = 2 * (cw->menu.box_width / 2);
			*width = cw->core.width - 4 * (cw->menu.box_width /2);
			*height = cw->core.height - 4 * (cw->menu.box_width /2);
			break;
	}
}

static void SetInternalDimension(Widget w, Dimension width, Dimension height)
{
	MwMenuWidget cw = (MwMenuWidget) w;

	switch (cw->menu.box_type)
	{
		case XtCshadow_box:
		case XtCno_box:
			cw->core.width=width;
			cw->core.height=height;
			break;
			
		case XtCsimple_box:
		case XtCup_box:
		case XtCdown_box:
			cw->core.width = width + 2 * cw->menu.box_width;
			cw->core.height = height + 2 * cw->menu.box_width;
			break;
			
		case XtCframein_box:
		case XtCframeout_box:	
			cw->core.width = width + 4 * (cw->menu.box_width /2);
			cw->core.height = height + 4 * (cw->menu.box_width /2);
			break;

	}
	XtResizeWidget(w , cw->core.width , cw->core.height , cw->core.border_width);
}

static Dimension get_height(MwMenuWidget w)
{
	MwBaseMEObject * entry;
	Dimension height = 0;

	ForAllChildren(w , entry)
	{
		if (!XtIsManaged((Widget) * entry)) continue;

		height += (*entry)->rectangle.height;
	}

	return height;
}

static Dimension get_max_width(MwMenuWidget w)
{
	MwBaseMEObject * entry;
	Dimension width = 0;
	XtWidgetGeometry pref_geom , inten_geom;

	inten_geom.width = 1;
	inten_geom.height = 1;
	inten_geom.request_mode = CWWidth | CWHeight;

	ForAllChildren(w , entry)
	{
		if (!XtIsManaged((Widget) * entry)) continue;

		XtQueryGeometry(((Widget) * entry) ,&inten_geom , &pref_geom);

		width = (width > pref_geom.width) ?
			width : pref_geom.width;
	}

	return width;
}

static int bogosity(Display *d, XErrorEvent *e)
{
	return 0;
}

static void GrabAll(Widget w, XtPointer client_data, XtPointer call_data)
{
	MwMenuWidget cw = (MwMenuWidget) w;
	Display *dpy = XtDisplay(w);
	Window Root = DefaultRootWindow(dpy);
	int (*old_error)(Display *, XErrorEvent *);

	if (((MwMenuWidgetClass)cw->core.widget_class)->menu_class.num_submenu == 0)
	{

		XGrabPointer(dpy , Root , True , 
			ButtonReleaseMask | ButtonPressMask,
			GrabModeAsync , GrabModeAsync ,
			None , cw->menu.cursor ,
			CurrentTime);

		/* Openwindows gives us BadAccess without this */
		old_error = XSetErrorHandler(bogosity);
		XGrabButton(dpy , AnyButton , AnyModifier , Root , True , 
			ButtonReleaseMask | ButtonPressMask,
			GrabModeAsync , GrabModeAsync ,
			cw->core.window , cw->menu.cursor);
		XSetErrorHandler(old_error);

		XChangeActivePointerGrab(dpy , ButtonReleaseMask | ButtonPressMask,
			cw->menu.cursor , XtLastTimestampProcessed(XtDisplay(w)));

		cw->menu.down_after_br = False;

		cw->menu.timer = XtAppAddTimeOut(XtWidgetToApplicationContext(w) , 
			PDTIME , timerCB , w);
	}
	else cw->menu.down_after_br = True;

	if (((MwMenuWidgetClass) cw->core.widget_class)->menu_class.insert_submenu)
		((MwMenuWidgetClass) cw->core.widget_class)->menu_class.insert_submenu(w);
}

static void Enter (Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
	MwMenuWidget cw = (MwMenuWidget) w;

	((MwMenuWidgetClass)cw->core.widget_class)->menu_class.popdown_submenu(w);
}

static void Leave (Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
	MwMenuWidget cw = (MwMenuWidget) w;
	MwBaseMEObjectClass class;

	if (cw->menu.last_selected)
	{
		class = (MwBaseMEObjectClass) (*cw->menu.last_selected)
			->object.widget_class;

		if (class->baseME_class.leavenotify)
			class->baseME_class.leavenotify((Widget) *cw->menu.last_selected);

	}
	cw->menu.last_selected = NULL;
}

static void Motion (Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
	MwMenuWidget cw = (MwMenuWidget) w;
	MwBaseMEObject * entry = NULL;
	MwBaseMEObject * pentry;
	MwBaseMEObjectClass class;

	GetPositionEntry(w , event->xmotion.x , event->xmotion.y , event , &entry);

	pentry = cw->menu.last_selected;
	cw->menu.last_selected = entry;

	if (pentry && (pentry != entry))
	{
		class = (MwBaseMEObjectClass) (*pentry)->object.widget_class;

		if (class->baseME_class.leavenotify)
			class->baseME_class.leavenotify((Widget) *pentry);
	}
	if (entry && (pentry != entry))
	{
		((MwMenuWidgetClass) cw->core.widget_class)
			->menu_class.popdown_submenu(w);

		class = (MwBaseMEObjectClass) (*entry)->object.widget_class;

		if (class->baseME_class.enternotify)
			class->baseME_class.enternotify((Widget) *entry);
	}
}

static void Button_Release (Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
	MwMenuWidget cw = (MwMenuWidget) w;
	MwBaseMEObject * entry = NULL;
	MwBaseMEObjectClass class;

	GetPositionEntry(w , event->xbutton.x , event->xbutton.y , event , &entry);

	if (! entry && cw->menu.down_after_br)
	{
		((MwMenuWidgetClass)cw->core.widget_class)->menu_class.popdown_all(w);
	}
	else
	{
		if (entry)
		{
			class = (MwBaseMEObjectClass) (*entry)->object.widget_class;

					
			((MwMenuWidgetClass)cw->core.widget_class)
				->menu_class.popdown_all(w);

			if (class->baseME_class.activate)
				class->baseME_class.activate((Widget) *entry);

		} else cw->menu.down_after_br = True;
	}
}

static void Button_Press (Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
	MwMenuWidget cw = (MwMenuWidget) w;
	MwBaseMEObject * entry = NULL;
	Display *dpy = XtDisplay(w);
	Widget other;

	GetPositionEntry(w , event->xbutton.x , event->xbutton.y ,
		event , &entry);

	if (!entry)
	{
		other = XtWindowToWidget(dpy , event->xany.window);
		if (other != None)
			if (XtIsSubclass(other , mwMenuWidgetClass))
					return;
	}
	if (!entry && cw->menu.down_after_br)
	{	
		((MwMenuWidgetClass)cw->core.widget_class)->menu_class.popdown_all(w);
	}

}

static void timerCB(XtPointer client_data, XtIntervalId *timer)
{
	MwMenuWidget w = (MwMenuWidget) client_data;

	w->menu.down_after_br = True;
}

static void GetPositionEntry(Widget w , int x , int y ,
		XEvent *event , MwBaseMEObject **entry )
{
	MwMenuWidget cw = (MwMenuWidget) w;
	MwBaseMEObject * pentry;
	Position pomx , pomy;
	Dimension width , height;

	if (XtWindow(w) != event->xany.window) return;

	mwMenuClassRec.menu_class.get_internal_dimension(w , &pomx , &pomy , 
		&width , &height);

	if ((x < pomx) || (x > pomx + width) || (y < pomy) || (y > pomy + height)) return;
	
	ForAllChildren(cw , pentry)
	{
		if (!XtIsManaged ((Widget) *pentry)) continue;

		if ( ( (*pentry)->rectangle.y <= y) && (( (*pentry)->rectangle.y +
			(int) (*pentry)->rectangle.height) >= y) && 
			(*pentry)->rectangle.sensitive)
		{
			*entry = pentry;
			return;
		}
			
			
	}
}

static void popdown_menu(Widget w)
{
	MwBaseMEObjectClass class;
	MwBaseMEObject * pentry;
	MwMenuWidget cw = (MwMenuWidget) w;

	XtPopdown(w);

	if (cw->menu.menu_sub_position == 1)
	{
		XUngrabPointer(XtDisplay(w),CurrentTime);
		XUngrabButton(XtDisplay(w),AnyButton , AnyModifier , 
			DefaultRootWindow(XtDisplay(w)));
	}
	ForAllChildren(cw , pentry)
	{
		if (!XtIsManaged ((Widget) *pentry)) continue;

		class = (MwBaseMEObjectClass) (*pentry)->object.widget_class;

		(*pentry)->baseME.entered = False;
/*
		if (class->rect_class.expose)
			class->rect_class.expose((Widget)*pentry , NULL , NULL);
*/

	}
}

void MwMenuDestroyChildren(Widget w)
{
	MwMenuWidget mw = (MwMenuWidget)w;
	int i, n;
	if (!XtIsSubclass(w, compositeWidgetClass)) {
		fprintf(stderr, "Widget %s is not a subclass of Composite\n",
			XtName(w));
		return;
	}
	n = mw->composite.num_children;
	for (i = 0; i < n; i++)
		XtDestroyWidget(mw->composite.children[0]);
}

