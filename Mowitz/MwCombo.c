/*
Copyright (C) 1998-2002  Ulric Eriksson <ulric@siag.nu>

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
MwCombo.c - MwCombo compound widget

Consists of a composite widget with two children, a Textfield and a
Command. The Command drops down an Override with a Viewport containing
a List.
--- */

#include	<stdio.h>
#include	<X11/keysym.h>
#include	<X11/IntrinsicP.h>
#include	<X11/StringDefs.h>
#include	<X11/Shell.h>
#include	<X11/Xaw/Command.h>
#include	<X11/Xaw/List.h>
#include	<X11/Xaw/Viewport.h>
#include	<X11/Xmu/Misc.h>
#include	<X11/Xaw/Command.h>
#include	<X11/xpm.h>
#include	"MwTextField.h"
#include	"MwComboP.h"
#include	"pixmaps/combo.xpm"

/****************************************************************
 *
 * MwCombo Resources
 *
 ****************************************************************/

#define XtNcomboShell "comboShell"
#define XtCComboShell "ComboShell"

#define offset(field) XtOffsetOf(MwComboRec, combo.field)
static XtResource resources[] = {
	{
		XtNtextCallback,
		XtCTextCallback,
		XtRCallback,
		sizeof(XtPointer),
		offset(text_callbacks),
		XtRCallback,
		(XtPointer)NULL
	}, {
		XtNborderWidth,
		XtCBorderWidth,
		XtRDimension,
		sizeof(Dimension),
		XtOffsetOf(RectObjRec, rectangle.border_width),
		XtRImmediate,
		(XtPointer)0,
	}, {
		XtNshadowWidth,
		XtCShadowWidth,
		XtRDimension,
		sizeof(Dimension),
		XtOffsetOf(MwFrameRec, frame.shadowWidth),
		XtRImmediate,
		(XtPointer)1,
	}, {
		XtNshadowType,
		XtCShadowType,
		XtRShadowType,
		sizeof(XtShadowType),
		XtOffsetOf(MwFrameRec, frame.type),
		XtRImmediate,
		(XtPointer)Lowered
	}, {
		XtNlistCallback,
		XtCListCallback,
		XtRCallback,
		sizeof(XtPointer),
		offset(list_callbacks),
		XtRImmediate,
		(XtPointer)NULL
	}, {
		XtNcomboTop,
		XtCComboTop,
		XtRPointer,
		sizeof(XtPointer),
		offset(topLevel),
		XtRImmediate,
		(XtPointer)None
	}, {
		XtNcomboData,
		XtCComboData,
		XtRPointer,
		sizeof(XtPointer),
		offset(data),
		XtRImmediate,
		(XtPointer)None
	}, {
		XtNcomboNData,
		XtCComboNData,
		XtRInt,
		sizeof(int),
		offset(ndata),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNcomboShell,
		XtCComboShell,
		XtRPointer,
		sizeof(XtPointer),
		offset(shell),
		XtRImmediate,
		(XtPointer)None
	}
};
#undef offset

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/


static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);

static void Resize(Widget);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult GeometryManager(Widget,
		XtWidgetGeometry *, XtWidgetGeometry *);
static void ChangeManaged(Widget);
static void Redisplay();

#define SuperClass ((MwFrameWidgetClass)&mwFrameClassRec)

MwComboClassRec mwComboClassRec = {
  {
/* core_class fields      */
    /* superclass         */    (WidgetClass) SuperClass,
    /* class_name         */    "MwCombo",
    /* widget_size        */    sizeof(MwComboRec),
    /* class_initialize   */    NULL,
    /* class_part_init    */	NULL,
    /* class_inited       */	FALSE,
    /* initialize	  */	Initialize,
    /* initialize_hook    */	NULL,
    /* realize            */    XtInheritRealize,
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
    /* resize             */    /*XtInheritResize*/Resize,
    /* expose             */    /*XtInheritExpose*/Redisplay,
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
    /* extension          */    NULL,
  },{
/* MwCombo class fields */
    /* empty		  */	0
  }
};

WidgetClass mwComboWidgetClass = (WidgetClass)&mwComboClassRec;


/****************************************************************
 *
 * Private Routines
 *
 ****************************************************************/

static enum {WAITING, ABORT, DONE} combo_status;

/* ---
This callback is called when an item on the list is clicked.
   It transfers control to the list_cb function with the clicked string
   as argument.
*/

static void combo_list_select(Widget w,
		XtPointer client_data, XtPointer call_data)
{
	MwComboWidget cw = (MwComboWidget)client_data;
	XawListReturnStruct *list_struct = (XawListReturnStruct *)call_data;
	String string = list_struct->string;
	MwTextFieldSetString(cw->combo.text, string);
	XtCallCallbackList((Widget)cw, cw->combo.list_callbacks,
		(XtPointer)string);
	combo_status = DONE;
}

/* ---
This callback is called when the command button is clicked.
   It pops up the list and blocks waiting for a selection to be made.
*/

static void combo_list_popup(Widget w,
		XtPointer client_data, XtPointer call_data)
{
	MwComboWidget cw = (MwComboWidget)XtParent(w);
	int x, y, xe, ye;
	Window child;
	Dimension height, width;

	if (cw->combo.shell == None) {
	}

	combo_status = WAITING;
	XtVaGetValues(XtParent(w),
		XtNheight, &height,
		XtNwidth, &width,
		(char *)0);
	XTranslateCoordinates(XtDisplay(w),
		XtWindow(XtParent(w)), DefaultRootWindow(XtDisplay(w)),
		0, height,
		&x, &y,
		&child);
	XtVaSetValues(cw->combo.shell,
		XtNx, x,
		XtNy, y,
		XtNwidth, width,
		XtNheight, 200,
		XtNborderWidth, 1,
		(char *)0);
	XawListChange(cw->combo.list, cw->combo.data, cw->combo.ndata, 0, True);
	XtPopup(cw->combo.shell, XtGrabNonexclusive);
	while (combo_status == WAITING) {
		XEvent event;
		XtAppNextEvent(XtWidgetToApplicationContext(w), &event);
		if (event.type == ButtonPress) {
			xe = event.xbutton.x_root;
			ye = event.xbutton.y_root;
			if (xe <= x || xe >= (x+width)
			    || ye <= y || ye >= (y+200))
				combo_status = ABORT;
		} else if (event.type == KeyPress) {
			combo_status = ABORT;
		}
		XtDispatchEvent(&event);
	}
	XtPopdown(cw->combo.shell);
}


/* ---
This event handler is triggered when the user clicks in the text widget.
   It allows editing the text until Esc or Return is pressed.
   Pressing Return invokes the function text_cb with the text
   as argument.
*/

static void combo_text_edit(Widget w, XtPointer p, XEvent *event, Boolean *n)
{
	MwComboWidget cw = (MwComboWidget)XtParent(w);
	char *old = MwTextFieldGetString(w);

	XtAddGrab(w, True, False);
	if (cw->combo.topLevel != None) {
		XtSetKeyboardFocus(cw->combo.topLevel, w);
	}
	XtVaSetValues(w,
		XtNdisplayCaret, True,
		(char *)0);
	combo_status = WAITING;
	while (combo_status == WAITING) {
		int count, bufsiz = 10;
		XEvent event;
		XtAppNextEvent(XtWidgetToApplicationContext(w), &event);
		if (event.type == KeyPress) {
			char buf[12];
			KeySym keysym;
			XKeyEvent kevent = event.xkey;
			count = XLookupString(&kevent,
				buf, bufsiz, &keysym, NULL);
			if (keysym == XK_Escape) combo_status = ABORT;
			else if (keysym == XK_Return) combo_status = DONE;
			else XtDispatchEvent(&event);
		} else {
			XtDispatchEvent(&event);
		}
	}
	XtVaSetValues(w,
		XtNdisplayCaret, False,
		(char *)0);
	XtRemoveGrab(w);
	if (cw->combo.topLevel) {
		XtSetKeyboardFocus(cw->combo.topLevel, None);
	}
	if (combo_status == ABORT)
		MwTextFieldSetString(w, old);
	XtCallCallbackList((Widget)cw, cw->combo.text_callbacks,
		(XtPointer)MwTextFieldGetString(w));
}

static void Initialize(Widget req, Widget new, ArgList args, Cardinal *num)
{
	XpmAttributes xa;
	Pixmap pm_return;
	int result;
	XpmColorSymbol symbol;
	Pixel color;
	MwComboWidget cw = (MwComboWidget)new;

	if (req->core.width == 0) new->core.width = 80;
	if (req->core.height == 0) new->core.height = 20;
	/* the next line is to let the Frame resize */
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
	result = XpmCreatePixmapFromData(XtDisplay(new),
		XRootWindowOfScreen(XtScreen(new)),
		combo_xpm, &pm_return, NULL, &xa);
	if (result != XpmSuccess) {
		fprintf(stderr, "XpmCreatePixmapFromData returns %s\n",
			XpmGetErrorString(result));
	}
	cw->combo.text = XtVaCreateManagedWidget("combo_text",
		mwTextfieldWidgetClass, new,
		XtNdisplayCaret, False,
		(char *)0);
	XtAddEventHandler(cw->combo.text,
		ButtonPressMask, False, combo_text_edit, NULL);

	cw->combo.cmd = XtVaCreateManagedWidget("combo_cmd",
		commandWidgetClass, new,
		XtNbitmap, pm_return,
		XtNforeground, color,
		(char *)0);
	XtAddCallback(cw->combo.cmd, XtNcallback, combo_list_popup, NULL);

	cw->combo.shell = XtVaCreateManagedWidget("combo_shell",
		overrideShellWidgetClass, new,
		(char *)0);
	cw->combo.viewport = XtVaCreateManagedWidget("combo_viewport",
		viewportWidgetClass, cw->combo.shell,
		XtNheight, 200,
		XtNallowHoriz, True,
		XtNallowVert, True,
		XtNuseBottom, True,
		XtNuseRight, True,
		(char *)0);
	cw->combo.list = XtVaCreateManagedWidget("combo_list",
		listWidgetClass, cw->combo.viewport,
		XtNdefaultColumns, 1,
		XtNforceColumns, True,
		(char *)0);
	XtAddCallback(cw->combo.list,
		XtNcallback, combo_list_select, new);
}

static void Destroy(Widget w)
{
	MwComboWidget cw = (MwComboWidget)w;
	XtDestroyWidget(cw->combo.list);
	XtDestroyWidget(cw->combo.viewport);
	XtDestroyWidget(cw->combo.shell);
	XtDestroyWidget(cw->combo.cmd);
	XtDestroyWidget(cw->combo.text);
}

static void Redisplay(Widget w, XEvent *event, Region region)
{
	(*SuperClass->core_class.expose)(w, event, region);
}

/* ---
Do a layout, actually assigning positions.
*/

static void DoLayout(MwComboWidget sw)
{
	Widget tw, cw;
	Dimension w1, w2, h1;

	int bw = /*sw->core.border_width+*/sw->frame.shadowWidth;
	w2 = sw->core.width-2*bw;
	w1 = w2-16;
	h1 = sw->core.height-2*bw;

	if (w1 < 2) w1 = 2;

	switch (sw->composite.num_children) {
	case 3:
	case 2:
		cw = sw->composite.children[1];
		XtConfigureWidget(cw, w1+bw, bw, w2-w1, h1, 0);
	case 1:
		tw = sw->composite.children[0];
		XtConfigureWidget(tw, bw, bw, w1, h1, 0);
	default:
		;
	}
}

/*
 * Actually layout combo
 */

static void Resize(Widget w)
{
	(*SuperClass->core_class.resize)(w);
	DoLayout((MwComboWidget)w);
} /* Resize */

/* ---
Geometry Manager

'reply' is unused; we say only yeay or nay, never almost.
*/

static XtGeometryResult GeometryManager(Widget w,
		XtWidgetGeometry *request, XtWidgetGeometry *reply)
{
	return XtGeometryYes;
}

static void ChangeManaged(Widget w)
{
    DoLayout((MwComboWidget)w);
}

static Boolean SetValues(Widget current, Widget request, Widget new,
		ArgList args, Cardinal *num_args)
{
    return False;
}

/* ---
Change the text in the Textfield.

This public function is semi-necessary because the Textfield doesn't
like to have its text changed with XtVaSetValues.
*/

void MwComboTextChange(Widget w, char *p)
{
	MwComboWidget cw;
	if (w == None) return;
	cw = (MwComboWidget)w;
	if (p == NULL) p = "";
	MwTextFieldSetString(cw->combo.text, p);
}

