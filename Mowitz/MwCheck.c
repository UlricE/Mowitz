/*
Copyright (C) 1999-2001  Ulric Eriksson <ulric@siag.nu>

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

#include "MwUtils.h"
#include "MwCheckP.h"

#include "pixmaps/check_motif_off.xpm"
#include "pixmaps/check_motif_on.xpm"
#include "pixmaps/radio_motif_off.xpm"
#include "pixmaps/radio_motif_on.xpm"
#include "pixmaps/tickbox_off.xpm"
#include "pixmaps/tickbox_on.xpm"
#include "pixmaps/radio_16_off.xpm"
#include "pixmaps/radio_16_on.xpm"

#define offset(field) XtOffsetOf(MwCheckRec, check.field)
static XtResource resources[] = {
	{
		XtNfont,		/* name */
		XtCFont,		/* class */
		XtRFontStruct,		/* type */
		sizeof(XFontStruct *),	/* size */
		offset(font),		/* offset */
		XtRString,		/* default_type */
		XtDefaultFont	 	/* default_addr */
	}, {
		XtNstate,
		XtCState,
		XtRBoolean,
		sizeof(Boolean),
		offset(state),
		XtRImmediate,
		(XtPointer)False
	}, {
		XtNcheckStyle,
		XtCCheckStyle,
		XtRInt,
		sizeof(int),
		offset(style),
		XtRImmediate,
		(XtPointer)MwCheckWin
	}, {
		XtNradioStart,
		XtCRadioStart,
		XtRWidget,
		sizeof(Widget),
		offset(start),
		XtRImmediate,
		(Widget)None
	}, {
		XtNradioGroup,
		XtCRadioGroup,
		XtRWidget,
		sizeof(Widget),
		offset(group),
		XtRImmediate,
		(Widget)None
	}, {
		XtNlabel,
		XtCLabel,
		XtRString,
		sizeof(String),
		offset(label),
		XtRImmediate,
		(XtPointer)NULL
	}, {
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
static void Realize(Widget, XtValueMask *, XSetWindowAttributes *);
static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static void RadioResetGroup(Widget, Widget);

/* actions */
static void MwCheckAction(Widget, XEvent *, String *, Cardinal *);

static XtActionsRec actions[] =
{
	{"check", MwCheckAction},
};

/* translations */
static char translations[] =
"<Btn1Down>:		check()\n";

MwCheckClassRec mwCheckClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) &widgetClassRec,
    /* class_name		*/	"MwCheck",
    /* widget_size		*/	sizeof(MwCheckRec),
    /* class_initialize		*/	NULL,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	Realize,
    /* actions			*/	actions,
    /* num_actions		*/	XtNumber(actions),
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	Destroy,
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
  { /* check fields */
    /* empty			*/	0
  }
};

WidgetClass mwCheckWidgetClass = (WidgetClass)&mwCheckClassRec;

/* gw is any widget in the group, pw is where we start searching */
static void RadioResetGroup(Widget gw, Widget pw)
{
	int i;

	if (XtIsSubclass(pw, mwCheckWidgetClass)) {
		MwCheckWidget cw = (MwCheckWidget)pw;
		MwCheckWidget gcw = (MwCheckWidget)gw;
		if (pw != gw && cw->check.group == gcw->check.group)
			XtVaSetValues(pw, XtNstate, False, (char *)0);
	}
	if (XtIsSubclass(pw, compositeWidgetClass)) {
		CompositeWidget cw = (CompositeWidget)pw;
		for (i = 0; i < cw->composite.num_children; i++)
			RadioResetGroup(gw, cw->composite.children[i]);
	}
}

static void MwCheckAction(Widget w, XEvent *event, String *params, Cardinal *n)
{
	MwCheckWidget tw = (MwCheckWidget)w;

	tw->check.state = !tw->check.state;
	if (tw->check.start && tw->check.group)
		RadioResetGroup(w, tw->check.start);
	XtCallCallbackList(w, tw->check.callbacks, (XtPointer)NULL);
	Redisplay(w, NULL, NULL);
}

#define MakePixmap(d,p) \
	result = XpmCreatePixmapFromData(dpy, wi, (d), &(p), NULL, &xa); \
	if (result != XpmSuccess) \
		fprintf(stderr, "XpmCreatePixmapFromData returns %s\n", \
			XpmGetErrorString(result));

static void Initialize(Widget req, Widget new, ArgList args, Cardinal *num)
{
	XpmAttributes xa;
	int result;
	XpmColorSymbol symbol;
	Pixel color;
	MwCheckWidget cw = (MwCheckWidget)new;
	Display *dpy = XtDisplay(new);
	Window wi = XRootWindowOfScreen(XtScreen(new));

	if (cw->core.width == 0) cw->core.width = 80;
	if (cw->core.height == 0) cw->core.height = 20;
	XtVaGetValues(new,
		XtNbackground, &color,
		(char *)0);
	xa.closeness = 40000;
	xa.exactColors = FALSE;
	xa.valuemask = XpmCloseness | XpmExactColors;
	symbol.name = NULL;
	symbol.value = "none";
	symbol.pixel = color;
	xa.colorsymbols = &symbol;
	xa.numsymbols = 1;
	xa.valuemask |= XpmColorSymbols;
	MakePixmap(check_motif_off_xpm, cw->check.cmoff);
	MakePixmap(check_motif_on_xpm, cw->check.cmon);
	MakePixmap(radio_16_off_xpm, cw->check.r16off);
	MakePixmap(radio_16_on_xpm, cw->check.r16on);
	MakePixmap(radio_motif_off_xpm, cw->check.rmoff);
	MakePixmap(radio_motif_on_xpm, cw->check.rmon);
	MakePixmap(tickbox_off_xpm, cw->check.tboff);
	MakePixmap(tickbox_on_xpm, cw->check.tbon);
}

static GC get_gc(Widget w)
{
	MwCheckWidget tw = (MwCheckWidget)w;
        unsigned long valuemask;
        XGCValues values;
	GC gc;

	values.font = tw->check.font->fid;
	valuemask = GCFont;
        gc = XCreateGC(XtDisplay(w), XtWindow(w),
                                valuemask, &values);
        return gc;
}

#define superclass (&coreClassRec)
static void Realize(Widget w, XtValueMask *valueMask,
                XSetWindowAttributes *attributes)
{
        MwCheckWidget tw = (MwCheckWidget) w;

        (*superclass->core_class.realize) (w, valueMask, attributes);
        tw->check.gc = get_gc(w);
}

static void Destroy(Widget w)
{
        MwCheckWidget tw = (MwCheckWidget) w;
	Display *dpy = XtDisplay(w);

        XFreeGC(dpy, tw->check.gc);
	XFreePixmap(dpy, tw->check.cmoff);
	XFreePixmap(dpy, tw->check.cmon);
	XFreePixmap(dpy, tw->check.r16on);
	XFreePixmap(dpy, tw->check.r16off);
	XFreePixmap(dpy, tw->check.rmoff);
	XFreePixmap(dpy, tw->check.rmon);
	XFreePixmap(dpy, tw->check.tbon);
	XFreePixmap(dpy, tw->check.tboff);
}

static void Redisplay(Widget w, XEvent *event, Region r)
{
	Pixmap scribble, icon;
	Dimension h;
	MwCheckWidget aw = (MwCheckWidget) w;
	Display *dpy = XtDisplay(w);
	Screen *s = XtScreen(w);
	Window wi = XtWindow(w);
	unsigned long black = BlackPixelOfScreen(s);
	unsigned long grey;
	XFontStruct *fs = aw->check.font;
	char *word = aw->check.label;
	int asc = fs->max_bounds.ascent;
	int desc = fs->max_bounds.descent;
	int height = asc-desc;
	int y = (aw->core.height+height)/2;

	XtVaGetValues(w,
		XtNbackground, &grey,
		XtNheight, &h,
		(char *)0);
	scribble = XCreatePixmap(dpy, wi, aw->core.width,
				aw->core.height, aw->core.depth);
        XSetForeground(dpy, aw->check.gc, grey);
	XFillRectangle(dpy, scribble, aw->check.gc, 0, 0,
			aw->core.width, aw->core.height);
	XSetForeground(dpy, aw->check.gc, black);

	switch (aw->check.style) {
	case MwCheckWin:
		if (aw->check.state) icon = aw->check.tbon;
		else icon = aw->check.tboff;
		break;
	case MwRadioWin:
		if (aw->check.state) icon = aw->check.r16on;
		else icon = aw->check.r16off;
		break;
	case MwCheckMotif:
		if (aw->check.state) icon = aw->check.cmon;
		else icon = aw->check.cmoff;
		break;
	default:	/* MwRadioMotif */
		if (aw->check.state) icon = aw->check.rmon;
		else icon = aw->check.rmoff;
		break;
	}
	XCopyArea(dpy, icon, scribble, aw->check.gc,
		0, 0, 16, 16, 2, (aw->core.height-16)/2);
	if (word) {
		XDrawString(dpy, scribble, aw->check.gc,
			20, y, word, strlen(word));
	}
	XCopyArea(dpy, scribble, wi, aw->check.gc,
		0, 0, aw->core.width, aw->core.height, 0, 0);
	XFreePixmap(dpy, scribble);
}

static Boolean SetValues(Widget current, Widget request, Widget new,
		ArgList args, Cardinal *nargs)
{
	Boolean do_redisplay = True;
	return do_redisplay;
}

