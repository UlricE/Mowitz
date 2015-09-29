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
#include <limits.h>
#include <sys/stat.h>

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <Mowitz/Mowitz.h>

#include <X11/xpm.h>

#include "MwAnimatorP.h"

typedef struct s_ani_image {
	char *filename;
	Pixmap pixmap, mask;
	struct s_ani_image *next;
} ani_image;

static void plugin_coords(Widget, XtPointer, int *, int *);

#define offset(field) XtOffsetOf(MwAnimatorRec, animator.field)
static XtResource resources[] = {
	{
		XtNanimatorCast,	/* name */
		XtCAnimatorCast,	/* class */
		XtRPointer,		/* type */
		sizeof(XtPointer),	/* size */
		offset(cast),		/* offset */
		XtRImmediate,		/* default_type */
		(XtPointer)0		/* default_addr */
	}, {
		XtNanimatorNow,
		XtCAnimatorNow,
		XtRInt,
		sizeof(int),
		offset(now),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNanimatorDelta,
		XtCAnimatorDelta,
		XtRInt,
		sizeof(int),
		offset(delta),
		XtRImmediate,
		(XtPointer)100
	}, {
		XtNanimatorDuration,
		XtCAnimatorDuration,
		XtRInt,
		sizeof(int),
		offset(duration),
		XtRImmediate,
		(XtPointer)1000
	}, {
		XtNanimatorMode,
		XtCAnimatorMode,
		XtRInt,
		sizeof(int),
		offset(newmode),
		XtRImmediate,
		(XtPointer)MW_ANI_STOP
	}, {
		XtNanimatorBgPixmap,
		XtCAnimatorBgPixmap,
		XtRString,
		sizeof(String),
		offset(bg_pixmap),
		XtRString,
		(XtPointer)0
	}, {
		XtNgradient,
		XtCGradient,
		XtRString,
		sizeof(String),
		offset(bgrad),
		XtRString,
		(XtPointer)0
	}, {
		XtNanimatorPluginCoords,
		XtCAnimatorPluginCoords,
		XtRPointer,
		sizeof(XtPointer),
		offset(plugin_coords),
		XtRImmediate,
		(XtPointer)plugin_coords
	}
};
#undef offset

/* methods */
static void DoLayout(MwAnimatorWidget);
static void Resize(Widget);
static XtGeometryResult GeometryManager(Widget,
		XtWidgetGeometry *, XtWidgetGeometry *);
static void ChangeManaged(Widget);
static void Redisplay(Widget, XEvent *, Region);
static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Realize(Widget, XtValueMask *, XSetWindowAttributes *);
static void Destroy(Widget);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);

static void plugin_coords(Widget w, XtPointer p, int *x, int *y)
{
        *x = *y = 0;
}

/* actions */
static void MwAnimatorAction(Widget, XEvent *, String *, Cardinal *);

static XtActionsRec actions[] =
{
	{"animator", MwAnimatorAction},
};

/* translations */
static char translations[] =
"<Key>Return:		animator(next)		\n\
<Btn1Down>:		animator(next)		\n\
:<Key>Tab:		animator(next)		\n\
<Key>N:			animator(next)		\n\
<Key>P:			animator(previous)	\n\
<Key>S:			animator(stop)		\n\
<Key>C:			animator(continue)	\n\
<Key>Escape:		animator(quit)		\n\
<Key>Q:			animator(quit)		\n\
";

MwAnimatorClassRec mwAnimatorClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) &compositeClassRec,
    /* class_name		*/	"MwAnimator",
    /* widget_size		*/	sizeof(MwAnimatorRec),
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
  }, {
    /* geometry_manager   */    GeometryManager,
    /* change_managed     */    ChangeManaged,
    /* insert_child       */    XtInheritInsertChild,
    /* delete_child       */    XtInheritDeleteChild,
    /* extension          */    NULL
  }, { /* animator fields */
    /* empty			*/	0
  }
};

WidgetClass mwAnimatorWidgetClass = (WidgetClass)&mwAnimatorClassRec;

/* supporting code copied directly from animator.c */

static void ani_stepper(XtPointer client_data, XtIntervalId *id)
{
	int ani_then, ani_now;
	MwAnimatorWidget aw = (MwAnimatorWidget) client_data;

	if (!XtIsRealized((Widget)aw)) return;

	/* cancel any pending timeout */
	/* this is tricky. if ani_stepper is called by us and not by X,
	   we must remove the timeout ourselves */
	if (!id && aw->animator.waiting) {
		XtRemoveTimeOut(aw->animator.timeout);
	}
	aw->animator.waiting = False;

	ani_then = ani_now = aw->animator.now;
	if (aw->animator.mode == MW_ANI_NEXT || aw->animator.mode == MW_ANI_CONTINUE)
		ani_now += aw->animator.delta;
	else if (aw->animator.mode == MW_ANI_PREVIOUS)
		ani_now -= aw->animator.delta;
	if (ani_now < 0)
		ani_now = 0;
	if (ani_now > aw->animator.duration)
		ani_now = 0;
	if (ani_now != ani_then) {
		aw->animator.now = ani_now;
		Redisplay((Widget)aw, NULL, None);
	}
	if (aw->animator.mode == MW_ANI_CONTINUE) {
		aw->animator.timeout = XtAppAddTimeOut(
				XtWidgetToApplicationContext((Widget)aw),
				aw->animator.delta, ani_stepper,
				(XtPointer)aw);
		aw->animator.waiting = True;
	}
}

static void ani_ctl(Widget w, int mode)
{
	MwAnimatorWidget aw = (MwAnimatorWidget)w;

	switch (mode) {
	case MW_ANI_CONTINUE:
		if (aw->animator.mode == mode) break;
	case MW_ANI_STOP:
	case MW_ANI_NEXT:
	case MW_ANI_PREVIOUS:
		aw->animator.mode = mode;
		ani_stepper((XtPointer)w, NULL);
		break;
	case MW_ANI_QUIT:
		/* can't really do that from here, can we? */
	default:
		break;
	}
}

static void MwAnimatorAction(Widget w, XEvent *event, String *params, Cardinal *n)
{
	if (*n < 1 || !MwStrcasecmp(params[0], "next"))
		ani_ctl(w, MW_ANI_NEXT);
	else if (!MwStrcasecmp(params[0], "previous"))
		ani_ctl(w, MW_ANI_PREVIOUS);
	else if (!MwStrcasecmp(params[0], "stop"))
		ani_ctl(w, MW_ANI_STOP);
	else if (!MwStrcasecmp(params[0], "continue"))
		ani_ctl(w, MW_ANI_CONTINUE);
	else if (!MwStrcasecmp(params[0], "quit"))
		ani_ctl(w, MW_ANI_QUIT);
}

static ani_image *i_list = NULL;

/* ---
This is self-contained: it uses only the static variable i_list to
   point to the head of the image list.
*/

static ani_image *name2image(Widget w, char *name)
{
	XpmAttributes xa;
	ani_image *i;
	int err;
	char b[1024];
	char tmpfile[1024];
	Display *display = XtDisplay(w);
	Window ani_win = XRootWindowOfScreen(XtScreen(w));
	static char *mowitz_data;

	if (!mowitz_data) mowitz_data = getenv("MOWITZ_DATA");
	if (!mowitz_data) mowitz_data = MOWITZ_DATA;

	if (!XtIsRealized(w)) return NULL;

#if 0	/* FIXME */
	sprintf(tmpfile, "%s/siagimage.xpm", siag_tmpdir);
#else
	sprintf(tmpfile, "%s/siagimage.xpm", "/tmp");
#endif
	if (!name) return NULL;

	for (i = i_list; i; i = i->next) {
		if (!strcmp(name, i->filename)) return i;
	}
	/* didn't find it; try loading */
	sprintf(b, "%s/any2xpm %s > %s", mowitz_data, name, tmpfile);
	system(b);

	i = (ani_image *)MwMalloc(sizeof(ani_image));
	xa.closeness = 40000;
	xa.exactColors = FALSE;
	xa.valuemask = XpmCloseness | XpmExactColors;
	err = XpmReadFileToPixmap(display, ani_win,
		tmpfile, &(i->pixmap), &(i->mask), &xa);
	if (err != XpmSuccess) {
		MwFree(i);
		fprintf(stderr, "XpmReadFileToPixmap returns %s\n",
			XpmGetErrorString(err));
		return NULL;
	}
	remove(tmpfile);
	i->filename = MwStrdup(name);
	i->next = i_list;
	i_list = i;
	return i;
}

/* ---
this looks unnecessarily complex and it is, but I had problems
   with signedness and MAXINT wraparound
*/

static int INTERPOLATE(int x1, int x2,
		unsigned int t, unsigned int t1, unsigned int t2)
{
	long x0 = x1;		/* always pos */
	long xd = x2-x1;	/* pos or neg */
	long td1 = t-t1;	/* always pos */
	long td2 = t2-t1;	/* always pos */
	long tmp = xd*td1;	/* pos or neg */
	long rl = x0+tmp/td2;	/* pos or neg */
	int ri = rl;		/* pos or neg */

	return ri;
}


static void draw_gradient(Widget w, Pixmap scribble)
{
	MwAnimatorWidget aw = (MwAnimatorWidget)w;
	char *bgrad = aw->animator.bgrad;
	Display *dpy = XtDisplay(w);
	int screen = DefaultScreen(dpy);
	Visual *visual = DefaultVisual(dpy, screen);
	int bitmap_pad = XBitmapPad(dpy);
	GC gc = aw->animator.gc;
	int bytes_per_line = 0;
	XColor color;
	unsigned long pixel;
	int format = ZPixmap;
	int offset = 0;
	char *data = NULL;
	int width, height, depth;
	int x, y;
	int bw, bh, bc, ba, bz, bn;
	char bc1[100], bc2[100];
	XColor top, bot;

	unsigned int neww = aw->core.width;
	unsigned int newh = aw->core.height;
	unsigned int ox, oy;
	unsigned int oldw = width, oldh = height;

	/* we're going from black at the top to blue
	   at the bottom */
	if (bgrad == NULL) bgrad = "";
	if (aw->animator.ximage == NULL) {
		XImage *img;
		bn = sscanf(bgrad, "%d %d %d %d %d %s %s",
			&bw, &bh, &bc, &ba, &bz, bc1, bc2);
		if (bn < 0) bn = 0;
		switch (bn) {
		case 0:	bw = 100;
		case 1:	bh = 100;
		case 2: bc = 128;
		case 3: ba = 0;
		case 4:	bz = 1;
		case 5: strcpy(bc1, "black");
		case 6:	strcpy(bc2, "blue");
		default: ;	/* catches e.g. -1 */
		}

		width = aw->core.width*bw/100;
		height = aw->core.height*bh/100;
		depth = aw->core.depth;
		img = aw->animator.ximage = XCreateImage(dpy,
			visual, depth, format, offset, data,
			width, height, bitmap_pad, bytes_per_line);
		img->data = MwMalloc(img->bytes_per_line*img->height);
		MwAllocNamedColor(dpy, bc1, &top);
		MwAllocNamedColor(dpy, bc2, &bot);
		for (y = 0; y < height; y++) {
			color.red = (int)top.red
				+((int)bot.red-top.red)*y/height;
			color.green = (int)top.green
				+((int)bot.green-top.green)*y/height;
			color.blue = (int)top.blue
				+((int)bot.blue-top.blue)*y/height;
			MwAllocColor(dpy, None, &color);
			pixel = color.pixel;
			for (x = 0; x < width; x++ ) {
				XPutPixel(img, x, y, pixel);
			}
		}
		aw->animator.ximage = XCreateImage(dpy,
			visual, depth, format, offset, data,
			aw->core.width, aw->core.height,
			bitmap_pad, bytes_per_line);
		aw->animator.ximage->data = MwMalloc(
					aw->animator.ximage->bytes_per_line
					*aw->animator.ximage->height);

		neww = aw->core.width;
		newh = aw->core.height;
		oldw = width, oldh = height;

		if (bz) {	/* zoom */
			/* snarfed from Image widget */
			for (y = 0; y < newh; y++) {
				oy = y*oldh/newh;
				for (x = 0; x < neww; x++) {
					ox = x*oldw/neww;
					pixel = XGetPixel(img, ox, oy);
					XPutPixel(aw->animator.ximage,
						x, y, pixel);
				}
			}
		} else {	/* tile */
			for (y = 0; y < newh; y++) {
				oy = y % oldh;
				for (x = 0; x < neww; x++) {
					ox = x % oldw;
					pixel = XGetPixel(img, ox, oy);
					XPutPixel(aw->animator.ximage,
						x, y, pixel);
				}
			}
		}
		XDestroyImage(img);
	}
	XPutImage(dpy, scribble, gc, aw->animator.ximage,
		0, 0, 0, 0, aw->core.width, aw->core.height);
}

/* ---
create, draw and return a pixmap of the stage at time now.
   Caller must free
this is self-contained; no global or static vars are used,
   so it can be called at any time from anywhere with any parameters
*/

Pixmap MwAnimatorPixmap(Widget w, int ani_now)
{
	MwAnimatorWidget aw = (MwAnimatorWidget)w;
	GC ani_gc = aw->animator.gc;
	Display *display = XtDisplay(w);
	Window ani_win = XtWindow(w);
	MwFmt fmt;
	XColor color;

	int x, y, x1, y1, x2, y2;
	unsigned int b1, d1;
	Window root;
	int width, height;
	unsigned int w1, h1;
	int visible;
	int angle1, angle2;
	MwAniObject *actor;
	ani_image *img, *bg_image;
	Pixmap scribble;
	unsigned int ani_width, ani_height, ani_depth;
	MwRichchar *p;
	char *bg_pixmap = aw->animator.bg_pixmap;

	if (!XtIsRealized(w)) return None;

	ani_width = aw->core.width;
	ani_height = aw->core.height;
	ani_depth = aw->core.depth;
	if (ani_width > 2000 || ani_height > 2000) return None;

	scribble = XCreatePixmap(display, ani_win,
		ani_width, ani_height, ani_depth);

	draw_gradient(w, scribble);

	/* tile the background */
	bg_image = name2image(w, bg_pixmap);

	if (bg_image) {
		int bg_x, bg_y;
		unsigned int bg_width, bg_height, bg_border, bg_depth;

		XSetClipMask(display, ani_gc, bg_image->mask);

		XGetGeometry(display, bg_image->pixmap, &root, &bg_x, &bg_y,
				&bg_width, &bg_height, &bg_border, &bg_depth);
		for (y = 0; y < ani_height; y += bg_height) {
			for (x = 0; x < ani_width; x += bg_width) {
				XSetClipOrigin(display, ani_gc, x, y);
				XCopyArea(display, bg_image->pixmap, scribble,
					ani_gc, 0, 0,
					bg_width, bg_height, x, y);
			}
		}
	}

	/* now loop over the objects, drawing each in the right place */
	for (actor = aw->animator.cast; actor; actor = actor->next) {
		/* find the tick before and after ani_now and
		   interpolate to find position and size.
		   the script always contains a tick for time=0 */
		MwAniScript *before, *after;

		before = actor->script;		/* this is never NULL */
		while (before->next) {
			if (before->next->time > ani_now) break;
			before = before->next;
		}
		after = before->next;
		visible = before->visible;
		if (!visible) continue;

		/* set color */
		MwDecodeFormat(actor->fmt, MW_FMT_FG, &fmt);
		/* fmt.fg is now the color name */
		MwAllocNamedColor(display, fmt.fg, &color);
		XSetForeground(display, ani_gc, color.pixel);

		XSetClipMask(display, ani_gc, None);

		if (!after) {	/* object remains at before */
			x = before->x;
			y = before->y;
			width = before->width;
			height = before->height;
		} else {	/* interpolate */
			x = INTERPOLATE(before->x, after->x,
				ani_now, before->time, after->time);
			y = INTERPOLATE(before->y, after->y,
				ani_now, before->time, after->time);
			width = INTERPOLATE(before->width, after->width,
				ani_now, before->time, after->time);
			height = INTERPOLATE(before->height, after->height,
				ani_now, before->time, after->time);
		}

		switch (actor->type) {
		case MW_ANI_NONE:
			break;
		case MW_ANI_LINE:
			x2 = x+width;
			y2 = y+height;
			XDrawLine(display, scribble,
				ani_gc, x, y, x2, y2);
			break;
		case MW_ANI_RECTANGLE:
			XDrawRectangle(display, scribble,
				ani_gc, x, y, width, height);
			break;
		case MW_ANI_ARC:
			angle1 = 64*90;		/* bogus!!! */
			angle2 = 64*180;
			XDrawArc(display, scribble,
				ani_gc, x, y, width, height,
				angle1, angle2);
			break;
		case MW_ANI_ELLIPSE:
			XDrawArc(display, scribble,
				ani_gc, x, y, width, height,
				0, 64*360);
			break;
		case MW_ANI_PIXMAP:
			img = name2image(w, actor->string);
			if (!img) break;

			XGetGeometry(display, img->pixmap, &root, &x1, &y1,
				&w1, &h1, &b1, &d1);
			XSetClipOrigin(display, ani_gc, x, y);
			XSetClipMask(display, ani_gc, img->mask);

			XCopyArea(display, img->pixmap,
				scribble, ani_gc,
				0, 0, w1, h1, x, y);
			break;
		case MW_ANI_STRING:
			/* set font */
			if (!actor->string) break;	/* sanity */
			p = MwRcMakerich(actor->string, actor->fmt);
			MwRcStrdraw(scribble, ani_gc, 0, 0, x, y, p, -1, 1.0);
			MwFree(p);
			break;
		case MW_ANI_POINT:
			XDrawPoint(display, scribble,
				ani_gc, x, y);
			break;
		case MW_ANI_FILLRECT:
			XFillRectangle(display, scribble,
				ani_gc, x, y, width, height);
			break;
		case MW_ANI_FILLARC:
			angle1 = 64*90;		/* bogus!!! */
			angle2 = 64*180;
			XFillArc(display, scribble,
				ani_gc, x, y, width, height,
				angle1, angle2);
			break;
		case MW_ANI_FILLELLIPSE:
			XFillArc(display, scribble,
				ani_gc, x, y, width, height,
				0, 64*360);
			break;
		default:
			fprintf(stderr, "bzz\n");
		}
	}

	return scribble;
}

/* end of snip from animator.c */

static GC get_gc(Widget w)
{
	unsigned long valuemask = 0;
	XGCValues values;

	return XCreateGC(XtDisplay(w), XtWindow(w), valuemask, &values);
}

static void Initialize(Widget req, Widget new, ArgList args, Cardinal *nargs)
{
	if (req->core.width == 0) req->core.width = 100;
	if (req->core.height == 0) req->core.height = 100;
}

/* This seems to be the only way to avoid BadDrawable errors */
#define superclass (&coreClassRec)
static void Realize(Widget w, XtValueMask *valueMask,
		XSetWindowAttributes *attributes)
{
	MwAnimatorWidget aw = (MwAnimatorWidget) w;
	(*superclass->core_class.realize) (w, valueMask, attributes);
	aw->animator.waiting = False;
	aw->animator.mode = aw->animator.newmode;
	aw->animator.gc = get_gc(w);
	aw->animator.ximage = NULL;
}

static void Destroy(Widget w)
{
	MwAnimatorWidget aw = (MwAnimatorWidget) w;
	aw->animator.mode = MW_ANI_QUIT;
	if (aw->animator.waiting)
		XtRemoveTimeOut(aw->animator.timeout);
	if (aw->animator.ximage)
		XDestroyImage(aw->animator.ximage);
}

static void Redisplay(Widget w, XEvent *xevent, Region r)
{
	MwAnimatorWidget aw = (MwAnimatorWidget) w;
	Pixmap scribble;
	GC ani_gc = aw->animator.gc;

	if (!XtIsRealized(w)) return;	/* but that doesn't work */
	if (!XtIsManaged(w)) return;	/* what about this */

	XSetGraphicsExposures(XtDisplay(w), ani_gc, 0);

	scribble = MwAnimatorPixmap(w, aw->animator.now);

	XCopyArea(XtDisplay(w), scribble, XtWindow(w),
		ani_gc, 0, 0,
		aw->core.width, aw->core.height, 0, 0);

	/* and now we don't need this any more */
	XFreePixmap(XtDisplay(w), scribble);
        /* update plugin positions */
        DoLayout(aw);
}

static Boolean SetValues(Widget current, Widget request, Widget new,
			ArgList args, Cardinal *argc)
{
	MwAnimatorWidget curaw = (MwAnimatorWidget) current;
	MwAnimatorWidget newaw = (MwAnimatorWidget) new;
	Boolean do_redisplay = False;

	if (curaw->animator.newmode != newaw->animator.newmode) {
		ani_ctl((Widget)newaw, newaw->animator.newmode);
		newaw->animator.newmode = MW_ANI_NONE;
	}
	if ((curaw->animator.now != newaw->animator.now)
		/*|| (curaw->animator.mode != newaw->animator.mode)*/) {
		do_redisplay = True;
	}

	if (curaw->animator.cast != newaw->animator.cast) {
		do_redisplay = True;
	}
	if (curaw->animator.bgrad  == NULL
	    || newaw->animator.bgrad == NULL
	    || strcmp(curaw->animator.bgrad, newaw->animator.bgrad)) {
		if (newaw->animator.ximage) {
			XDestroyImage(newaw->animator.ximage);
		}
		newaw->animator.ximage = NULL;
	}
#ifndef WANT_FLICKER
	if (do_redisplay) {
		Redisplay(new, NULL, None);
		do_redisplay = False;
#endif
	}

	return do_redisplay;
}

/*
 * Do a layout, actually assigning positions.
 */

static void DoLayout(MwAnimatorWidget sw)
{
        int i;
        int x, y;

        for (i = 0; i < sw->composite.num_children; i++) {
		x = y = 0;
                XtMoveWidget(sw->composite.children[i], x, y);
        }
}

/*
 * Actually layout the table
 */

static void Resize(Widget w)
{
	MwAnimatorWidget aw = (MwAnimatorWidget)w;
	if (!XtIsRealized(w)) return;	/* but that doesn't work */
	if (aw->animator.ximage) {
		XDestroyImage(aw->animator.ximage);
		aw->animator.ximage = NULL;
	}
        DoLayout(aw);
} /* Resize */

/*
 * Geometry Manager
 *
 * 'reply' is unused; we say only yeay or nay, never almost.
 */

static XtGeometryResult GeometryManager(Widget w,
                XtWidgetGeometry *request, XtWidgetGeometry *reply)
{
        return XtGeometryYes;
}

static void ChangeManaged(Widget w)
{
    DoLayout((MwAnimatorWidget)w);
}

