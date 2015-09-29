/*
   Copyright (C) 1996-2002  Ulric Eriksson <ulric@siag.nu>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the Licence, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free
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

#include <Mowitz/Mowitz.h>
#include "MwTabbingP.h"

#define offset(field) XtOffsetOf(MwTabbingRec, tabbing.field)
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
		XtNtabbingSelected,
		XtCTabbingSelected,
		XtRInt,
		sizeof(int),
		offset(selected),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNselectCallback,
		XtCSelectCallback,
		XtRCallback,
		sizeof(XtPointer),
		offset(select_callbacks),
		XtRCallback,
		(XtPointer)0
	}, {
		XtNrenameCallback,
		XtCSelectCallback,
		XtRCallback,
		sizeof(XtPointer),
		offset(rename_callbacks),
		XtRCallback,
		(XtPointer)NULL
	}, {
		XtNtabbingTop,
		XtCTabbingTop,
		XtRInt,
		sizeof(int),
		offset(top),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNtabbingCount,
		XtCTabbingCount,
		XtRInt,
		sizeof(int),
		offset(ntext),
		XtRImmediate,
		(XtPointer)0
	}
};
#undef offset

/* methods */
static void Redisplay(Widget, XEvent *, Region);
static void Realize(Widget, XtValueMask *, XSetWindowAttributes *);
static void Destroy(Widget);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static void Initialize(Widget, Widget, ArgList, Cardinal *);

/* actions */
static void TabbingAction(Widget, XEvent *, String *, Cardinal *);

static XtActionsRec actions[] =
{
	{"tabbing", TabbingAction},
};

/* translations */
static char translations[] =
"<Btn1Down>:		tabbing()\n";

MwTabbingClassRec mwTabbingClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) &widgetClassRec,
    /* class_name		*/	"MwTabbing",
    /* widget_size		*/	sizeof(MwTabbingRec),
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
  { /* tabbing fields */
    /* empty			*/	0
  }
};

WidgetClass mwTabbingWidgetClass = (WidgetClass)&mwTabbingClassRec;


/* supporting code copied directly from tabbing.c */

static void rename_tab(MwTabbingWidget tw, int j)
{
	char b[1024];
	int n;

	strcpy(b, tw->tabbing.text[j]);
	n = MwDialogInput((Widget)tw, _("Name:"), b);

	if (n) {
		MwFree(tw->tabbing.text[j]);
		tw->tabbing.text[j] = MwStrdup(b);
		XtCallCallbackList((Widget)tw,
			tw->tabbing.rename_callbacks, (XtPointer)j);
		Redisplay((Widget)tw, NULL, None);
	}
}

static void select_tab(MwTabbingWidget tw, int j)
{
	tw->tabbing.selected = j;
	XtCallCallbackList((Widget)tw,
			tw->tabbing.select_callbacks, (XtPointer)j);
	Redisplay((Widget)tw, NULL, None);
}

static void TabbingAction(Widget w, XEvent *event, String *params, Cardinal *n)
{
	MwTabbingWidget tw = (MwTabbingWidget)w;
	int i, j, text_width;
	XFontStruct *fs = tw->tabbing.font;
	char *word;

	i = 0;
	for (j = tw->tabbing.top; j < tw->tabbing.ntext; j++) {
		word = tw->tabbing.text[j];
		text_width = XTextWidth(fs, word, strlen(word));
		i = i+text_width+16;
		if (event->xbutton.x < i) {
			if (j == tw->tabbing.selected) rename_tab(tw, j);
			else select_tab(tw, j);
			return;
		}
	}
}

static GC get_gc(Widget w)
{
	MwTabbingWidget tw = (MwTabbingWidget)w;
        unsigned long valuemask;
        XGCValues values;
	GC gc;

	values.font = tw->tabbing.font->fid;
	valuemask = GCFont;
        gc = XCreateGC(XtDisplay(w), XtWindow(w),
                                valuemask, &values);
        return gc;
}

static void Initialize(Widget req, Widget new, ArgList args, Cardinal *nargs)
{
	MwTabbingWidget tw = (MwTabbingWidget)new;

	if (req->core.width == 0) new->core.width = 100;
	if (req->core.height == 0) new->core.height = 20;
	tw->tabbing.text = NULL;
}

#define superclass (&coreClassRec)
static void Realize(Widget w, XtValueMask *valueMask,
                XSetWindowAttributes *attributes)
{
        MwTabbingWidget tw = (MwTabbingWidget) w;

        (*superclass->core_class.realize) (w, valueMask, attributes);
        tw->tabbing.tab_gc = get_gc(w);
}

static void Destroy(Widget w)
{
	int i;
        MwTabbingWidget tw = (MwTabbingWidget) w;

        XFreeGC(XtDisplay(w), tw->tabbing.tab_gc);
	for (i = 0; i < tw->tabbing.ntext; i++)
		MwFree(tw->tabbing.text[i]);
	MwFree(tw->tabbing.text);
}

static void Redisplay(Widget w, XEvent *event, Region r)
{
	Pixmap scribble;
	int i, j;
	Dimension h;
	int width, text_width;
	MwTabbingWidget aw = (MwTabbingWidget) w;
	Display *dpy = XtDisplay(w);
	Screen *s = XtScreen(w);
	Window wi = XtWindow(w);
	unsigned long black = BlackPixelOfScreen(s);
	unsigned long white = WhitePixelOfScreen(s);
	unsigned long grey;
	XPoint points[4];
	int selected_i = aw->tabbing.selected;
	XFontStruct *fs = aw->tabbing.font;
	char *word;
	int height = fs->max_bounds.ascent+fs->max_bounds.descent;
	int text_y;

	XtVaGetValues(w,
		XtNbackground, &grey,
		XtNheight, &h,
		(char *)0);
	scribble = XCreatePixmap(dpy, wi, aw->core.width,
				aw->core.height, aw->core.depth);
        XSetForeground(dpy, aw->tabbing.tab_gc, grey);
	XFillRectangle(dpy, scribble, aw->tabbing.tab_gc, 0, 0,
			aw->core.width, aw->core.height);

	text_y = (h+height)/2-fs->max_bounds.descent;
	i = 0;
        for (j = aw->tabbing.top; j < aw->tabbing.ntext; j++) {
		word = aw->tabbing.text[j];
                if (j == aw->tabbing.selected) selected_i = i;
                text_width = XTextWidth(fs, word, strlen(word));
                width = text_width+16;
                points[0].x = i+0;		points[0].y = 0;
                points[1].x = 8;		points[1].y = h-1;
                points[2].x = width-8;		points[2].y = 0;
                points[3].x = 8;		points[3].y = -h+1;
                XSetForeground(dpy, aw->tabbing.tab_gc, grey);
                XFillPolygon(dpy, scribble, aw->tabbing.tab_gc, points, 4,
                        Convex, CoordModePrevious);
                XSetForeground(dpy, aw->tabbing.tab_gc, black);
                XDrawString(dpy, scribble, aw->tabbing.tab_gc, i+12, text_y,
                        word, strlen(word));
                XDrawLines(dpy, scribble, aw->tabbing.tab_gc, points, 4,
                        CoordModePrevious);
                i += width;
        }
	XSetForeground(dpy, aw->tabbing.tab_gc, black);
	XDrawLine(dpy, scribble, aw->tabbing.tab_gc,
		0, 0, aw->core.width, 0);
	if (aw->tabbing.ntext && aw->tabbing.selected >= aw->tabbing.top) {
	        i = selected_i;
	        j = aw->tabbing.selected;
		word = aw->tabbing.text[j];
	        text_width = XTextWidth(fs, word, strlen(word));
	        width = text_width+16;
	        points[0].x = i+0;		points[0].y = 0;
	        points[1].x = 8;		points[1].y = h-1;
	        points[2].x = width-8;		points[2].y = 0;
	        points[3].x = 8;		points[3].y = -h+1;
	        XSetForeground(dpy, aw->tabbing.tab_gc, white);
	        XFillPolygon(dpy, scribble, aw->tabbing.tab_gc, points, 4,
	                Convex, CoordModePrevious);
	        XSetForeground(dpy, aw->tabbing.tab_gc, black);
	        XDrawString(dpy, scribble, aw->tabbing.tab_gc, i+12, text_y,
	                word, strlen(word));
	        XDrawLines(dpy, scribble, aw->tabbing.tab_gc, points, 4,
	                CoordModePrevious);
	}
	XCopyArea(dpy, scribble, wi, aw->tabbing.tab_gc,
		0, 0, aw->core.width, aw->core.height, 0, 0);
	XFreePixmap(dpy, scribble);
}

static Boolean SetValues(Widget current, Widget request, Widget new,
		ArgList args, Cardinal *nargs)
{
	Boolean do_redisplay = True;
	MwTabbingWidget newtw = (MwTabbingWidget)new;

	if (newtw->tabbing.top > newtw->tabbing.ntext-1)
		newtw->tabbing.top = newtw->tabbing.ntext-1;
	if (newtw->tabbing.top < 0)
		newtw->tabbing.top = 0;

	Redisplay(new, NULL, None);
	do_redisplay = False;

	return do_redisplay;
}

/* ---
*/
int MwTabbingInsert(Widget w, char *text, int pos)
{
	int i;
	MwTabbingWidget tw = (MwTabbingWidget)w;

	char **p = (char **)MwMalloc((tw->tabbing.ntext+1)*sizeof(char *));
	if (pos < 0) pos = tw->tabbing.ntext+1+pos;
	if (pos < 0) pos = 0;
	if (pos > tw->tabbing.ntext) pos = tw->tabbing.ntext;
	for (i = 0; i < pos; i++)
		p[i] = tw->tabbing.text[i];
	p[i] = MwStrdup(text);
	for (; i < tw->tabbing.ntext; i++)
		p[i+1] = tw->tabbing.text[i];
	tw->tabbing.ntext++;
	MwFree(tw->tabbing.text);
	tw->tabbing.text = p;
	if (XtIsRealized(w))
		XClearWindow(XtDisplay(w), XtWindow(w));
	return pos;
}

/* ---
*/
void MwTabbingRemove(Widget w, int pos)
{
	int i;
	MwTabbingWidget tw = (MwTabbingWidget)w;

	MwFree(tw->tabbing.text[pos]);
	for (i = pos+1; i < tw->tabbing.ntext; i++)
		tw->tabbing.text[i-1] = tw->tabbing.text[i];
	tw->tabbing.ntext--;
	if (tw->tabbing.top > tw->tabbing.ntext-1)
		tw->tabbing.top = tw->tabbing.ntext-1;
	if (tw->tabbing.top < 0)
		tw->tabbing.top = 0;
	if (XtIsRealized(w))
		XClearWindow(XtDisplay(w), XtWindow(w));
}

/* ---
*/
int MwTabbingTextToPos(Widget w, char *text)
{
	int i;
	MwTabbingWidget tw = (MwTabbingWidget)w;

	for (i = 0; i < tw->tabbing.ntext; i++)
		if (!strcmp(tw->tabbing.text[i], text)) return i;
	return -1;
}

/* ---
*/
char *MwTabbingPosToText(Widget w, int pos)
{
	MwTabbingWidget tw = (MwTabbingWidget)w;

	return tw->tabbing.text[pos];
}


