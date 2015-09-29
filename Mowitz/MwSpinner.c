/*
Copyright (C) 2001-2002  Ulric Eriksson <ulric@siag.nu>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the Licence, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA.
*/

/* ---
MwSpinner.c - MwSpinner compound widget
--- */

#include	<stdio.h>
#include	<stdlib.h>
#include	<X11/keysym.h>
#include	<X11/IntrinsicP.h>
#include	<X11/StringDefs.h>
#include	<X11/Shell.h>
#include	<X11/Xaw/Repeater.h>
#include	<X11/Xmu/Misc.h>
#include	<X11/xpm.h>
#include	"MwFrameP.h"
#include	"MwTextField.h"
#include	"MwSpinnerP.h"
#include	"pixmaps/spinup.xpm"
#include	"pixmaps/spindown.xpm"

/****************************************************************
 *
 * MwSpinner Resources
 *
 ****************************************************************/

#define XtNspinnerShell "spinnerShell"
#define XtCSpinnerShell "SpinnerShell"

#define offset(field) XtOffsetOf(MwSpinnerRec, spinner.field)
static XtResource resources[] = {
	{
		XtNmin,		/* name */
		XtCMin,		/* class */
		XtRInt,		/* type */
		sizeof(int),	/* size */
		offset(min),	/* offset */
		XtRImmediate,	/* default_type */
		(XtPointer)0	/* default_addr */
	}, {
		XtNmax,
		XtCMax,
		XtRInt,
		sizeof(int),
		offset(max),
		XtRImmediate,
		(XtPointer)100
	}, {
		XtNstep,
		XtCStep,
		XtRInt,
		sizeof(int),
		offset(step),
		XtRImmediate,
		(XtPointer)1
	}, {
		XtNvalue,
		XtCValue,
		XtRInt,
		sizeof(int),
		offset(value),
		XtRImmediate,
		(XtPointer)0
	},{
		XtNcallback,
		XtCCallback,
		XtRCallback,
		sizeof(XtPointer),
		offset(callbacks),
		XtRCallback,
		(XtPointer)NULL
	},{
		XtNshadowType,
		XtCShadowType,
		XtRShadowType,
		sizeof(XtShadowType),
		XtOffsetOf(MwFrameRec, frame.type),
		XtRImmediate,
		(XtPointer)Lowered
	},{
		XtNshadowWidth,
		XtCShadowWidth,
		XtRDimension,
		sizeof(Dimension),
		XtOffsetOf(MwFrameRec, frame.shadowWidth),
		XtRImmediate,
		(XtPointer)1
	}
};
#undef offset

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/


static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Realize(Widget, XtValueMask *, XSetWindowAttributes *);
static void Destroy(Widget);
static void Resize(Widget);
static XtGeometryResult GeometryManager(Widget,
                XtWidgetGeometry *, XtWidgetGeometry *);
static void ChangeManaged(Widget);
static void Redisplay();

static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);

#define SuperClass ((MwFrameWidgetClass)&mwFrameClassRec)

MwSpinnerClassRec mwSpinnerClassRec = {
  {
/* core_class fields      */
    /* superclass         */    (WidgetClass) SuperClass,
    /* class_name         */    "MwSpinner",
    /* widget_size        */    sizeof(MwSpinnerRec),
    /* class_initialize   */    NULL,
    /* class_part_init    */	NULL,
    /* class_inited       */	FALSE,
    /* initialize	  */	Initialize,
    /* initialize_hook    */	NULL,
    /* realize            */    Realize,
    /* actions            */    NULL,
    /* num_actions	  */	0,
    /* resources          */    resources,
    /* num_resources      */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion	  */	TRUE,
    /* compress_exposure  */	TRUE,
    /* compress_enterleave*/	TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    Destroy,
    /* resize             */    Resize,
    /* expose             */    Redisplay,
    /* set_values         */    SetValues,
    /* set_values_hook    */	NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */	NULL,
    /* accept_focus       */    NULL,
    /* version            */	XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    NULL,
    /* query_geometry     */	XtInheritQueryGeometry,
    /* display_accelerator*/	XtInheritDisplayAccelerator,
    /* extension          */	NULL
  },{
/* composite_class fields */
    /* geometry_manager   */    GeometryManager,
    /* change_managed     */    ChangeManaged,
    /* insert_child	  */	XtInheritInsertChild,
    /* delete_child	  */	XtInheritDeleteChild,
    /* extension          */	NULL
  },{
/* Frame class fields */
    /* extension	  */	NULL
  },{
/* MwSpinner class fields */
    /* empty		  */	0
  }
};

WidgetClass mwSpinnerWidgetClass = (WidgetClass)&mwSpinnerClassRec;


/****************************************************************
 *
 * Private Routines
 *
 ****************************************************************/

static enum {WAITING, ABORT, DONE} spinner_status;

static int spinner_bogosity(Display *d, XErrorEvent *e)
{
	return 0;
}

static void set_text(MwSpinnerWidget sw)
{
	char b[100];
	int n = sw->spinner.value;
	if (!XtIsRealized((Widget)sw)) return;
	if (n > sw->spinner.max) n = sw->spinner.max;
	if (n < sw->spinner.min) n = sw->spinner.min;
	sw->spinner.value = n;
	sprintf(b, "%d", n);
	MwTextFieldSetString(sw->spinner.text, b);
/*	XSetErrorHandler(old);
*/}

static void do_callback(MwSpinnerWidget sw)
{
	XtCallCallbackList((Widget)sw, sw->spinner.callbacks,
		(XtPointer)sw->spinner.value);
}

/* ---
This event handler is triggered when the user clicks in the text widget.
   It allows editing the text until Esc or Return is pressed.
   Pressing Return sets the widget value to the entered text and
   calls callbacks. Pressing Esc returns the text to what it was before.
*/

static void spinner_text_edit(Widget w, XtPointer p, XEvent *event, Boolean *n)
{
	MwSpinnerWidget cw = (MwSpinnerWidget)XtParent(w);
	Widget top;

	for (top = w; XtParent(top) != None; top = XtParent(top));
	XtAddGrab(w, True, False);
	XtSetKeyboardFocus(top, w);

	XtVaSetValues(w,
		XtNdisplayCaret, True,
		(char *)0);
	spinner_status = WAITING;
	while (spinner_status == WAITING) {
		int count, bufsiz = 10;
		XEvent event;
		XtAppNextEvent(XtWidgetToApplicationContext(w), &event);
		if (event.type == KeyPress) {
			char buf[12];
			KeySym keysym;
			XKeyEvent kevent = event.xkey;
			count = XLookupString(&kevent,
				buf, bufsiz, &keysym, NULL);
			if (keysym == XK_Escape) spinner_status = ABORT;
			else if (keysym == XK_Return) spinner_status = DONE;
			else XtDispatchEvent(&event);
		} else {
			XtDispatchEvent(&event);
		}
	}
	XtVaSetValues(w,
		XtNdisplayCaret, False,
		(char *)0);
	XtRemoveGrab(w);
	XtSetKeyboardFocus(top, None);

	if (spinner_status == DONE) {
		char *b = MwTextFieldGetString(w);
		cw->spinner.value = atoi(b);
		set_text(cw);
		do_callback(cw);
	} else {
		set_text(cw);
	}
}

static void spinup_cb(Widget w, XtPointer p, XtPointer q)
{
	MwSpinnerWidget sw = (MwSpinnerWidget)XtParent(w);
	sw->spinner.value += sw->spinner.step;
	set_text(sw);
	do_callback(sw);
}

static void spindown_cb(Widget w, XtPointer p, XtPointer q)
{
	MwSpinnerWidget sw = (MwSpinnerWidget)XtParent(w);
	sw->spinner.value -= sw->spinner.step;
	set_text(sw);
	do_callback(sw);
}

static void Initialize(Widget req, Widget new, ArgList args, Cardinal *num)
{
	XpmAttributes xa;
	Pixmap pm_return;
	int result = None;
	XpmColorSymbol symbol;
	Pixel color;
	MwSpinnerWidget cw = (MwSpinnerWidget)new;

/* I have no idea what's causing the X errors */
int (*old)(Display *, XErrorEvent *);
old = XSetErrorHandler(spinner_bogosity);

	if (req->core.width == 0) new->core.width = 80;
	if (req->core.height == 0) new->core.height = 20;
	XtClass(new)->core_class.resize(new);
	XtVaGetValues(new,
		XtNbackground, &color,
		(char *)0);
	xa.closeness = 40000;
	xa.exactColors = False;
	xa.valuemask = XpmCloseness | XpmExactColors;
	symbol.name = NULL;
	symbol.value = "none";
	symbol.pixel = color;
	xa.colorsymbols = &symbol;
	xa.numsymbols = 1;
	xa.valuemask |= XpmColorSymbols;

	cw->spinner.text = XtVaCreateManagedWidget("spinner_text",
		mwTextfieldWidgetClass, new,
		XtNdisplayCaret, False,
		(char *)0);
	XtAddEventHandler(cw->spinner.text,
		ButtonPressMask, False, spinner_text_edit, NULL);
	/*set_text(cw);*/

	result = XpmCreatePixmapFromData(XtDisplay(new),
		XRootWindowOfScreen(XtScreen(new)),
		spinup_xpm, &pm_return, NULL, &xa);
	if (result != XpmSuccess) {
		fprintf(stderr, "XpmCreatePixmapFromData returns %s\n",
			XpmGetErrorString(result));
	}
	cw->spinner.up = XtVaCreateManagedWidget("spinner_up",
		repeaterWidgetClass, new,
		XtNbitmap, pm_return,
		XtNforeground, color,
		(char *)0);
	XtAddCallback(cw->spinner.up, XtNcallback, spinup_cb, NULL);

	result = XpmCreatePixmapFromData(XtDisplay(new),
		XRootWindowOfScreen(XtScreen(new)),
		spindown_xpm, &pm_return, NULL, &xa);
	if (result != XpmSuccess) {
		fprintf(stderr, "XpmCreatePixmapFromData returns %s\n",
			XpmGetErrorString(result));
	}
	cw->spinner.down = XtVaCreateManagedWidget("spinner_down",
		repeaterWidgetClass, new,
		XtNbitmap, pm_return,
		XtNforeground, color,
		(char *)0);
	XtAddCallback(cw->spinner.down, XtNcallback, spindown_cb, NULL);
XSetErrorHandler(old);
}

static void Realize(Widget w, XtValueMask *valueMask,
                XSetWindowAttributes *attributes)
{
	MwSpinnerWidget cw = (MwSpinnerWidget)w;

	(*SuperClass->core_class.realize)(w, valueMask, attributes);
	if (cw->spinner.text) {
		XtRealizeWidget(cw->spinner.text);
		set_text(cw);
	}
}

static void Destroy(Widget w)
{
	MwSpinnerWidget cw = (MwSpinnerWidget)w;
	XtDestroyWidget(cw->spinner.up);
	XtDestroyWidget(cw->spinner.down);
	XtDestroyWidget(cw->spinner.text);
}

static void DoLayout(MwSpinnerWidget sw)
{
	Widget tw, cw;
	Dimension w1, w2, h1;
	int bw = sw->frame.shadowWidth;
	w2 = sw->core.width-2*bw;
	w1 = w2-16;
	h1 = sw->core.height-2*bw;

	if (w1 < 2) w1 = 2;
	switch (sw->composite.num_children) {
	case 3:
		cw = sw->composite.children[2];
		XtConfigureWidget(cw, w1+bw, bw+h1/2, w2-w1, h1/2, 0);
	case 2:
		cw = sw->composite.children[1];
		XtConfigureWidget(cw, w1+bw, bw, w2-w1, h1/2, 0);
	case 1:
		tw = sw->composite.children[0];
		XtConfigureWidget(tw, bw, bw, w1, h1, 0);
	default:
		;
	}
}

static void Redisplay(Widget w, XEvent *event, Region region)
{
	(*SuperClass->core_class.expose)(w, event, region);
}

static void Resize(Widget w)
{
	(*SuperClass->core_class.resize)(w);
	DoLayout((MwSpinnerWidget)w);
}

static XtGeometryResult GeometryManager(Widget w,
                XtWidgetGeometry *request, XtWidgetGeometry *reply)
{
        return XtGeometryYes;
}

static void ChangeManaged(Widget w)
{
    	DoLayout((MwSpinnerWidget)w);
}

static Boolean SetValues(Widget current, Widget request, Widget new,
		ArgList args, Cardinal *num_args)
{
	set_text((MwSpinnerWidget)new);
    	return False;
}

