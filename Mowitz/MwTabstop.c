/*
Copyright (C) 2000-2001  Ulric Eriksson <ulric@siag.nu>

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

#include "MwUtils.h"
#include "MwXutils.h"
#include <X11/xpm.h>

#include "MwTabstopP.h"

static float floatOne = 1.0;

#define offset(field) XtOffsetOf(MwTabstopRec, mwTabstop.field)
static XtResource resources[] = {
	{
		XtNleftMargin,
		XtCLeftMargin,
		XtRInt,
		sizeof(int),
		offset(left_margin),
		XtRImmediate,
		(XtPointer)72
	}, {
		XtNrightMargin,
		XtCRightMargin,
		XtRInt,
		sizeof(int),
		offset(right_margin),
		XtRImmediate,
		(XtPointer)72
	}, {
		XtNpaperWidth,
		XtCPaperWidth,
		XtRInt,
		sizeof(int),
		offset(paper_width),
		XtRImmediate,
		(XtPointer)595
	}, {
		XtNtabstopTopCol,
		XtCTabstopTopCol,
		XtRInt,
		sizeof(int),
		offset(top_col),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNzoom,
		XtCZoom,
		XtRFloat,
		sizeof(float),
		offset(zoom),
		XtRFloat,
		(XtPointer)&floatOne
	}, {
		XtNfont,
		XtCFont,
		XtRFontStruct,
		sizeof(XFontStruct *),
		offset(font),
		XtRString,
		XtDefaultFont
	}, {
		XtNtabs,
		XtCTabs,
		XtRString,
		sizeof(String),
		offset(tabs),
		XtRImmediate,
		(XtPointer)0
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
static void Realize(Widget w, XtValueMask *, XSetWindowAttributes *);
static void Redisplay(Widget, XEvent *, Region);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);

/* actions */
static void Notify(Widget, XEvent *, String *, Cardinal *);
static void DeleteTab(Widget, XEvent *, String *, Cardinal *);
static void AddTab(Widget, XEvent *, String *, Cardinal *);

static XtActionsRec actions[] =
{
	{"notify", Notify},
	{"add_tab", AddTab},
	{"delete_tab", DeleteTab},
};

/* translations */
static char translations[] =
"Shift<Btn1Down>:		delete_tab() notify()\n"
":<Btn1Down>:			add_tab(l) notify()\n"
":<Btn2Down>:			add_tab(c) notify()\n"
":<Btn3Down>:			add_tab(r) notify()\n"
;

MwTabstopClassRec mwTabstopClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) &widgetClassRec,
    /* class_name		*/	"MwTabstop",
    /* widget_size		*/	sizeof(MwTabstopRec),
    /* class_initialize		*/	NULL,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	NULL,
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
  { /* mwTabstop fields */
    /* empty			*/	0
  }
};

WidgetClass mwTabstopWidgetClass = (WidgetClass)&mwTabstopClassRec;


MwTabstop MwTabstopNextStop(Widget w, int t)
{
	MwTabstopWidget rw = (MwTabstopWidget)w;
	if (w == None) return MwNextTab(NULL, t+1);
	return MwNextTab(rw->mwTabstop.tt, t);
}

static void Notify(Widget w, XEvent *event, String *params, Cardinal *n)
{
	MwTabstopWidget rw = (MwTabstopWidget)w;
	XtCallCallbackList(w, rw->mwTabstop.callbacks, (XtPointer)rw->mwTabstop.tabs);
}

static void DeleteTab(Widget w, XEvent *event, String *params, Cardinal *n)
{
	MwTabstopWidget rw = (MwTabstopWidget)w;
	float zoom = rw->mwTabstop.zoom;
	int lm = rw->mwTabstop.left_margin, tc = rw->mwTabstop.top_col, ps = lm-tc+1;
	int x = (event->xbutton.x)/zoom-ps;
	int i;
	char *p = rw->mwTabstop.tabs;
	MwTabstop *tt = rw->mwTabstop.tt;
	/* no need to reallocate the tab string */
	for (i = 0; tt[i].j && tt[i].x < x; i++) {
		sprintf(p, "%c%d ", tt[i].j, tt[i].x);
		p += strlen(p);
	}
	if (tt[i].j) i++;
	while (tt[i].j) {
		sprintf(p, "%c%d ", tt[i].j, tt[i].x);
		p += strlen(p);
		i++;
	}
	MwFree(rw->mwTabstop.tt);
	rw->mwTabstop.tt = MwGetTabs(rw->mwTabstop.tabs);
	XClearWindow(XtDisplay(w), XtWindow(w));
	Redisplay(w, NULL, None);
}

static void AddTab(Widget w, XEvent *event, String *params, Cardinal *n)
{
	MwTabstopWidget rw = (MwTabstopWidget)w;
	float zoom = rw->mwTabstop.zoom;
	int lm = rw->mwTabstop.left_margin, tc = rw->mwTabstop.top_col, ps = lm-tc+1;
	int x = (event->xbutton.x)/zoom-ps;
	int i;
	char *p;
	MwTabstop *tt = rw->mwTabstop.tt;

	p = MwMalloc(strlen(rw->mwTabstop.tabs)+10);
	MwFree(rw->mwTabstop.tabs);
	rw->mwTabstop.tabs = p;
	for (i = 0; tt[i].j && tt[i].x < x; i++) {
		sprintf(p, "%c%d ", tt[i].j, tt[i].x);
		p += strlen(p);
	}
	sprintf(p, "%c%d ", params[0][0], x);
	p += strlen(p);
	while (tt[i].j) {
		sprintf(p, "%c%d ", tt[i].j, tt[i].x);
		p += strlen(p);
		i++;
	}
	MwFree(rw->mwTabstop.tt);
	rw->mwTabstop.tt = MwGetTabs(rw->mwTabstop.tabs);
	XClearWindow(XtDisplay(w), XtWindow(w));
	Redisplay(w, NULL, None);
}

#define superclass (&coreClassRec)

static void Realize(Widget w, XtValueMask *v, XSetWindowAttributes *a)
{
	MwTabstopWidget rw = (MwTabstopWidget)w;
	unsigned long mask = 0;
	XGCValues values;
	Display *dpy = XtDisplay(w);
	unsigned long grey;
	(*superclass->core_class.realize)(w, v, a);
	values.font = rw->mwTabstop.font->fid;
	mask = GCFont;
	rw->mwTabstop.gc = XCreateGC(dpy, XtWindow(w), mask, &values);
	if (rw->mwTabstop.tabs == NULL) {
		rw->mwTabstop.tabs = MwStrdup("l36 l72 l108 l144 l180 l216 "
					     "l252 l288 l324 l360 l396 l432");
		rw->mwTabstop.tt = MwGetTabs(rw->mwTabstop.tabs);
	}
	XtVaGetValues(w, XtNbackground, &grey, (char *)0);
	rw->mwTabstop.ctab = MwLoadPixmap(dpy, grey, "ctab.xpm");
	rw->mwTabstop.ltab = MwLoadPixmap(dpy, grey, "ltab.xpm");
	rw->mwTabstop.rtab = MwLoadPixmap(dpy, grey, "rtab.xpm");
}

static void Redisplay(Widget w, XEvent *event, Region r)
{
	MwTabstopWidget aw = (MwTabstopWidget)w;
	int h = aw->core.height;
	int w1 = aw->core.width;
	Display *dpy = XtDisplay(w);
	Screen *s = XtScreen(w);
	Window wi = XtWindow(w);
	int i, o, x;
	GC gc = aw->mwTabstop.gc;
	unsigned long black = BlackPixelOfScreen(s);
	unsigned long white = WhitePixelOfScreen(s);
	float zoom = aw->mwTabstop.zoom;
	int pw = aw->mwTabstop.paper_width,
	    lm = aw->mwTabstop.left_margin, rm = aw->mwTabstop.right_margin,
	    tc = aw->mwTabstop.top_col, ps = lm-tc+1, pe = pw-rm-tc, in;
	Pixmap pm;
	Pixmap scribble;

	scribble = XCreatePixmap(dpy, wi, w1, h, aw->core.depth);
	if (scribble == None) return;

	XSetForeground(dpy, gc, aw->core.background_pixel);
	XFillRectangle(dpy, scribble, gc, 0, 0, w1, h);

	XSetForeground(dpy, gc, white);

	XFillRectangle(dpy, scribble, gc, zoom*ps, 5, zoom*(pw-lm-rm), h-10);
	o = -1;

	XSetForeground(dpy, gc, black);

	/* draw tab stops */
	if (aw->mwTabstop.tt == NULL) aw->mwTabstop.tt = MwGetTabs(aw->mwTabstop.tabs);
	for (i = 0; aw->mwTabstop.tt[i].j; i++) {
		switch (aw->mwTabstop.tt[i].j) {
		case 'c':	pm = aw->mwTabstop.ctab; break;
		case 'r':	pm = aw->mwTabstop.rtab; break;
		default:	pm = aw->mwTabstop.ltab; break;
		}

		x = aw->mwTabstop.tt[i].x;
		XCopyArea(dpy, pm, scribble, gc, 0, 0, 9, 5,
			zoom*(ps+x)-4+o, h-5);
	}

	/* draw half inch markers */
	for (i = ps+36; i < pe; i += 72) {
		XDrawLine(dpy, scribble, gc, zoom*i+o, 9, zoom*i+o, h-10);
	}

	/* draw inch numbers */
	for (i = ps+72, in = 1; i < pe; i += 72, in++) {
		char num[10];
		int nw;
		sprintf(num, "%d", in);
		nw = XTextWidth(aw->mwTabstop.font, num, strlen(num));
		XDrawString(dpy, scribble, gc, zoom*i-nw/2, h-9, num, strlen(num));
	}
	XDrawLine(dpy, scribble, gc, zoom*-tc, 0, zoom*-tc, h-1);
	XDrawLine(dpy, scribble, gc, zoom*(pw-tc-1), 0, zoom*(pw-tc-1), h-1);
	XDrawLine(dpy, scribble, gc, 0, 0, 0, h-1);
	XDrawLine(dpy, scribble, gc, 0, 0, w1-1, 0);
	XDrawLine(dpy, scribble, gc, 0, h-1, w1-1, h-1);
	XDrawLine(dpy, scribble, gc, w1-1, 0, w1-1, h-1);

	XCopyArea(dpy, scribble, wi, gc, 0, 0, w1, h, 0, 0);
	XFreePixmap(dpy, scribble);
}

static Boolean SetValues(Widget current, Widget request, Widget new,
		ArgList args, Cardinal *nargs)
{
	MwTabstopWidget rw1 = (MwTabstopWidget)current;
	MwTabstopWidget rw2 = (MwTabstopWidget)new;
	Boolean do_redisplay =
			(rw1->mwTabstop.tabs != rw2->mwTabstop.tabs)
			|| strcmp(rw1->mwTabstop.tabs, rw2->mwTabstop.tabs)
			|| (rw1->mwTabstop.zoom != rw2->mwTabstop.zoom)
			|| (rw1->mwTabstop.top_col != rw2->mwTabstop.top_col)
			|| (rw1->mwTabstop.left_margin != rw2->mwTabstop.left_margin)
			|| (rw1->mwTabstop.right_margin != rw2->mwTabstop.right_margin)
			|| (rw1->mwTabstop.paper_width != rw2->mwTabstop.paper_width);
	if (rw1->mwTabstop.tabs != rw2->mwTabstop.tabs) {
		MwFree(rw1->mwTabstop.tabs);
		rw2->mwTabstop.tabs = MwStrdup(rw2->mwTabstop.tabs);
		MwFree(rw1->mwTabstop.tt);
		rw2->mwTabstop.tt = MwGetTabs(rw2->mwTabstop.tabs);
	}

	return do_redisplay;
}

void MwTabstopSetZoom(Widget w, float zoom)
{
	MwTabstopWidget rw = (MwTabstopWidget)w;

	if (zoom < .1) zoom = .1;
	if (zoom > 10) zoom = 10;
	if (zoom != rw->mwTabstop.zoom) {
		rw->mwTabstop.zoom = zoom;
		XClearWindow(XtDisplay(w), XtWindow(w));
		Redisplay(w, NULL, None);
	}
}

MwTabstop *MwGetTabs(char *p)
{
        char *q;
        MwTabstop *tt;
        int i = 0;

        p = MwStrdup(p);
        tt = MwMalloc((strlen(p)/3+1)*sizeof *tt);
        for (q = strtok(p, " "); q; q = strtok(NULL, " ")) {
                tt[i].j = q[0];
                tt[i].x = atoi(q+1);
                i++;
        }
        tt[i].j = 0;
        MwFree(p);
        return tt;
}

MwTabstop MwNextTab(MwTabstop *tt, int t)
{
        int i;
        MwTabstop def = {'l', 0};

        for (i = 0; tt && tt[i].j; i++)
                if (tt[i].x > t) return tt[i];
        def.x = t;
        return def;
}

