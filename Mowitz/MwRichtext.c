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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <sys/stat.h>

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <Mowitz/Mowitz.h>
#include <X11/xpm.h>
#include "MwRichtextP.h"

static void plugin_coords(Widget, XtPointer, int *, int *);
static int default_row_height(XtPointer, int);
static int default_adj_horiz(XtPointer, int);
static int default_style(XtPointer, int);
static MwRichchar *default_text(XtPointer, int);
static Boolean default_bop(XtPointer, int);

static float floatOne = 1.0;

#define offset(field) XtOffsetOf(MwRichtextRec, richtext.field)
static XtResource resources[] = {
	{
		XtNrichtextTopRow,	/* name */
		XtCRichtextTopRow,	/* class */
		XtRInt,			/* type */
		sizeof(int),		/* size */
		offset(top_row),	/* offset */
		XtRImmediate,		/* default_type */
		(XtPointer)0		/* default_addr */
	}, {
		XtNrichtextTopCol,
		XtCRichtextTopCol,
		XtRInt,
		sizeof(int),
		offset(top_col),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNrichtextSelectTopRow,
		XtCRichtextSelectTopRow,
		XtRInt,
		sizeof(int),
		offset(sel_top_row),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNrichtextSelectTopCol,
		XtCRichtextSelectTopCol,
		XtRInt,
		sizeof(int),
		offset(sel_top_col),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNrichtextSelectBottomRow,
		XtCRichtextSelectBottomRow,
		XtRInt,
		sizeof(int),
		offset(sel_bottom_row),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNrichtextSelectBottomCol,
		XtCRichtextSelectBottomCol,
		XtRInt,
		sizeof(int),
		offset(sel_bottom_col),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNrichtextPointRow,
		XtCRichtextPointRow,
		XtRInt,
		sizeof(int),
		offset(point_row),
		XtRImmediate,
		(XtPointer)1
	}, {
		XtNrichtextPointCol,
		XtCRichtextPointCol,
		XtRInt,
		sizeof(int),
		offset(point_col),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNrichtextRowHeight,
		XtCRichtextRowHeight,
		XtRPointer,
		sizeof(XtPointer),
		offset(row_height),
		XtRImmediate,
		(XtPointer)default_row_height
	}, {
		XtNrichtextAdjHoriz,
		XtCRichtextAdjHoriz,
		XtRPointer,
		sizeof(XtPointer),
		offset(adj_horiz),
		XtRImmediate,
		(XtPointer)default_adj_horiz
	}, {
		XtNrichtextStyle,
		XtCRichtextStyle,
		XtRPointer,
		sizeof(XtPointer),
		offset(style),
		XtRImmediate,
		(XtPointer)default_style
	}, {
		XtNrichtextBop,
		XtCRichtextBop,
		XtRPointer,
		sizeof(XtPointer),
		offset(bop),
		XtRImmediate,
		(XtPointer)default_bop
	}, {
		XtNrichtextText,
		XtCRichtextText,
		XtRPointer,
		sizeof(XtPointer),
		offset(text),
		XtRImmediate,
		(XtPointer)default_text
	}, {
		XtNrichtextData,
		XtCRichtextData,
		XtRPointer,
		sizeof(XtPointer),
		offset(data),
		XtRImmediate,
		(XtPointer) NULL
	}, {
		XtNrichtextRedisplay,
		XtCRichtextRedisplay,
		XtRBoolean,
		sizeof(Boolean),
		offset(redisplay),
		XtRImmediate,
		(XtPointer)False
	}, {
		XtNrichtextVisibleCursor,
		XtCRichtextVisibleCursor,
		XtRBoolean,
		sizeof(Boolean),
		offset(visible_cursor),
		XtRImmediate,
		(XtPointer)True
	}, {
		XtNrichtextPaperWidth,
		XtCRichtextPaperWidth,
		XtRInt,
		sizeof(int),
		offset(paper_width),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNrichtextLeftMargin,
		XtCRichtextLeftMargin,
		XtRInt,
		sizeof(int),
		offset(left_margin),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNrichtextRightMargin,
		XtCRichtextRightMargin,
		XtRInt,
		sizeof(int),
		offset(right_margin),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNrichtextPluginCoords,
		XtCRichtextPluginCoords,
		XtRPointer,
		sizeof(XtPointer),
		offset(plugin_coords),
		XtRImmediate,
		(XtPointer)plugin_coords
	}, {
		XtNzoom,
		XtCZoom,
		XtRFloat,
		sizeof(float),
		offset(zoom),
		XtRFloat,
		(XtPointer)&floatOne
	}, {
		XtNrichtextDelay,
		XtCRichtextDelay,
		XtRInt,
		sizeof(int),
		offset(delay),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNrichtextFormat,
		XtCRichtextFormat,
		XtRInt,
		sizeof(int),
		offset(format),
		XtRImmediate,
		(XtPointer)-1
	}, {
		XtNrichtextString,
		XtCRichtextString,
		XtRPointer,
		sizeof(XtPointer),
		offset(rc_string),
		XtRImmediate,
		(XtPointer)NULL
	}, {
		XtNrichtextRuler,
		XtCRichtextRuler,
		XtRWidget,
		sizeof(Widget),
		offset(ruler),
		XtRImmediate,
		(XtPointer)None,
	}
};
#undef offset

/* methods */
static void DoLayout(MwRichtextWidget);
static void Resize(Widget);
static XtGeometryResult GeometryManager(Widget,
		XtWidgetGeometry *, XtWidgetGeometry *);
static void ChangeManaged(Widget);
static void Redisplay(Widget, XEvent *, Region);
static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Realize(Widget, XtValueMask *, XSetWindowAttributes *);
static void Destroy(Widget);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);

static int default_row_height(XtPointer p, int i)
{
	MwRichtextWidget rtw = (MwRichtextWidget)p;

	if (rtw->richtext.rc_string == NULL ||
	    rtw->richtext.rc_string[0].c == '\0')
		return 20;
	return MwRcStrheight(rtw->richtext.rc_string, -1);
}

static int default_adj_horiz(XtPointer p, int i)
{
	return MW_HADJ_LEFT;
}

static int default_style(XtPointer p, int i)
{
	return 0;
}

static MwRichchar *default_text(XtPointer p, int i)
{
	MwRichtextWidget rw = (MwRichtextWidget)p;
	if (i != 1) return NULL;
	return rw->richtext.rc_string;
}

/* by default, do not display Beginning Of Paragraph marks */
static Boolean default_bop(XtPointer p, int i)
{
	return False;
}

static void plugin_coords(Widget w, XtPointer p, int *x, int *y)
{
	*x = *y = 0;
}

/* actions */
static void MoveForwardChar(Widget, XEvent *, String *, Cardinal *);
static void MoveBackwardChar(Widget, XEvent *, String *, Cardinal *);
static void MoveToLineStart(Widget, XEvent *, String *, Cardinal *);
static void MoveToLineEnd(Widget, XEvent *, String *, Cardinal *);
static void DeleteForwardChar(Widget, XEvent *, String *, Cardinal *);
static void DeleteBackwardChar(Widget, XEvent *, String *, Cardinal *);
static void InsertChar(Widget, XEvent *, String *, Cardinal *);
static void DeleteToEnd(Widget, XEvent *, String *, Cardinal *);
static void DeleteHighlighted(Widget, XEvent *, String *, Cardinal *);
static void SelectStart(Widget, XEvent *, String *, Cardinal *);
static void ExtendAdjust(Widget, XEvent *, String *, Cardinal *);
static void ExtendEnd(Widget, XEvent *, String *, Cardinal *);
static void InsertSelection(Widget, XEvent *, String *, Cardinal *);

static XtActionsRec actions[] =
{
	{"forward-character",		MoveForwardChar},
	{"backward-character",		MoveBackwardChar},
	{"beginning-of-line",		MoveToLineStart},
	{"end-of-line",			MoveToLineEnd},
	{"delete-next-character",	DeleteForwardChar},
	{"delete-previous-character",	DeleteBackwardChar},
	{"insert-char",			InsertChar},
	{"delete-to-end",		DeleteToEnd},
	{"delete-highlighted",		DeleteHighlighted},
	{"select-start",		SelectStart},
	{"extend-adjust",		ExtendAdjust},
	{"extend-end",			ExtendEnd},
	{"insert-selection",		InsertSelection},
};

/* translations */
static char translations[] =
"<Key>Home:		beginning-of-line() \n"
":<Key>KP_Home:		beginning-of-line() \n"
"Ctrl<Key>a:		beginning-of-line() \n"
"<Key>End:		end-of-line() \n"
":<Key>KP_End:		end-of-line() \n"
"Ctrl<Key>e:		end-of-line() \n"
"<Key>Right:		forward-character() \n"
":<Key>KP_Right:	forward-character() \n"
"Ctrl<Key>f:		forward-character() \n"
"<Key>Left:		backward-character() \n"
":<Key>KP_Left:		backward-character() \n"
"Ctrl<Key>b:		backward-character() \n"
"<Key>Delete:		delete-next-character() \n"
":<Key>KP_Delete:	delete-next-character() \n"
"Ctrl<Key>d:		delete-next-character() \n"
"<Key>BackSpace:	delete-previous-character() \n"
"Ctrl<Key>k:		delete-to-end() \n"
"Ctrl<Key>w:		delete-highlighted() \n"
"<Key>:			insert-char()\n"
"<Btn1Down>:		select-start() \n"
"<Btn1Motion>:		extend-adjust() \n"
"<Btn1Up>:		extend-end() \n"
"<Btn2Down>:		insert-selection() \n"
;

MwRichtextClassRec mwRichtextClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) &compositeClassRec,
    /* class_name		*/	"MwRichtext",
    /* widget_size		*/	sizeof(MwRichtextRec),
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
    /* resize			*/	Resize,
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
  },{
/* composite_class fields */
    /* geometry_manager   */    GeometryManager,
    /* change_managed     */    ChangeManaged,
    /* insert_child       */    XtInheritInsertChild,
    /* delete_child       */    XtInheritDeleteChild,
    /* extension          */    NULL
  }, { /* richtext fields */
    /* empty			*/	0
  }
};

WidgetClass mwRichtextWidgetClass = (WidgetClass)&mwRichtextClassRec;


static int ret_style(MwRichtextWidget rtw, int row)
{
	if (rtw->richtext.style)
		return (*rtw->richtext.style)(rtw->richtext.data, row);
	return MW_STY_DEFAULT;
}

static MwRichchar *ret_text(MwRichtextWidget rtw, int row)
{
	if (rtw->richtext.text)
		return (*rtw->richtext.text)(rtw->richtext.data, row);
	return NULL;
}


static void MoveForwardChar(Widget w, XEvent *event, String *params, Cardinal *n)
{
	MwRichtextWidget rw = (MwRichtextWidget)w;
	int i = rw->richtext.point_col;
	MwRichchar *p = rw->richtext.rc_string;

	if (p[i].c) i++;
	rw->richtext.point_col = i;
	Redisplay(w, NULL, None);
}

static void MoveBackwardChar(Widget w, XEvent *event, String *params, Cardinal *n)
{
	MwRichtextWidget rw = (MwRichtextWidget)w;
	int i = rw->richtext.point_col;

	if (i) i--;
	rw->richtext.point_col = i;
	Redisplay(w, NULL, None);
}

static void MoveToLineStart(Widget w, XEvent *event, String *params, Cardinal *n)
{
	MwRichtextWidget rw = (MwRichtextWidget)w;
	rw->richtext.point_col = 0;
	Redisplay(w, NULL, None);
}

static void MoveToLineEnd(Widget w, XEvent *event, String *params, Cardinal *n)
{
	MwRichtextWidget rw = (MwRichtextWidget)w;
	rw->richtext.point_col = MwRcStrlen(rw->richtext.rc_string);
	Redisplay(w, NULL, None);
}

static void DeleteForwardChar(Widget w, XEvent *event, String *params, Cardinal *n)
{
	MwRichtextWidget rw = (MwRichtextWidget)w;
	int i = rw->richtext.point_col;
	MwRichchar *p = rw->richtext.rc_string;
	if (!p[i].c) return;

	MwRcStrcpy(p+i, p+i+1);
	Redisplay(w, NULL, None);
}

static void DeleteBackwardChar(Widget w, XEvent *event, String *params, Cardinal *n)
{
	MwRichtextWidget rw = (MwRichtextWidget)w;
	int i = rw->richtext.point_col;
	MwRichchar *p = rw->richtext.rc_string;

	if (!i) return;

	i--;
	MwRcStrcpy(p+i, p+i+1);
	rw->richtext.point_col = i;
	Redisplay(w, NULL, None);
}

static void InsertChar(Widget w, XEvent *event, String *params, Cardinal *n)
{
	MwRichtextWidget rw = (MwRichtextWidget)w;
	int count, bufsiz = 100;
	unsigned char buf[120];
	MwRichchar *rp;
	KeySym keysym;

	count = MwRichtextLookupString(w, event, buf, bufsiz, &keysym);
	if (keysym >= ' ' && count == 1) {
		buf[count] = '\0';
		rp = MwRcMakerich(buf, rw->richtext.format);
		MwRichtextInsertText(w, rp, count);
		MwFree(rp);
		rw->richtext.point_col += count;
		Redisplay(w, NULL, None);
	}
}

static void DeleteToEnd(Widget w, XEvent *event, String *params, Cardinal *n)
{
	MwRichtextWidget rw = (MwRichtextWidget)w;
	int i = rw->richtext.point_col;
	MwRichchar *p = rw->richtext.rc_string;

	p[i].c = '\0';
	Redisplay(w, NULL, None);
}

static void DeleteHighlighted(Widget w, XEvent *event, String *params, Cardinal *n)
{
	MwRichtextWidget rw = (MwRichtextWidget)w;
	MwRichchar *p = rw->richtext.rc_string;

	if (rw->richtext.sel_top_col < 0) return;
	MwRcStrcpy(p+rw->richtext.sel_top_col, p+rw->richtext.sel_bottom_col);
	rw->richtext.point_col = rw->richtext.sel_top_col;
	rw->richtext.sel_top_col = rw->richtext.sel_bottom_col = -1;

	Redisplay(w, NULL, None);
}

static void SelectStart(Widget w, XEvent *event, String *params, Cardinal *n)
{
	/*RichtextWidget rw = (RichtextWidget)w;*/

	Redisplay(w, NULL, None);
}

static void ExtendAdjust(Widget w, XEvent *event, String *params, Cardinal *n)
{
	/*RichtextWidget rw = (RichtextWidget)w;*/

	Redisplay(w, NULL, None);
}

static void ExtendEnd(Widget w, XEvent *event, String *params, Cardinal *n)
{
	/*RichtextWidget rw = (RichtextWidget)w;*/

	Redisplay(w, NULL, None);
}

static void InsertSelection(Widget w, XEvent *event, String *params, Cardinal *n)
{
	/*RichtextWidget rw = (RichtextWidget)w;*/

	Redisplay(w, NULL, None);
}

static GC get_gc(Widget w, unsigned long fg, unsigned long bg, Font font)
{
        unsigned long valuemask = 0;
        XGCValues values;
        GC gc = XCreateGC(XtDisplay(w), XtWindow(w), valuemask, &values);

        XSetForeground(XtDisplay(w), gc, fg);
        XSetBackground(XtDisplay(w), gc, bg);
        if (font != -1)
                XSetFont(XtDisplay(w), gc, font);
        return gc;
}

static MwFmt fmt0 = {
        "Helvetica",    /* font family */
        120,            /* size in decipoints */
        False,          /* bold */
        False,          /* italic */
        False,          /* underline */
        False,          /* strikethrough */
        "black",        /* foreground */
        "white",        /* background */
        0,              /* borders */
        MW_VADJ_CENTER, /* vertical adjust */
        MW_HADJ_LEFT,   /* horizontal adjust */
        0};             /* style */

#define superclass (&coreClassRec)
static void Realize(Widget w, XtValueMask *valueMask,
		XSetWindowAttributes *attributes)
{
	MwRichtextWidget rtw = (MwRichtextWidget)w;
	unsigned long fg, bg, blockbg;
	XColor screen_color, exact_color;
	Display *dpy = XtDisplay(w);
	char *name, *class;
	XIMStyles *styles;
	int i;

	if (rtw->richtext.format == -1)
		rtw->richtext.format = MwEncodeFormat(~0, &fmt0);
	(*superclass->core_class.realize)(w, valueMask, attributes);
	fg = BlackPixelOfScreen(XtScreen(w));
	bg = rtw->core.background_pixel;
	XAllocNamedColor(dpy,
		DefaultColormap(dpy, DefaultScreen(dpy)),
		"grey", &screen_color, &exact_color);
	blockbg = screen_color.pixel;
	rtw->richtext.clear_gc = get_gc(w, bg, fg, -1);
	rtw->richtext.cell_gc = get_gc(w, fg, blockbg,
			-1/*get_font(0)*/);
	rtw->richtext.block_gc = get_gc(w, blockbg, bg, -1);
	rtw->richtext.cursor_gc = get_gc(w, fg^bg, 0, -1);
	XSetFunction(dpy, rtw->richtext.cursor_gc, GXxor);
        XSetLineAttributes(dpy, rtw->richtext.cursor_gc,
			1, LineSolid, CapButt, JoinMiter);

#ifdef HAVE_XCREATEIC
	/* Set up input methods */
	XtGetApplicationNameAndClass(dpy, &name, &class);
	rtw->richtext.xim = XOpenIM(dpy, XtDatabase(dpy), name, class);
	if (rtw->richtext.xim) {
		XGetIMValues(rtw->richtext.xim,
			XNQueryInputStyle, &styles,
			(char *)0);
		for (i = 0; i < styles->count_styles; i++) {
			if (styles->supported_styles[i] ==
			    (XIMPreeditNothing | XIMStatusNothing)) break;
		}
		if (i == styles->count_styles) i = 0;
		rtw->richtext.xic = XCreateIC(rtw->richtext.xim,
			XNInputStyle, styles->supported_styles[i],
			XNClientWindow, XtWindow(w),
			(char *)0);
	}
#endif
}

static void Initialize(Widget request, Widget new, ArgList args, Cardinal *n)
{
	MwRichtextWidget rtw = (MwRichtextWidget)new;

	rtw->richtext.timeout = None;
	rtw->richtext.plain = NULL;
	if (rtw->richtext.data == NULL)
		rtw->richtext.data = (XtPointer)new;
	if (rtw->core.width == 0) rtw->core.width = 200;
	if (rtw->core.height == 0) rtw->core.height = 22;
}

static void Destroy(Widget w)
{
	MwRichtextWidget rtw = (MwRichtextWidget)w;

	XFreeGC(XtDisplay(w), rtw->richtext.clear_gc);
	XFreeGC(XtDisplay(w), rtw->richtext.cell_gc);
	XFreeGC(XtDisplay(w), rtw->richtext.cursor_gc);
	XFreeGC(XtDisplay(w), rtw->richtext.block_gc);
#ifdef HAVE_XCREATEIC
	if (rtw->richtext.xim) {
		XDestroyIC(rtw->richtext.xic);
		XCloseIM(rtw->richtext.xim);
	}	
#endif
	if (rtw->richtext.timeout)
		XtRemoveTimeOut(rtw->richtext.timeout);
}

static Dimension row_height(MwRichtextWidget rtw, int row)
{
	if (rtw->richtext.row_height)
		return (*rtw->richtext.row_height)(rtw->richtext.data, row);
	return 20;
}

/* ---
forget about x for now
*/

/* ---
Check if we are in the block.
*/

static int inblock(MwRichtextWidget rtw, int r, int c)
{
	return ((r > rtw->richtext.sel_top_row ||
		 (r == rtw->richtext.sel_top_row &&
		  c >= rtw->richtext.sel_top_col)) &&
		 (r < rtw->richtext.sel_bottom_row ||
		  (r == rtw->richtext.sel_bottom_row &&
		   c <= rtw->richtext.sel_bottom_col)));
}

static int ret_hadj(MwRichtextWidget rtw, int row)
{
	if (rtw->richtext.adj_horiz)
		return (*rtw->richtext.adj_horiz)(rtw->richtext.data, row);
	return MW_HADJ_LEFT;
}


static int segment_char2coords(MwRichtextWidget rtw, float *x,
	MwRichchar *line, int length, int extra_space, int no_of_blanks,
	int tabmode, int col)
{
	int i;
	float tw;
	MwRichchar c;

	tw = MwRcStrwidth(line, length);
        switch (tabmode) {
        case 'r':
                *x -= tw;
                break;
        case 'c':
                *x -= tw/2;
		break;
	default:        /* left or full */
                break;
        }
        for (i = 0; i < length; i++) {
		if (i >= col) return 1;

                c = line[i];
                if (isspace(c.c)) c.c = ' ';

		if (c.c == ' ' && extra_space > 0 && no_of_blanks > 0) {
                        float xx = extra_space/no_of_blanks;
                        *x += xx;
                        extra_space -= xx;
                        no_of_blanks--;
                }
		*x += MwRcWidth(c);
	}
	return 0;
}

/*
Function converts character coordinates to screen-ready pixel coordinates.
That means: zoomed + top added.
*/
void MwRichtextCharToCoords(MwRichtextWidget rtw,
		int cell_row, int cell_col,
		int *cell_x, int *cell_y)
{
	int r;
	MwTabstop mt;
	int lm = rtw->richtext.left_margin;
	int rm = rtw->richtext.right_margin;
	int pw = rtw->richtext.paper_width;
	float tw;
	int nb, i = 0;
	MwRichchar c;
	MwRichchar *line = ret_text(rtw, cell_row);
	int n;
	int hadj;
	int ss, nt;
	float zoom = rtw->richtext.zoom;
	float fx = 0;

        *cell_y = 0;
	for (r = 1; r < cell_row; r++)
		*cell_y += row_height(rtw, r);

	fx = lm;
	hadj = ret_hadj(rtw, cell_row);
	if (!line) goto Done;
	if (hadj == MW_HADJ_CENTER) {
		fx = lm+(pw-lm-rm)/2;
		segment_char2coords(rtw, &fx, line, MwRcStrlen(line),
				0, 0, 'c', cell_col);
	} else if (hadj == MW_HADJ_RIGHT) {
		fx = pw-rm;
		segment_char2coords(rtw, &fx, line, MwRcStrlen(line),
				0, 0, 'r', cell_col);
	} else {
		mt.x = 0;
		mt.j = 'l';
		ss = nt = 0;
		while (line[nt].c && line[nt].c != '\t') nt++;
		while (line[nt].c == '\t') {
			n = segment_char2coords(rtw, &fx, line+ss, nt-ss,
				0, 0, mt.j, cell_col-ss);
			if (n) goto Done;
			mt = MwTabstopNextStop(rtw->richtext.ruler, fx-lm);
			fx = mt.x+lm;
			ss = ++nt;
			while (line[nt].c && line[nt].c != '\t') nt++;
		}
		tw = 0;
		nb = 0;
		if (hadj == MW_HADJ_FULL &&
		    !rtw->richtext.bop(rtw->richtext.data, cell_row+1)) {
			for (i = 0; line[i].c; i++) {
                                c = line[i];
                                if (isspace(c.c)) nb++;
                                tw += MwRcWidth(c);
                        }
			tw = pw-lm-rm-tw;
		}
		n = segment_char2coords(rtw, &fx, line+ss, nt-ss,
				tw, nb, mt.j, cell_col-ss);
	}
Done:

	fx -= rtw->richtext.top_col;
	*cell_y -= rtw->richtext.top_row;
	fx *= zoom;
	*cell_y *= zoom;
	*cell_x = fx;
}

/*
Returns 1 if the position we're about to draw a character at is to the
right of the one we're looking for. The result is returned in col, which
is relative to the start of the segment.
*/
static int segment_coords2char(MwRichtextWidget rtw, float *x, int tx,
	MwRichchar *line, int length, int extra_space, int no_of_blanks,
	int tabmode, int *col)
{
	float tw;
	MwRichchar c;

	*col = 0;
	tw = MwRcStrwidth(line, length);
        switch (tabmode) {
        case 'r':
                *x -= tw;
                break;
        case 'c':
                *x -= tw/2;
		break;
	default:        /* left or full */
                break;
        }
        for (*col = 0; *col < length; (*col)++) {
		if (*x > tx) {
			return 1;
		}

                c = line[*col];
                if (isspace(c.c)) c.c = ' ';

		if (c.c == ' ' && extra_space > 0 && no_of_blanks > 0) {
                        float xx = extra_space/no_of_blanks;
                        *x += xx;
                        extra_space -= xx;
                        no_of_blanks--;
                }
		*x += MwRcWidth(c);
	}
	return 0;
}

/* ---
*/
void MwRichtextCoordsToChar(MwRichtextWidget rtw,
		int *cur_row, int *cur_col,
		int cur_x, int cur_y)
{
	MwTabstop mt;
	int lm = rtw->richtext.left_margin;
	int rm = rtw->richtext.right_margin;
	int pw = rtw->richtext.paper_width;
	int tw, nb, i = 0;
	MwRichchar c;
	MwRichchar *line;
	int n;
	int hadj;
	int ss, nt;
	float cell_x = 0;
	int cell_y = 0;
	float zoom = rtw->richtext.zoom;

	cur_x /= zoom;
	cur_y /= zoom;
	cur_x += rtw->richtext.top_col-2;
	cur_y += rtw->richtext.top_row;

	for (*cur_row = 1; cell_y < cur_y; *cur_row += 1)
		cell_y += row_height(rtw, *cur_row);
	if (*cur_row > 1) *cur_row -= 1;

	*cur_col = 0;

	line = ret_text(rtw, *cur_row);
	cell_x = lm;
	hadj = ret_hadj(rtw, *cur_row);
	if (!line) return;
	if (hadj == MW_HADJ_CENTER) {
		cell_x = lm+(pw-lm-rm)/2;
		n = segment_coords2char(rtw, &cell_x, cur_x,
				line, MwRcStrlen(line),
				0, 0, 'c', cur_col);
	} else if (hadj == MW_HADJ_RIGHT) {
		cell_x = pw-rm;
		n = segment_coords2char(rtw, &cell_x, cur_x,
				line, MwRcStrlen(line),
				0, 0, 'r', cur_col);
	} else {
		mt.x = 0;
		mt.j = 'l';
		ss = nt = 0;
		while (line[nt].c && line[nt].c != '\t') nt++;
		while (line[nt].c == '\t') {
			n = segment_coords2char(rtw, &cell_x, cur_x,
				line+ss, nt-ss,
				0, 0, mt.j, cur_col);
			if (n) return;
			mt = MwTabstopNextStop(rtw->richtext.ruler, cell_x-lm);
			cell_x = mt.x+lm;
			ss = ++nt;
			while (line[nt].c && line[nt].c != '\t') nt++;
		}
		tw = 0;
		nb = 0;
		if (hadj == MW_HADJ_FULL &&
		    !rtw->richtext.bop(rtw->richtext.data, *cur_row+1)) {
			for (i = 0; line[i].c; i++) {
                                c = line[i];
                                if (isspace(c.c)) nb++;
                                tw += MwRcWidth(c);
                        }
			tw = pw-lm-rm-tw;
		}
		n = segment_coords2char(rtw, &cell_x, cur_x,
				line+ss, nt-ss,
				tw, nb, mt.j, cur_col);
	}
}

static void update_ruler(Widget w)
{
	MwRichtextWidget rtw = (MwRichtextWidget)w;

	if (rtw->richtext.ruler) {
		XtVaSetValues(rtw->richtext.ruler,
			XtNtabstopTopCol, rtw->richtext.top_col,
			XtNleftMargin, rtw->richtext.left_margin,
			XtNrightMargin, rtw->richtext.right_margin,
			XtNpaperWidth, rtw->richtext.paper_width,
			(char *)0);
		MwTabstopSetZoom(rtw->richtext.ruler, rtw->richtext.zoom);
	}
}

/*
Change top_row and top_col so that point is visible.
*/
static Boolean move_top(MwRichtextWidget rtw)
{
	Boolean pr_scr_flag = False;
	int cur_x, cur_y, h;
	int width, height;
	float zoom = rtw->richtext.zoom;
	int otc = rtw->richtext.top_col;

	/* Figure out how big the window is */
	width = rtw->core.width;	/* unzoomed size */
	height = rtw->core.height;
	MwRichtextCharToCoords(rtw,
		rtw->richtext.point_row, rtw->richtext.point_col,
		&cur_x, &cur_y);
	/* cur_x and cur_y are in zoomed pixels */

	if (cur_x < 0) {
		/* must move grid to the right, i.e. make top_col smaller */
		rtw->richtext.top_col += cur_x;
		pr_scr_flag = TRUE;
	} else if (cur_x+1 > width) {
		/* move grid to the left, i.e. make top_col larger */
		rtw->richtext.top_col += cur_x+1-width;
		pr_scr_flag = TRUE;
	}
	h = zoom*row_height(rtw, rtw->richtext.point_row)+4;
	if (cur_y < 0) {
		rtw->richtext.top_row += cur_y;
		/* move grid down, i.e. make top_row smaller */
		pr_scr_flag = TRUE;
	} else if (cur_y+h > height) {
		/* move grid up, i.e. make top_col larger */
		rtw->richtext.top_row += cur_y+h-height;
		pr_scr_flag = TRUE;
	}
	if (rtw->richtext.top_row < 0) rtw->richtext.top_row = 0;
	if (rtw->richtext.top_col < 0) rtw->richtext.top_col = 0;
	if (otc != rtw->richtext.top_col) update_ruler((Widget)rtw);
	return pr_scr_flag;
}

static void toggle_cursor(MwRichtextWidget rtw)
{
	int cur_x, cur_y;
	int height = row_height(rtw, rtw->richtext.point_row);
	float zoom = rtw->richtext.zoom;

	MwRichtextCharToCoords(rtw,
		rtw->richtext.point_row, rtw->richtext.point_col,
		&cur_x, &cur_y);
	XDrawLine(XtDisplay((Widget)rtw), XtWindow((Widget)rtw),
		rtw->richtext.cursor_gc, cur_x, cur_y+zoom*height/4,
                cur_x,
		cur_y+zoom*5*height/4);
}


static void draw_segment(MwRichtextWidget rtw, Drawable cell_win,
		float *x_base, int y_base, int yo, MwRichchar *line, int length,
		int extra_space, int no_of_blanks, int tabmode, int row)
{
	int of = -1;
	MwFmt fmt;
	XColor color;
	int i;
	Display *display = XtDisplay(rtw);
	float zoom = rtw->richtext.zoom;
	GC gc = rtw->richtext.cell_gc;
	int tw;
	MwRichchar c;
	int tc = -rtw->richtext.top_col;
	int height = row_height(rtw, row);

	tw = MwRcStrwidth(line, length);
	switch (tabmode) {
	case 'r':
		*x_base -= tw;
		break;
	case 'c':
		*x_base -= tw/2;
		break;
	default:	/* left or full */
		break;
	}
        for (i = 0; i < length; i++) {
                float width;

		c = line[i];
		if (isspace(c.c)) c.c = ' ';

		width = MwRcWidth(c);
		if (inblock(rtw, row, i)) {
			XFillRectangle(display, cell_win,
				rtw->richtext.block_gc,
				zoom*(*x_base+tc),
				y_base+zoom*(yo-3*height/4),
				ceil(zoom*width),
				zoom*(height));
		}

		if (c.fmt != of) {
			of = c.fmt;
			MwDecodeFormat(of, ~0, &fmt);
			MwAllocNamedColor(display, fmt.fg, &color);
			XSetForeground(display, gc, color.pixel);
		}
		MwRcStrdraw(cell_win, gc, 0, y_base, *x_base+tc, yo, &c, 1, zoom);
		if (c.c == ' ' && extra_space > 0 && no_of_blanks > 0) {
			float x = extra_space/no_of_blanks;
			*x_base += x;
			extra_space -= x;
			no_of_blanks--;
		}
                if (fmt.uline) {
                        XDrawLine(display, cell_win, gc,
                                zoom*(*x_base+tc), y_base+zoom*(yo+1),
                                zoom*(*x_base+tc+width), y_base+zoom*(yo+1));
                }
                if (fmt.strike) {
                        XDrawLine(display, cell_win, gc,
                                zoom*(*x_base+tc), y_base+zoom*(yo-4),
                                zoom*(*x_base+tc+width), y_base+zoom*(yo-4));
                }
                *x_base += width;
        }
}

/*
This does tabs like Word Perfect, which isn't perfect but better than nothing.
It goes like this:

0. Get the tab table: an array of tuples {justification, position}
   where justification is one of center, left, right and position
   is the horizontal position in unzoomed pixels. I'll simulate
   the tabs for now, as pw currently has only left tabs with fixed distance.
1. Split the line into tab-separated segments. Most lines don't contain
   any tabs, so there is only one segment.
2. If the line justification is center or right, tabs are ignored, i.e.
   treated as blanks. I am not entirely happy with this part.
3. Otherwise start by drawing the first segment flush left. Each tab
   skips to the first tab stop to the right of the last character.
4. Each tab skips to the first tab stop to the right of the last
   character.
5. Start over at 3 by drawing the next segment. This time the segment
   may be justified left, center or right.

In case we are doing "full" justification, the last segment
must be treated specially. The spaces must be laid out so that we get
flush left *and* right edges.

1. Calculate the total width of all the words, including the blanks.
   Also count the blanks.
2. Calculate the extra space that needs to be laid out.
3. Draw the text as usual, but for each blank adjust the spacing. Add
   extra_space/no_of_blanks, then subtract that amount
   from extra_space and subtract 1 from no_of_blanks.
4. Draw the first word left-adjusted at the tab stop where we left off.
5. If there are more words, precede each with the amount of space from 3.
*/
/* y_base is in zoomed coordinates. yo is unzoomed */
static void draw_line(MwRichtextWidget rtw, Drawable cell_win,
		int y_base, int yo, int row, int clr)
{
        int height = row_height(rtw, row);
        float x_base = 0;
	Display *display = XtDisplay(rtw);
        MwRichchar *line = ret_text(rtw, row);
	float zoom = rtw->richtext.zoom;
	int ss, nt, hadj;
	MwTabstop mt;
	int lm = rtw->richtext.left_margin;
	int rm = rtw->richtext.right_margin;
	int pw = rtw->richtext.paper_width;
	float tw;
	int nb, i = 0;
	MwRichchar c;

        if (clr) {
                /* the 4 is from trial and error; should be from font metrics */
                XClearArea(display, cell_win, 0, y_base+zoom*(yo+4),
                                rtw->core.width, zoom*height, FALSE);
        }

        if (!line) return;
        x_base = lm;

        if (ret_style(rtw, row) == MW_STY_EMBED) {
#if 0
                char *p = (char *)MwRcMakeplain(line);
                embed_draw(cell_win, x_base, y_base, p);
                MwFree(p);
#else
		fprintf(stderr, "No, no, no.\n"
			"Nothing should be using this any more\n");
#endif
                return;         /* done */
        }


	hadj = ret_hadj(rtw, row);
	if (hadj == MW_HADJ_CENTER) {
		x_base = lm+(pw-lm-rm)/2;
		draw_segment(rtw, cell_win, &x_base, y_base, yo+height,
				line, MwRcStrlen(line), 0, 0, 'c', row);
	} else if (hadj == MW_HADJ_RIGHT) {
		x_base = pw-rm;
		draw_segment(rtw, cell_win, &x_base, y_base, yo+height,
				line, MwRcStrlen(line), 0, 0, 'r', row);
	} else {
		mt.x = 0;
		mt.j = 'l';
		ss = nt = 0;
		while (line[nt].c && line[nt].c != '\t') nt++;
		while (line[nt].c == '\t') {
			draw_segment(rtw, cell_win, &x_base, y_base, yo+height,
					line+ss, nt-ss, 0, 0, mt.j, row);
			mt = MwTabstopNextStop(rtw->richtext.ruler, x_base-lm);
			x_base = mt.x+lm;
			ss = ++nt;
			while (line[nt].c && line[nt].c != '\t') nt++;
		}
		tw = 0;
		nb = 0;
		if (hadj == MW_HADJ_FULL &&
		    !rtw->richtext.bop(rtw->richtext.data, row+1)) {
			for (i = 0; line[i].c; i++) {
				c = line[i];
				if (isspace(c.c)) nb++;
				tw += MwRcWidth(c);
			}
			tw = pw-lm-rm-tw;
		}

		draw_segment(rtw, cell_win, &x_base, y_base, yo+height,
				line+ss, nt-ss, tw, nb, mt.j, row);
	}
}

void MwRichtextDrawLine(Widget w, int row)
{
        Window cell_win = XtWindow(w);
        MwRichtextWidget rtw = (MwRichtextWidget)w;
        int y_base = 0;
	int r;

	for (r = 1; r < row; r++)
		y_base += row_height(rtw, r);
	y_base -= rtw->richtext.top_row;

	if (rtw->richtext.visible_cursor)
		toggle_cursor(rtw);
        draw_line(rtw, cell_win, 0, y_base, row, TRUE);
	if (rtw->richtext.visible_cursor)
		toggle_cursor(rtw);
}

void MwRichtextDraw(MwRichtextWidget rtw, Drawable d)
{
        int i, c0, x_base, y_base, y, h;
        int height;
	float zoom = rtw->richtext.zoom;

	/* The following line serves absolutely no purpose,
	except that it magically prevents Siag Office from
	crashing on Solaris. */
	fflush(stdout);

	MwRichtextCoordsToChar(rtw, &i, &c0, 0, 0);	/* where to start */
	if (i > 1) i--;		/* just in case */
	height = rtw->core.height;		/* zoomed height */
	MwRichtextCharToCoords(rtw, i, 0, &x_base, &y_base);
	h = row_height(rtw, i);
	y = 0;
	while (y_base+zoom*y < height) {
		draw_line(rtw, d, y_base, y, i, FALSE);
		y += row_height(rtw, i);
		i++;
	}
}

/* ---
Draw onto a pixmap.
*/

Pixmap richtext_pixmap(MwRichtextWidget rtw)
{
        int width, height, depth;
        Pixmap scribble;
        Widget w = (Widget)rtw;

	width = rtw->core.width;
	height = rtw->core.height;
	depth = rtw->core.depth;
	if (width > 2000 || height > 2000) return None;

        scribble = XCreatePixmap(XtDisplay(w), XtWindow(w),
                width, height, depth);
        XFillRectangle(XtDisplay(w), scribble, rtw->richtext.clear_gc,
                0, 0, width, height);
        MwRichtextDraw(rtw, scribble);
        return scribble;
}

static void do_redisplay(XtPointer client_data, XtIntervalId *id)
{
	Widget w = (Widget)client_data;
        Pixmap scribble;
        GC gc;
        unsigned long valuemask = 0;
        XGCValues values;
	MwRichtextWidget rtw = (MwRichtextWidget)w;

	scribble = richtext_pixmap(rtw);
        if (scribble == None) return;
        gc = XCreateGC(XtDisplay(w), XtWindow(w),
                        valuemask, &values);
        XCopyArea(XtDisplay(w), scribble, XtWindow(w),
                gc, 0, 0, rtw->core.width, rtw->core.height, 0, 0);
        XFreePixmap(XtDisplay(w), scribble);
        XFreeGC(XtDisplay(w), gc);
	if (rtw->richtext.visible_cursor) {
		toggle_cursor(rtw);
	}
	/* update plugin positions */
	DoLayout(rtw);
	
	/* clear the timeout */
	rtw->richtext.timeout = None;
	update_ruler(w);
}

static void Redisplay(Widget w, XEvent *event, Region r)
{
	MwRichtextWidget tw = (MwRichtextWidget)w;
	if (tw->richtext.timeout) return;	/* already set */
	if (tw->richtext.delay) {
		tw->richtext.timeout = XtAppAddTimeOut(
				XtWidgetToApplicationContext(w),
				tw->richtext.delay, do_redisplay, (XtPointer)w);
		update_ruler(w);
	} else {
		do_redisplay((XtPointer)w, NULL);
	}
}

static Boolean SetValues(Widget current, Widget request, Widget new,
		ArgList args, Cardinal *nargs)
{
	MwRichtextWidget currtw = (MwRichtextWidget)current;
	MwRichtextWidget newrtw = (MwRichtextWidget)new;
	Boolean do_redisplay = False;

	do_redisplay = (currtw->richtext.sel_top_row != newrtw->richtext.sel_top_row
		|| currtw->richtext.sel_top_col != newrtw->richtext.sel_top_col
		|| currtw->richtext.sel_bottom_row != newrtw->richtext.sel_bottom_row
		|| currtw->richtext.sel_bottom_col != newrtw->richtext.sel_bottom_col
		|| currtw->richtext.top_row != newrtw->richtext.top_row
		|| currtw->richtext.top_col != newrtw->richtext.top_col
		|| currtw->richtext.zoom != newrtw->richtext.zoom);
	if (newrtw->richtext.visible_cursor &&
	    (newrtw->richtext.point_row != currtw->richtext.point_row ||
	     newrtw->richtext.point_col != currtw->richtext.point_col ||
	     newrtw->richtext.redisplay))
		do_redisplay |= move_top(newrtw);

	if (newrtw->richtext.redisplay) {
		do_redisplay = True;
		newrtw->richtext.redisplay = False;
	}

	/* can't let Xt handle this because it will flicker */
	if (do_redisplay) {
		Redisplay((Widget)newrtw, NULL, None);
		do_redisplay = False;
	} else {
		if (currtw->richtext.visible_cursor)
			toggle_cursor(currtw);
		if (newrtw->richtext.visible_cursor)
			toggle_cursor(newrtw);
	}

	return do_redisplay;
}

/*
 * Do a layout, actually assigning positions.
 *
 * This function uses a callback from the application to get the
 * position of each plugin. In the case of PW, that callback uses
 * richtext_char2coords. See pw/window.c.
 */

static void DoLayout(MwRichtextWidget sw)
{
        int i;
        int x, y, top_x = 0, top_y = 0;

        if (sw->composite.num_children) {
		top_x = sw->richtext.top_col;
		top_y = sw->richtext.top_row;
        }
        for (i = 0; i < sw->composite.num_children; i++) {
                (*sw->richtext.plugin_coords)(sw->composite.children[i],
                        sw->richtext.data, &x, &y);
                XtMoveWidget(sw->composite.children[i], x, y);
        }
}

/*
 * Actually layout the table
 */

static void Resize(Widget w)
{
        DoLayout((MwRichtextWidget)w);
}

/*
 * Geometry Manager
 */

static XtGeometryResult GeometryManager(Widget w,
                XtWidgetGeometry *request, XtWidgetGeometry *reply)
{
        return XtGeometryYes;
}

static void ChangeManaged(Widget w)
{
    DoLayout((MwRichtextWidget)w);
}

int MwRichtextLookupString(Widget w, XEvent *event, char *buf,
			 int bufsiz, KeySym *keysym)
{
#ifdef HAVE_XCREATEIC
	Status status;
	if (((MwRichtextWidget)w)->richtext.xim)
		return XmbLookupString(((MwRichtextWidget)w)->richtext.xic,
					(XKeyEvent *)event, (char *)buf, bufsiz,
					keysym, &status);
	else	
	        return XLookupString((XKeyEvent *)event, (char *)buf, bufsiz,
					keysym, NULL);
#else
        return XLookupString((XKeyEvent *)event, (char *)buf, bufsiz,
				keysym, NULL);
#endif
}

void MwRichtextSetZoom(Widget w, float zoom)
{
	MwRichtextWidget rw = (MwRichtextWidget)w;

	if (zoom < .1) zoom = .1;
	if (zoom > 10) zoom = 10;
	if (rw->richtext.zoom != zoom) {
		rw->richtext.zoom = zoom;
		Redisplay(w, NULL, None);
	}
}

/*
Functions that allow this widget to replace TextField with a minimum of effort.
*/
void MwRichtextSetString(Widget w, char *p)
{
	MwRichtextWidget rw = (MwRichtextWidget)w;

	MwFree(rw->richtext.rc_string);
	rw->richtext.rc_string = MwRcMakerich((unsigned char *)p,
						rw->richtext.format);
	Redisplay(w, NULL, None);
}

char *MwRichtextGetString(Widget w)
{
	MwRichtextWidget rw = (MwRichtextWidget)w;
	MwFree(rw->richtext.plain);
	rw->richtext.plain = (char *)MwRcMakeplain(rw->richtext.rc_string);
	return rw->richtext.plain;
}

void MwRichtextInsertText(Widget w, MwRichchar *buf, int count)
{
	MwRichtextWidget rw = (MwRichtextWidget)w;
	int i = rw->richtext.point_col;
	MwRichchar *p = rw->richtext.rc_string;
	int n = MwRcStrlen(p);
	MwRichchar *q = MwMalloc((n+count+1)*sizeof *q);
	MwRcStrncpy(q, p, i);
	MwRcStrncpy(q+i, buf, count);
	MwRcStrcpy(q+i+count, p+i);
	rw->richtext.rc_string = q;
	MwFree(p);
	Redisplay(w, NULL, None);
}

