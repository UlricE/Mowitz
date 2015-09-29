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
#include "MwNotebookP.h"

#define offset(field) XtOffsetOf(MwNotebookRec, notebook.field)
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
		XtNnotebookSelected,
		XtCNotebookSelected,
		XtRInt,
		sizeof(int),
		offset(selected),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNnotebookSelect,
		XtCNotebookSelect,
		XtRPointer,
		sizeof(XtPointer),
		offset(select),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNnotebookTop,
		XtCNotebookTop,
		XtRInt,
		sizeof(int),
		offset(top),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNnotebookCount,
		XtCNotebookCount,
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

/* actions */
static void MwNotebookAction(Widget, XEvent *, String *, Cardinal *);

static XtActionsRec actions[] =
{
	{"notebook", MwNotebookAction},
};

/* translations */
static char translations[] =
"<Btn1Down>:		notebook()\n";

MwNotebookClassRec mwNotebookClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) &widgetClassRec,
    /* class_name		*/	"MwNotebook",
    /* widget_size		*/	sizeof(MwNotebookRec),
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
  { /* notebook fields */
    /* empty			*/	0
  }
};

WidgetClass mwNotebookWidgetClass = (WidgetClass)&mwNotebookClassRec;


/* supporting code copied directly from notebook.c */

static void MwNotebookAction(Widget w, XEvent *event, String *params, Cardinal *n)
{
	MwNotebookWidget tw = (MwNotebookWidget)w;
	int i, j, text_width;
	XFontStruct *fs = tw->notebook.font;
	char *word;

	i = 0;
	for (j = tw->notebook.top; j < tw->notebook.ntext; j++) {
		word = tw->notebook.text[j];
		text_width = XTextWidth(fs, word, strlen(word));
		i = i+text_width+16;
		if (event->xbutton.x < i) {
			if (tw->notebook.select)
				(*tw->notebook.select)(w, j);
			return;
		}
	}
}

static GC get_gc(Widget w)
{
	MwNotebookWidget tw = (MwNotebookWidget)w;
        unsigned long valuemask;
        XGCValues values;
	GC gc;

	values.font = tw->notebook.font->fid;
	values.line_width = 2;

	valuemask = GCFont | GCLineWidth;
        gc = XCreateGC(XtDisplay(w), XtWindow(w),
                                valuemask, &values);
        return gc;
}

#define superclass (&coreClassRec)
static void Realize(Widget w, XtValueMask *valueMask,
                XSetWindowAttributes *attributes)
{
        MwNotebookWidget tw = (MwNotebookWidget) w;

        (*superclass->core_class.realize) (w, valueMask, attributes);
        tw->notebook.tab_gc = get_gc(w);
	tw->notebook.text = NULL;
	tw->notebook.ntext = 0;
}

static void Destroy(Widget w)
{
        MwNotebookWidget tw = (MwNotebookWidget) w;

        XFreeGC(XtDisplay(w), tw->notebook.tab_gc);
}

/* ---

*/

static void Redisplay(Widget w, XEvent *event, Region r)
{
	Pixmap scribble;
	int i, j;
	Dimension h;
	int width, text_width;
	MwNotebookWidget aw = (MwNotebookWidget) w;
	Display *dpy = XtDisplay(w);
	Screen *s = XtScreen(w);
	Window wi = XtWindow(w);
	unsigned long black = BlackPixelOfScreen(s);
	unsigned long white = WhitePixelOfScreen(s);
	unsigned long grey;
	XPoint points[4];
	XFontStruct *fs = aw->notebook.font;
	char *word;
	int height = fs->max_bounds.ascent+fs->max_bounds.descent;
	int text_y;
	int off_y;

	XtVaGetValues(w,
		XtNbackground, &grey,
		XtNheight, &h,
		(char *)0);
	scribble = XCreatePixmap(dpy, wi, aw->core.width,
				aw->core.height, aw->core.depth);
        XSetForeground(dpy, aw->notebook.tab_gc, grey);
	XFillRectangle(dpy, scribble, aw->notebook.tab_gc, 0, 0,
			aw->core.width, aw->core.height);

	text_y = (h+height)/2-fs->max_bounds.descent;
	i = 0;
	XSetForeground(dpy, aw->notebook.tab_gc, white);
	XDrawLine(dpy, scribble, aw->notebook.tab_gc,
		0, h-1, aw->core.width, h-1);
        for (j = aw->notebook.top; j < aw->notebook.ntext; j++) {
		word = aw->notebook.text[j];
                text_width = XTextWidth(fs, word, strlen(word));
                width = text_width+16;
                if (j == aw->notebook.selected) {
			off_y = 0;
			XSetForeground(dpy, aw->notebook.tab_gc, grey);
			XDrawLine(dpy, scribble, aw->notebook.tab_gc,
				i, h-1, i+width, h-1);
		} else {
			off_y = 2;
		}
		points[0].x = i+1;		points[0].y = h-off_y;
		points[1].x = i+1;		points[1].y = off_y+5;
		points[2].x = i+5;		points[2].y = off_y+1;
		points[3].x = i+width-5;	points[3].y = off_y+1;
		XSetForeground(dpy, aw->notebook.tab_gc, white);
		XDrawLines(dpy, scribble, aw->notebook.tab_gc, points, 4,
			CoordModeOrigin);
		points[0].x = i+width-5;	points[0].y = off_y+1;
		points[1].x = i+width-1;	points[1].y = off_y+5;
		points[2].x = i+width-1;	points[2].y = h-off_y;
		XSetForeground(dpy, aw->notebook.tab_gc, black);
		XDrawLines(dpy, scribble, aw->notebook.tab_gc, points, 3,
			CoordModeOrigin);
                XDrawString(dpy, scribble, aw->notebook.tab_gc,
			i+8, text_y+off_y, word, strlen(word));
                i += width;
        }
	XCopyArea(dpy, scribble, wi, aw->notebook.tab_gc,
		0, 0, aw->core.width, aw->core.height, 0, 0);
	XFreePixmap(dpy, scribble);
}

static Boolean SetValues(Widget current, Widget request, Widget new,
		ArgList args, Cardinal *nargs)
{
	Boolean do_redisplay = True;
	MwNotebookWidget newtw = (MwNotebookWidget)new;

	if (newtw->notebook.top > newtw->notebook.ntext-1)
		newtw->notebook.top = newtw->notebook.ntext-1;
	if (newtw->notebook.top < 0)
		newtw->notebook.top = 0;

	Redisplay(new, NULL, None);
	do_redisplay = False;

	return do_redisplay;
}

/* ---
*/
int MwNotebookInsert(Widget w, char *text, int pos)
{
	int i;
	MwNotebookWidget tw = (MwNotebookWidget)w;

	char **p = (char **)MwMalloc((tw->notebook.ntext+1)*sizeof(char *));

	if (pos == -1) pos = tw->notebook.ntext;
	for (i = 0; i < pos; i++)
		p[i] = tw->notebook.text[i];
	p[i] = MwStrdup(text);
	for (; i < tw->notebook.ntext; i++)
		p[i+1] = tw->notebook.text[i];
	tw->notebook.ntext++;

	MwFree(tw->notebook.text);
	tw->notebook.text = p;
	XClearWindow(XtDisplay(w), XtWindow(w));
	return pos;
}

/* ---
*/
void MwNotebookRemove(Widget w, int pos)
{
	int i;
	MwNotebookWidget tw = (MwNotebookWidget)w;

	MwFree(tw->notebook.text[pos]);
	for (i = pos+1; i < tw->notebook.ntext; i++)
		tw->notebook.text[i-1] = tw->notebook.text[i];
	tw->notebook.ntext--;
	if (tw->notebook.top > tw->notebook.ntext-1)
		tw->notebook.top = tw->notebook.ntext-1;
	if (tw->notebook.top < 0)
		tw->notebook.top = 0;
	XClearWindow(XtDisplay(w), XtWindow(w));
}

/* ---
*/
int MwNotebookTextToPos(Widget w, char *text)
{
	int i;
	MwNotebookWidget tw = (MwNotebookWidget)w;

	for (i = 0; i < tw->notebook.ntext; i++)
		if (!strcmp(tw->notebook.text[i], text)) return i;
	return -1;
}

/* ---
*/
char *MwNotebookPosToText(Widget w, int pos)
{
	MwNotebookWidget tw = (MwNotebookWidget)w;

	return tw->notebook.text[pos];
}


