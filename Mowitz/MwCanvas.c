/*
Copyright (C) 1996-2001  Ulric Eriksson <ulric@siag.nu>

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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <X11/xpm.h>

#include "MwCanvasP.h"

#define DEFAULT_HEIGHT 50
#define DEFAULT_WIDTH 50

#define offset(field) XtOffsetOf(MwCanvasRec, canvas.field)
static XtResource resources[] = {
	{
		XtNcallback,
		XtCCallback,
		XtRCallback,
		sizeof(XtPointer),
		offset(callbacks),
		XtRCallback,
		(XtPointer)NULL
	}
};
#undef offset

/* methods */
static void Redisplay(Widget, XEvent *, Region);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static void Initialize(Widget, Widget, ArgList, Cardinal *);

/* actions */
static void MwCanvasAction(Widget, XEvent *, String *, Cardinal *);

static XtActionsRec actions[] =
{
	{"canvas", MwCanvasAction},
};

/* translations */
static char translations[] =
"<Key>:		canvas()\n";

#define SuperClass ((CoreWidgetClass)&coreClassRec)

MwCanvasClassRec mwCanvasClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) SuperClass,
    /* class_name		*/	"MwCanvas",
    /* widget_size		*/	sizeof(MwCanvasRec),
    /* class_initialize		*/	NULL,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	XtInheritRealize,
    /* actions			*/	actions,
    /* num_actions		*/	XtNumber(actions),
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	NULL,
    /* resize			*/	NULL,
    /* expose			*/	Redisplay,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	translations,
    /* query_geometry		*/	XtInheritQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  { /* canvas fields */
    /* empty			*/	0
  }
};

WidgetClass mwCanvasWidgetClass = (WidgetClass)&mwCanvasClassRec;


/* supporting code copied directly from canvas.c */

static void MwCanvasAction(Widget w, XEvent *event, String *params, Cardinal *n)
{
	;
}

static void Initialize(Widget req, Widget new, ArgList args, Cardinal *nargs)
{
	MwCanvasWidget cw = (MwCanvasWidget)new;

	if (cw->core.height == 0) cw->core.height = DEFAULT_HEIGHT;
	if (cw->core.width == 0) cw->core.width = DEFAULT_WIDTH;

/*
widgetClassRec.core_class.resize((Widget)cw);
	(*XtClass(new)->core_class.resize)((Widget)cw);
*/
}

static void Redisplay(Widget w, XEvent *event, Region r)
{
	MwCanvasWidget aw = (MwCanvasWidget) w;

	XtCallCallbackList(w, aw->canvas.callbacks, NULL);
}

static Boolean SetValues(Widget current, Widget request, Widget new,
		ArgList args, Cardinal *nargs)
{
	Boolean do_redisplay = True;

	return do_redisplay;
}

