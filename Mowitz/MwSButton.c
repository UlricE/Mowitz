/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xlib.h>

#include "MwSButtonP.h"
#include "MwNws.h"

#define offset(field) XtOffsetOf(MwSButtonRec,sButton.field)

static XtResource resources [] = {
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
	 XtNswitch ,
	 XtCSwitch ,
	 XtRCallback ,
	 sizeof(XtCallbackList) ,
	 offset(switchcb) ,
	 XtRCallback ,
	 (XtPointer) NULL
	},
        {
         XtNbuttonMode,
         XtCButtonMode,
         XtRButtonMode,
         sizeof(int),
         offset(mode),
         XtRImmediate,
         (XtPointer) XtCnormalMode,
        },
	{
	 XtNinitDelay,
	 XtCInitDelay,
	 XtRCardinal,
	 sizeof(Cardinal),
	 offset(init_delay),
	 XtRImmediate,
	(XtPointer)500,
	},
	{
	 XtNrepeatDelay,
	 XtCRepeatDelay,
	 XtRCardinal,
	 sizeof(Cardinal),
	 offset(repeat_delay),
	 XtRImmediate,
	 (XtPointer)100, 
	},
	{
	 XtNhighlight_on_enter ,
	 XtCHighlight_on_enter ,
	 XtRBoolean ,
	 sizeof(Boolean) ,
	 XtOffsetOf(MwSButtonRec,base.highlight_on_enter) ,
	 XtRImmediate ,
	 (XtPointer) True
	},
	{
	 XtNbox_type ,
	 XtCBox_type ,
	 XtRBox_type ,
	 sizeof(int) ,
	 XtOffsetOf(MwSButtonRec,base.box_type) ,
	 XtRImmediate ,
	 (XtPointer) XtCup_box
	},
	{
	 XtNon ,
	 XtCOn ,
	 XtRBoolean ,
	 sizeof(Boolean) ,
	 offset(on) ,
	 XtRImmediate ,
	 (XtPointer) False
	},
};

#undef offset

static void ClassInitialize(void);
static void Initialize (Widget, Widget, ArgList, Cardinal *);

static void deactivate_t (Widget, XEvent *, String *, Cardinal *);
static void activate_t (Widget, XEvent *, String *, Cardinal *);
static void KBactivate (Widget, XEvent *, String *, Cardinal *);
static void timerCB (XtPointer, XtIntervalId *);
static void Enter_Leave (Widget, XEvent *, String *, Cardinal *);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);

static XtActionsRec action [] = {
        {"activate",activate_t},
        {"deactivate",deactivate_t},
        {"KBactivate",KBactivate},
	{"enter_leave",Enter_Leave},
};

static char trans_tab [] =
        "\
	 <FocusIn>: focusIn()\n\
	 <FocusOut>: focusOut()\n\
	 ~Shift<Key>Tab: traverseForward()\n\
	 Shift<Key>Tab: traverseBackward()\n\
	 <Key>Return: KBactivate()\n\
	 <Key>space: KBactivate()\n\
         <Btn1Down>: hide_help() activate() \n\
	 <Btn1Up>: deactivate() \n\
	 <Enter>: enter_leave() highlight() show_help() \n\
	 <Leave>: enter_leave() unhighlight() hide_help() \n\
	 <BtnDown>: hide_help() \n\
	 <KeyDown>: hide_help() \n\
	";

MwSButtonClassRec mwSButtonClassRec = {
/* core */
   {
    /* superclass            */ (WidgetClass) &mwBaseClassRec,
    /* class_name            */ "MwSButton",
    /* widget_size           */ sizeof(MwSButtonRec),
    /* class_initialize      */ ClassInitialize,
    /* class_part_initialize */ NULL,
    /* class_inited          */ FALSE,
    /* initialize            */ (XtInitProc) Initialize,
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
    /* set_values            */ SetValues,
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
    /* unhighlight	       */ XtInheritUnhighlight,
    /* highlightBorder	       */ XtInheritHighlightBorder,
    /* unhighlightBorder       */ XtInheritUnhighlightBorder,
   },
/* sButton */
   {
    /* empty                 */ 0
   }
};

WidgetClass mwSButtonWidgetClass = (WidgetClass) & mwSButtonClassRec;

static void ClassInitialize(void)
{
	_InitializeWidgetSet();

	XtSetTypeConverter(XtRString, XtRButtonMode, cvtStringToButtonMode,
		NULL, 0, XtCacheNone, NULL);
}

static void Initialize(Widget req_widget, Widget new_widget,
		ArgList args, Cardinal *num_args)
{
	MwSButtonWidget nw = (MwSButtonWidget) new_widget;

	if (nw->sButton.mode == XtCtoggleMode)
	{
		if (nw->sButton.on)
		{
			nw->sButton.pressed = True;
			nw->base.box_type =  XtCdown_box;
		}
		else
		{
			nw->sButton.pressed = False;
			nw->base.box_type =  XtCup_box;
		}
	}
	else nw->sButton.pressed = False;
}

static Boolean SetValues(Widget current, Widget request, Widget new_widget,
		ArgList args, Cardinal *num_args)
{
	MwSButtonWidget nw = (MwSButtonWidget) new_widget;
	MwSButtonWidget cw = (MwSButtonWidget) current;
	Boolean redraw = False;

	if (nw->sButton.mode == XtCtoggleMode && (cw->sButton.on != nw->sButton.on || 
	    cw->sButton.mode != nw->sButton.mode))
	{
		if (nw->sButton.on)
			nw->base.box_type = XtCdown_box;
		else
			nw->base.box_type = XtCup_box;

		redraw = True;
	}

	return redraw;
}

static void KBactivate(Widget sw, XEvent *event,
		String *params, Cardinal *num_params)
{
	MwSButtonWidget w = (MwSButtonWidget)sw;
	if (w->sButton.mode == XtCtoggleMode)
	{
 		w->sButton.on = !w->sButton.on;

		if (w->sButton.on)
		{
			XtVaSetValues((Widget)w , XtNbox_type , XtCdown_box , NULL);
		}
		else
		{
			XtVaSetValues((Widget)w , XtNbox_type , XtCup_box , NULL);
		}

		XtCallCallbackList((Widget)w , w->sButton.switchcb , NULL);
	}
	else
	{
		XtVaSetValues((Widget)w , XtNbox_type , XtCdown_box , NULL);

		XtCallCallbackList((Widget)w, w->sButton.activate, NULL);

		XtVaSetValues((Widget)w , XtNbox_type , XtCup_box , NULL);
	}
}

static void Enter_Leave (Widget sw, XEvent *event,
		String *params, Cardinal *num_params)
{
	MwSButtonWidget w = (MwSButtonWidget)sw;
	if (w->sButton.mode == XtCcyclicMode) return;

	if (w->sButton.mode == XtCnormalMode)
	{
		if ( (w->sButton.pressed) && 
		     (event->type == LeaveNotify))
		{
			XtVaSetValues((Widget)w , XtNbox_type , XtCup_box , NULL);

			w->sButton.pressed = False;
		}
		else if ((event->type == EnterNotify) &&
			 (event->xcrossing.state & Button1Mask)) 	
		{
			w->sButton.pressed = True;

			XtVaSetValues((Widget)w , XtNbox_type , XtCdown_box , NULL);
		}
	}
	else
	{
		if ((w->sButton.pressed) && 
		     (event->type == LeaveNotify))
		{
			if (w->sButton.on)
			{
				XtVaSetValues((Widget)w , XtNbox_type , XtCdown_box , NULL);
			}
			else
			{
				XtVaSetValues((Widget)w , XtNbox_type , XtCup_box , NULL);
			}

			w->sButton.pressed = False;
		}
		else if ((event->type == EnterNotify) &&
			 (event->xcrossing.state & Button1Mask)) 	
		{
			if (w->sButton.on)
			{
				XtVaSetValues((Widget)w , XtNbox_type , XtCdown_box , NULL);
			}
			else
			{
				XtVaSetValues((Widget)w , XtNbox_type , XtCup_box , NULL);
			}
			w->sButton.pressed = True;
		}	
	}
}

static void activate_t(Widget sw , XEvent *event ,
		String *params , Cardinal *num_params)
{
	MwSButtonWidget w = (MwSButtonWidget)sw;
	if (w->sButton.mode == XtCcyclicMode)
	{
		XtVaSetValues((Widget)w , XtNbox_type , XtCdown_box , NULL);

		XtCallCallbackList((Widget)w, w->sButton.activate, NULL);

		w->sButton.timer = XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)w),
					w->sButton.init_delay, timerCB, w);
       	}
       	else if (w->sButton.mode == XtCnormalMode)
       	{
		w->sButton.pressed = True;

		XtVaSetValues((Widget)w , XtNbox_type , XtCdown_box , NULL);
       	}
       	else if (w->sButton.mode == XtCtoggleMode)
       	{
		if (w->sButton.on)
		{
			XtVaSetValues((Widget)w , XtNbox_type , XtCup_box , NULL);
		}
		else
		{
			XtVaSetValues((Widget)w , XtNbox_type , XtCdown_box , NULL);
		}
	       	w->sButton.pressed = True;
       	}
}


static void deactivate_t(Widget sw , XEvent *event ,
		String *params , Cardinal *num_params)
{
	MwSButtonWidget w = (MwSButtonWidget)sw;
	if (w->sButton.mode == XtCcyclicMode)
	{
		XtVaSetValues((Widget)w , XtNbox_type , XtCup_box , NULL);

		XtRemoveTimeOut(w->sButton.timer);
	} 
	else  if (w->sButton.mode == XtCnormalMode)
	{
		XtVaSetValues((Widget)w , XtNbox_type , XtCup_box , NULL);

		XFlush(XtDisplay((Widget)w));

		if (w->sButton.pressed)
			XtCallCallbackList((Widget)w,w->sButton.activate, NULL);

		w->sButton.pressed = False;
	}
       	else if (w->sButton.mode == XtCtoggleMode)
       	{
       		if (w->sButton.pressed)
       		{
			w->sButton.on = !w->sButton.on;

			if (w->sButton.on)
			{
	       			w->sButton.pressed = False;
				XtVaSetValues((Widget)w , XtNbox_type , XtCdown_box , NULL);
			}
			else
			{
		       		w->sButton.pressed = False;
				XtVaSetValues((Widget)w , XtNbox_type , XtCup_box , NULL);
			}

			XtCallCallbackList((Widget)w , w->sButton.switchcb , NULL);

		}
       	}
}


static void timerCB(XtPointer client_data, XtIntervalId *timer)
{
	MwSButtonWidget w = (MwSButtonWidget) client_data;


	XtCallCallbackList((Widget) w, w->sButton.activate, NULL);
	

	w->sButton.timer = XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)w),
				w->sButton.repeat_delay, timerCB, w);
}
