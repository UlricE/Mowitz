
/*
 * MwHandle.c - MwHandle widget
 */

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "MwUtils.h"
#include "MwXutils.h"
#include "MwHandleP.h"
#include <X11/Shell.h>

#define MULTI_CLICK_TIME 500

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

static char translations[] =
	"<Btn1Down>:	handle-start()	\n"
	"<Btn1Motion>:	handle-drag()	\n"
	"<Btn1Up>:	handle-release()";


#define offset(field) XtOffsetOf(MwHandleRec, field)
static XtResource resources[] = {
	{
		XtNborderWidth,				/* name */
		XtCBorderWidth,				/* class */
		XtRDimension,				/* type */
		sizeof(Dimension),			/* size */
		XtOffsetOf(RectObjRec,rectangle.border_width), /* offset */
		XtRImmediate,				/* default_type */
		(XtPointer)0				/* default_addr */
	}, {
		XtNhandleVictim,
		XtCHandleVictim,
		XtRWidget,
		sizeof(Widget),
		offset(handle.victim),
		XtRImmediate,
		(XtPointer)None,
	}, {
		XtNattachCallback,
		XtCAttachCallback,
		XtRCallback,
		sizeof(XtPointer),
		offset(handle.attach_callbacks),
		XtRCallback,
		(XtPointer)NULL
	}, {
		XtNdetachCallback,
		XtCDetachCallback,
		XtRCallback,
		sizeof(XtPointer),
		offset(handle.detach_callbacks),
		XtRCallback,
		(XtPointer)NULL
	}, {
		XtNwidth,
		XtCWidth,
		XtRDimension,
		sizeof(Dimension),
		XtOffsetOf(RectObjRec,rectangle.width),
		XtRImmediate,
		(XtPointer)9
	}, {
		XtNshadowWidth,
		XtCShadowWidth,
		XtRDimension,
		sizeof(Dimension),
		offset(frame.shadowWidth),
		XtRImmediate,
		(XtPointer)1
	}, {
		XtNshadowType,
		XtCShadowType,
		XtRShadowType,
		sizeof(XtShadowType),
		offset(frame.type),
		XtRImmediate,
		(XtPointer)Raised
	}
};
#undef offset

static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static void handle_start(Widget, XEvent *, String *, Cardinal *),
	 handle_drag(Widget, XEvent *, String *, Cardinal *),
	 handle_release(Widget, XEvent *, String *, Cardinal *);
static void Initialize(Widget, Widget, ArgList, Cardinal *);

static XtActionsRec actionsList[] = {
	{"handle-start", handle_start},
	{"handle-drag", handle_drag},
	{"handle-release", handle_release},
};

#define SuperClass ((MwFrameWidgetClass)&mwFrameClassRec)

MwHandleClassRec mwHandleClassRec = {
  {
    (WidgetClass) SuperClass, 		/* superclass		  */	
    "MwHandle",				/* class_name		  */
    sizeof(MwHandleRec),		/* size			  */
    NULL,				/* class_initialize	  */
    NULL,				/* class_part_initialize  */
    FALSE,				/* class_inited		  */
    Initialize,				/* initialize		  */
    NULL,				/* initialize_hook	  */
    XtInheritRealize,			/* realize		  */
    actionsList,			/* actions		  */
    XtNumber(actionsList),		/* num_actions		  */
    resources,				/* resources		  */
    XtNumber(resources),		/* resource_count	  */
    NULLQUARK,				/* xrm_class		  */
    FALSE,				/* compress_motion	  */
    TRUE,				/* compress_exposure	  */
    TRUE,				/* compress_enterleave    */
    FALSE,				/* visible_interest	  */
    NULL,				/* destroy		  */
    XtInheritResize,			/* resize		  */
    XtInheritExpose,			/* expose		  */
    SetValues,				/* set_values		  */
    NULL,				/* set_values_hook	  */
    XtInheritSetValuesAlmost,		/* set_values_almost	  */
    NULL,				/* get_values_hook	  */
    NULL,				/* accept_focus		  */
    XtVersion,				/* version		  */
    NULL,				/* callback_private	  */
    translations,			/* tm_table		  */
    XtInheritQueryGeometry,		/* query_geometry	  */
    XtInheritDisplayAccelerator,	/* display_accelerator	  */
    NULL				/* extension		  */
  },
  {
/* composite_class fields */
    /* geometry_manager   */    NULL,
    /* change_managed     */    NULL,
    /* insert_child       */    XtInheritInsertChild,   /* TODO? */
    /* delete_child       */    XtInheritDeleteChild,   /* TODO? */
    /* extension          */    NULL
  },
  {
/* Frame class fields */
    /* extension          */    NULL,
  },
  {
    0,                                     /* field not used    */
  },  /* MwHandleClass fields initialization */
};

  /* for public consumption */
WidgetClass mwHandleWidgetClass = (WidgetClass) &mwHandleClassRec;

static void Initialize(Widget request, Widget new,
			ArgList args, Cardinal *num_args)
{
	MwHandleWidget cbw = (MwHandleWidget) new;
	cbw->handle.parent = None;
	cbw->handle.detached = False;
	cbw->handle.shell = None;
	cbw->handle.old_x = 0;
	cbw->handle.old_y = 0;
	cbw->handle.oldtime = 0;
}


static int bogosity(Display *d, XErrorEvent *e)
{
	return 0;
}

static void handle_start(Widget w, XEvent *event, String *p, Cardinal *n)
{
	MwHandleWidget hw = (MwHandleWidget)w;
	Time newtime = event->xbutton.time;
	int (*old_handler)(Display *, XErrorEvent *);
	if (hw->handle.detached) {
		XRaiseWindow(XtDisplay(w), XtWindow(hw->handle.shell));
		if (abs((long)newtime-(long)hw->handle.oldtime)
				< MULTI_CLICK_TIME) {
			XReparentWindow(XtDisplay(w),
				XtWindow(hw->handle.victim),
				XtWindow(hw->handle.parent),
				hw->handle.old_x, hw->handle.old_y);
			XtDestroyWidget(hw->handle.shell);

			/* We must now undo all the magic we did below */
			/* set mappedWhenManaged to the same value as before */
			XtSetMappedWhenManaged(hw->handle.victim,
						hw->handle.mwm);
			/* remanage the widget if it was previously managed */
			if (hw->handle.managed)
				XtManageChild(hw->handle.victim);
			/* Magic undone */

			hw->handle.detached = 0;

			/* I keep getting X errors here and don't really
			   see any reason why; this is a workaround */
			old_handler = XSetErrorHandler(bogosity);
			XtCallCallbackList(w, hw->handle.attach_callbacks,
				(XtPointer)hw->handle.victim);
			XFlush(XtDisplay(w));
			XSync(XtDisplay(w), False);
			XSetErrorHandler(old_handler);
		}
	}
	hw->handle.oldtime = newtime;
}

static void handle_drag(Widget w, XEvent *event, String *p, Cardinal *n)
{
	MwHandleWidget hw = (MwHandleWidget)w;
	Window root, child;
	int root_x, root_y, win_x, win_y;
	unsigned int mask;
	Dimension width, height;
	if (hw->handle.victim == None) return;

	XQueryPointer(XtDisplay(w), XtWindow(w),
		&root, &child,
		&root_x, &root_y,
		&win_x, &win_y,
		&mask);
	if (!hw->handle.detached) {
		int owner_events = False;
	        unsigned int event_mask = ButtonReleaseMask | PointerMotionMask |
                        EnterWindowMask | LeaveWindowMask;
	        int pointer_mode = GrabModeAsync;
	        int keyboard_mode = GrabModeAsync;
	        static Cursor cursor = None;
	        Time now = CurrentTime;
		hw->handle.parent = XtParent(hw->handle.victim);/* Rudegrid */
		XtVaGetValues(hw->handle.victim,
			XtNx, &(hw->handle.old_x),
			XtNy, &(hw->handle.old_y),
			XtNwidth, &width,
			XtNheight, &height,
			(char *)0);
		hw->handle.shell = XtVaCreatePopupShell("handle_shell",
			overrideShellWidgetClass, w,
			XtNx, root_x,
			XtNy, root_y,
			XtNwidth, width,
			XtNheight, height,
			(char *)0);

		XtRealizeWidget(hw->handle.shell);
		XtPopup(hw->handle.shell, XtGrabNone);

		XReparentWindow(XtDisplay(w),
			XtWindow(hw->handle.victim),
			XtWindow(hw->handle.shell), 0, 0);

		/* And now, a bit of magic */
		/* remember if the widget was mappenWhenManaged... */
		XtVaGetValues(hw->handle.victim,
			XtNmappedWhenManaged, &(hw->handle.mwm),
			(char *)0);
		/* ... and if it was managed */
		hw->handle.managed = XtIsManaged(hw->handle.victim);
		XtSetMappedWhenManaged(hw->handle.victim, False);
		/* must unmanage the child to keep it from being resized
		   by the parent. */
		XtUnmanageChild(hw->handle.victim);
		/* this has probably unmapped the widget, so remap it */
		XtMapWidget(hw->handle.victim);
		/* End of magic */

		XtCallCallbackList(w, hw->handle.detach_callbacks,
			(XtPointer)hw->handle.victim);
		XtGrabPointer(w, owner_events, event_mask,
			pointer_mode, keyboard_mode, None, cursor, now);
		hw->handle.detached = 1;

	} else {
		XtVaSetValues(hw->handle.shell,
			XtNx, root_x,
			XtNy, root_y,
			(char *)0);
	}
}

static void handle_release(Widget w, XEvent *event, String *p, Cardinal *n)
{
	XtUngrabPointer(w, CurrentTime);
}

static Boolean SetValues (Widget current, Widget request, Widget new,
			ArgList args, Cardinal *num_args)
{
	return True;
}

Widget MwMakeHandle(Widget pw, Widget victim,
		void (*detach)(Widget, XtPointer, XtPointer),
		void(*attach)(Widget, XtPointer, XtPointer))
{
	Widget w;
	Pixmap pm;
	Pixel color;
	XtVaGetValues(pw,
		XtNbackground, &color,
		(char *)0);
	pm = MwLoadPixmap(XtDisplay(pw), color, "handlebg.xpm");
	w = XtVaCreateManagedWidget("handle",
		mwHandleWidgetClass, pw,
		XtNforeground, color,
		XtNbackgroundPixmap, pm,
		XtNshadowWidth, 1,
		XtNhandleVictim, victim,
		XtNlabel, "",
		(char *)0);
	XtAddCallback(w, XtNattachCallback, attach, (XtPointer)victim);
	XtAddCallback(w, XtNdetachCallback, detach, (XtPointer)victim);
	return w;
}

