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

#include "MwImageP.h"

#define DEFAULT_HEIGHT 50
#define DEFAULT_WIDTH 50

#define offset(field) XtOffsetOf(MwImageRec, image.field)
static XtResource resources[] = {
	{
		XtNbitmap,	/* name */
		XtCBitmap,	/* class */
		XtRPixmap,		/* type */
		sizeof(Pixmap),	/* size */
		offset(bitmap),		/* offset */
		XtRImmediate,		/* default_type */
		(XtPointer) NULL	/* default_addr */
	}
};
#undef offset

/* methods */
static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Realize(Widget, XtValueMask *, XSetWindowAttributes *);
static void Redisplay(Widget, XEvent *, Region);
static void Destroy(Widget);
static void Resize(Widget);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);

/* actions */
static void MwImageAction(Widget, XEvent *, String *, Cardinal *);

static XtActionsRec actions[] =
{
	{"image", MwImageAction},
};

/* translations */
static char translations[] =
"<Key>:		image()\n";

MwImageClassRec mwImageClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) &widgetClassRec,
    /* class_name		*/	"MwImage",
    /* widget_size		*/	sizeof(MwImageRec),
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
  },
  { /* image fields */
    /* empty			*/	0
  }
};

WidgetClass mwImageWidgetClass = (WidgetClass)&mwImageClassRec;

static void MwImageAction(Widget w, XEvent *event, String *params, Cardinal *n)
{
	;
}

/* ---
*/
static void scale_pixmap(Widget w)
{
	Display *dpy = XtDisplay(w);
	MwImageWidget iw = (MwImageWidget)w;
	GC gc = iw->image.gc;
	Pixmap old = iw->image.bitmap;
	unsigned int neww = iw->core.width;
	unsigned int newh = iw->core.height;
        Pixmap new;
        unsigned int x, y, ox, oy;
        unsigned long pixel;
        Window root;
        unsigned int oldw, oldh, border, depth;
        XImage *im_in, *im_out;

        if (old == None) {
		iw->image.bg = None;
		return;
	}

        XGetGeometry(dpy, old, &root, &x, &y, &oldw, &oldh,
                        &border, &depth);

        new = XCreatePixmap(dpy, root, neww, newh, depth);
        if (new == None) {
		iw->image.bg = None;
		return;
	}

	if (neww == oldw && newh == oldh) {
		/* just copy the old pixmap */
		XCopyArea(dpy, old, new, gc, 0, 0, oldw, oldh, 0, 0);
	        iw->image.bg = new;
		return;
	}

        im_in = XGetImage(dpy, old, 0, 0, oldw, oldh, ~0, ZPixmap);
        im_out = XGetImage(dpy, new, 0, 0, neww, newh, ~0, ZPixmap);

        for (y = 0; y < newh; y++) {
                oy = y*oldh/newh;
                for (x = 0; x < neww; x++) {
                        ox = x*oldw/neww;
                        pixel = XGetPixel(im_in, ox, oy);
                        XPutPixel(im_out, x, y, pixel);
                }
        }

        XPutImage(dpy, new, gc, im_out, 0, 0, 0, 0, neww, newh);
	XDestroyImage(im_in);
	XDestroyImage(im_out);

        iw->image.bg = new;
}

static void Initialize(Widget req, Widget new, ArgList args, Cardinal *nargs)
{
	MwImageWidget iw = (MwImageWidget)new;

	if (iw->core.height == 0) iw->core.height = DEFAULT_HEIGHT;
	if (iw->core.width == 0) iw->core.width = DEFAULT_WIDTH;
	(*XtClass(new)->core_class.resize)((Widget)iw);
}

#define superclass (&coreClassRec)
static void Realize(Widget w, XtValueMask *valueMask,
		XSetWindowAttributes *attributes)
{
	unsigned long valuemask = 0;
	XGCValues values;
	MwImageWidget iw = (MwImageWidget)w;

	(*superclass->core_class.realize)(w, valueMask, attributes);
	iw->image.gc = XCreateGC(XtDisplay(w), XtWindow(w), valuemask, &values);
	iw->image.bg = None;
	iw->image.timeout = None;
}

static void Destroy(Widget w)
{
	MwImageWidget iw = (MwImageWidget)w;
	XFreeGC(XtDisplay(w), iw->image.gc);
	if (iw->image.bg) XFreePixmap(XtDisplay(w), iw->image.bg);
}

static void do_redisplay(XtPointer client_data, XtIntervalId *id)
{
	Widget w = (Widget)client_data;
	MwImageWidget iw = (MwImageWidget)w;

	if (!XtIsRealized(w) || iw->image.bitmap == None) return;

	if (iw->image.bg == None) scale_pixmap(w);
	if (iw->image.bg == None) return;

	XCopyArea(XtDisplay(w), iw->image.bg, XtWindow(w), iw->image.gc,
		0, 0, iw->core.width, iw->core.height, 0, 0);

	iw->image.timeout = None;
}

static void Redisplay(Widget w, XEvent *event, Region r)
{
	MwImageWidget iw = (MwImageWidget)w;
	if (iw->image.timeout) return;	/* already set */
	iw->image.timeout = XtAppAddTimeOut(
			XtWidgetToApplicationContext(w),
			50, do_redisplay, (XtPointer)w);
}

static void Resize(Widget w)
{
	MwImageWidget aw = (MwImageWidget) w;

	if (!XtIsRealized(w)) return;

	if (aw->image.bg) XFreePixmap(XtDisplay(w), aw->image.bg);
	aw->image.bg = None;
}

static Boolean SetValues(Widget current, Widget request, Widget new,
		ArgList args, Cardinal *nargs)
{
	Boolean do_redisplay = True;

	Resize(new);

	return do_redisplay;
}

