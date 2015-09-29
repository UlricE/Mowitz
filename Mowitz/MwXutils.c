/*
Copyright (C) 1996-2003  Ulric Eriksson <ulric@siag.nu>

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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/xpm.h>
#include <Mowitz/Mowitz.h>

static int find_file(const char *path, char *fn, const char *name)
{
	int found = 0;
	struct stat statbuf;
	char *p = NULL, *q;

	if (name[0] == '/') {
		strcpy(fn, name);
		found = !stat(fn, &statbuf);
		return found;
	} else {
		if (path) p = MwStrdup(path);
		else p = MwStrdup(DEFAULT_PIXPATH);
		for (q = strtok(p, ":"); q; q = strtok(NULL, ":")) {
			sprintf(fn, "%s/%s", q, name);
			found = !stat(fn, &statbuf);
			if (found) break;
		}
	}
	MwFree(p);
	return found;
}

Pixmap MwLoadPixmap(Display *dpy, Pixel color, const char *pm)
{
	XpmAttributes xa;
	Pixmap pm_return;
	int result;
	XpmColorSymbol symbol;
	char fn[1024];
	static char *pixpath, *mowitz_data;
	char pp2[1024];
	int found = 0;
	Screen *scr = DefaultScreenOfDisplay(dpy);
	Window root = XRootWindowOfScreen(scr);

	MwInitFormat(dpy);
	if (!mowitz_data) {
		mowitz_data = getenv("MOWITZ_DATA");
		if (!mowitz_data) mowitz_data = MOWITZ_DATA;
	}
	if (!pixpath) {
		pixpath = getenv("PIXPATH");
		if (!pixpath) {
			sprintf(fn, "%s/pixmaps", mowitz_data);
			pixpath = MwStrdup(fn);
		}
	}
	found = find_file(pixpath, fn, pm);
	if (!found) {
		sprintf(pp2, "%s/pixmaps", mowitz_data);
		found = find_file(pp2, fn, pm);
	}
	if (!found) {
		fprintf(stderr, "Pixmap %s not found in %s\n", pm, fn);
		return None;
	}

	xa.closeness = 40000;
	xa.exactColors = False;
	xa.valuemask = XpmCloseness | XpmExactColors;

	symbol.name = NULL;
	symbol.value = "none";
	symbol.pixel = color;
	xa.colorsymbols = &symbol;
	xa.numsymbols = 1;
	xa.valuemask |= XpmColorSymbols;

	result = XpmReadFileToPixmap(dpy, root, fn, &pm_return, NULL, &xa);

	if (result != XpmSuccess) {
		fprintf(stderr, "XpmReadFileToPixmap(%s) returns %s\n",
			fn, XpmGetErrorString(result));
		return None;
	}
	return pm_return;
}

/*
Most of this stuff is just too boneheaded to be true.
*/
/* color allocation stuff snarfed from the xmagick library */
#ifndef USHRT_MAX
#define USHRT_MAX 65535
#endif

typedef struct xmagick_color {
	XColor color;
	unsigned long rgb;
} xmagick_color;

static int nalloc = 0, nquery = 0;
static XColor *alloc_cache = NULL, *query_cache = NULL;
static Colormap cmap = None;
#define MAXCOLORS 32768


/* compare two XColors */
static int compare_colors(const void *p, const void *q)
{
	int i;
	XColor *c = (XColor *)p;
	XColor *d = (XColor *)q;
	i = (int)c->red - d->red;
	if (i) return i;
	i = (int)c->green - d->green;
	if (i) return i;
	return (int)c->blue - d->blue;
}

static void init_cmap(Display *dpy)
{
	XColor c;

	if (cmap == None) {
		cmap = DefaultColormap(dpy, DefaultScreen(dpy));
		c.flags = DoRed|DoGreen|DoBlue;
		c.red = c.green = c.blue = 0;
		MwAllocColor(dpy, None, &c);
		c.red = 0xffff;
		MwAllocColor(dpy, None, &c);
		c.red = 0;
		c.green = 0xffff;
		MwAllocColor(dpy, None, &c);
		c.green = 0;
		c.blue = 0xffff;
		MwAllocColor(dpy, None, &c);
		c.red = c.green = 0xffff;
		c.blue = 0;
		MwAllocColor(dpy, None, &c);
		c.green = 0;
		c.blue = 0xffff;
		MwAllocColor(dpy, None, &c);
		c.red = 0;
		c.green = 0xffff;
		MwAllocColor(dpy, None, &c);
		c.red = c.blue = 0xffff;
		MwAllocColor(dpy, None, &c);
	}
}

void MwAllocColor(Display *dpy, Colormap cm, XColor *color)
{    
	int i, j, lower, upper;
	int d = 0;
	XColor c;
	static int lastcolor = -1;
	unsigned long x;

	MwInitFormat(dpy);
	init_cmap(dpy);
	if (cm != None) cmap = cm;	/* ugly hack */

	c.red = color->red & 0xf800;
	c.green = color->green & 0xf800;
	c.blue = color->blue & 0xf800;
	if (lastcolor != -1 && compare_colors(&c, &alloc_cache[lastcolor]) == 0) {
		color->pixel = alloc_cache[lastcolor].pixel;
		return;
	}

	lower = 0;
	upper = nalloc-1;
	while (lower <= upper) {
		i = (lower+upper)/2;
		d = compare_colors(&c, &alloc_cache[i]);
		if (d == 0) {
			lastcolor = i;
			color->pixel = alloc_cache[i].pixel;
			return;
		} else if (d > 0) {
			lower = i+1;
		} else {
			upper = i-1;
		}
	}
	if (nalloc >= MAXCOLORS) {
		color->pixel = 0;
		return;
	}

	nalloc++;
	alloc_cache = MwRealloc(alloc_cache, nalloc * sizeof *alloc_cache);

	i = lower;

	if (i < 0) i = 0;
	if (i >= nalloc) i = nalloc-1;
	for (j = nalloc-1; j > i; j--) {
		alloc_cache[j] = alloc_cache[j-1];
	}
	alloc_cache[i] = c;
	x = c.red * 0xffff;
	c.red = x / 0xf800;
	x = c.green * 0xffff;
	c.green = x / 0xf800;
	x = c.blue * 0xffff;
	c.blue = x / 0xf800;
	c.flags = DoRed|DoGreen|DoBlue;
	if (XAllocColor(dpy, cmap, &c) == 0) {
		if (i < 4) {
			c.pixel = alloc_cache[0].pixel;	/* black */
		} else {
			c.pixel = alloc_cache[7].pixel;		/* white */
		}
	}
	color->pixel = alloc_cache[i].pixel = c.pixel;
	lastcolor = i;
}

void MwAllocNamedColor(Display *dpy, char *name, XColor *color)
{
	MwInitFormat(dpy);
	init_cmap(dpy);
	XParseColor(dpy, cmap, name, color);
	MwAllocColor(dpy, None, color);
}

/* FIXME: query_cache should be sorted to permit treesort */
void MwQueryColor(Display *dpy, Colormap cm, XColor *color)
{
	int i;

	MwInitFormat(dpy);
	init_cmap(dpy);
	if (cm != None) cmap = cm;

	color->flags = DoRed|DoGreen|DoBlue;

	for (i = 0; i < nquery; i++) {
		if (query_cache[i].pixel == color->pixel) {
			color->red = query_cache[i].red;
			color->green = query_cache[i].green;
			color->blue = query_cache[i].blue;
			return;
		}
	}
	if (i >= MAXCOLORS) {
		color->red = 0;
		color->green = 0;
		color->blue = 0;
		return;
	}
	XQueryColor(dpy, cmap, color);
	nquery++;
	query_cache = MwRealloc(query_cache, nquery * sizeof *query_cache);
	query_cache[i] = *color;
	nquery++;
}


/* ---
Place the child centered relative to the parent.

011225: Center relative to screen instead.
*/

void MwCenter(Widget child)
{
	Position x, y;
	Dimension w1, h1;
	Window root;
	int rx, ry, rw, rh, rb, rd;

	XtRealizeWidget(child);
	XtUnrealizeWidget(child);
	XtVaGetValues(child,
		XtNwidth, &w1,
		XtNheight, &h1, (char *)0);
	XGetGeometry(XtDisplay(child), DefaultRootWindow(XtDisplay(child)),
		&root, &rx, &ry, &rw, &rh, &rb, &rd);
	x = (rw-w1)/2;
	y = (rh-h1)/2;
	XtVaSetValues(child,
		XtNx, x,
		XtNy, y, (char *)0);
}

String MwLabelGet(Widget w)
{
	String p = NULL;
	XtVaGetValues(w, XtNlabel, &p, (char *)0);
	return p;
}      

void MwLabelSet(Widget w, String p)
{
	char *q, *r;
	int must_set = 0;

	if (w == None || p == NULL) return;
	q = _((char *)p);
	if (q == NULL) return;
	r = MwLabelGet(w);

	if (r == NULL) {
		must_set = 1;
	} else {
		must_set = (strcmp(q, r) != 0);
	}

	if (must_set) XtVaSetValues(w, XtNlabel, q, (char *)0);
}

/* ---
Get the state of a toggle widget
*/

Boolean MwStateGet(Widget w)
{
	Boolean i = False;
	if (w != None)
		XtVaGetValues(w, XtNstate, &i, (char *)0);
	return i;
}

/* this is needed if we include vanilla Athena headers */
#ifndef XtNshadowWidth
#define XtNshadowWidth "shadowWidth"
#endif

/* ---
If no widget, do nothing.
If same state and same shadow width as before, do nothing.
If set, use shadow width ssw.
If unset, use shadow width usw.
*/

void MwStateSet(Widget w, Boolean i, int ssw, int usw)
{
	Boolean set;
	Dimension osw, nsw;
	if (w == None) return;
	XtVaGetValues(w,
		XtNstate, &set,
		XtNshadowWidth, &osw,
		(char *)0);
	if (i) nsw = ssw;
	else nsw = usw;
	if (i == set && nsw == osw) return;
	XtVaSetValues(w,
		XtNstate, i,
		XtNshadowWidth, nsw,
		(char *)0);
}

Dimension MwWidthGet(Widget w)
{
	Dimension x = 80;
	if (w != None)
		XtVaGetValues(w, XtNwidth, &x, (char *)0);
	return x;
}

/* ---
Get the height of a widget.
*/

Dimension MwHeightGet(Widget w)
{
	Dimension x = 20;
	if (w != None)
		XtVaGetValues(w, XtNheight, &x, (char *)0);
	return x;
}


int MwXErrorHandler(Display *dpy, XErrorEvent *event)
{
	char b[1000];

	XGetErrorText(dpy, event->error_code, b, 999);
	puts(b);
	return 0;
}

void MwSetIcon(Widget toplevel, char **icon_data)
{
	Pixmap icon, mask;
	XWMHints *hints;
	Display *dpy = XtDisplay(toplevel);
	int Width, Height;
	int scrn = DefaultScreen(dpy);
	int dpth = XDefaultDepth(dpy, scrn);
	Window win = DefaultRootWindow(dpy);
	/* Create the pixmaps accordingly		      *
	 * All the pixmaps MUST have the same height and width */

	XpmAttributes xpmattributes;
	xpmattributes.depth = dpth;
	xpmattributes.valuemask = XpmSize | XpmDepth;
	XpmCreatePixmapFromData(dpy, win, icon_data, &icon, &mask,
				&xpmattributes);
	Width = xpmattributes.width;
	Height = xpmattributes.height;
	
	/* Now tell the window manager what the icon is */
	hints = XAllocWMHints();
	hints->flags = IconPixmapHint | IconMaskHint;
	hints->icon_pixmap = icon;
	hints->icon_mask = mask;
	XSetWMHints(dpy, XtWindow(toplevel), hints);
	XFree(hints);
}

#define HIGHLIGHT_WIDTH 2
#define UNHIGHLIGHT_WIDTH 1

/* ---
This function highlights by increasing the shadow width. This
doesn't work for plain Athena widgets which have no shadow at
all, so they won't be highlighted.
*/

static void highlight_action(Widget w, XEvent *event,
	String *params, Cardinal *num_params)
{
	int sw;
	Boolean set;

	if (*num_params) sw = atoi(*params);
	else sw = HIGHLIGHT_WIDTH;
	if (*num_params > 1) {
		XtVaGetValues(w,
			XtNstate, &set,
			(char *)0);
	} else {
		set = 0;
	}
	if (set) {
		sw++;
	}

	if (w) {
		XtVaSetValues(w, "shadowWidth", sw, (char *)0);
	}
}

/* ---
Remove the highlighting.
*/

static void unhighlight_action(Widget w, XEvent *event,
	String *params, Cardinal *num_params)
{
	int sw;
	Boolean set;

	if (*num_params) sw = atoi(*params);
	else sw = UNHIGHLIGHT_WIDTH;
	if (*num_params > 1) {
		XtVaGetValues(w,
			XtNstate, &set,
			(char *)0);
	} else {
		set = 0;
	}
	if (set) {
		sw++;
	}

	if (w) {
		XtVaSetValues(w, "shadowWidth", sw, (char *)0);
	}
}

static XtActionsRec actions[] =
{
	{"siag-highlight", highlight_action},
	{"siag-unhighlight", unhighlight_action},
};

/* ---
Set the highlighting and unhighlighting colours.
*/

void MwHighlightInit(Widget pw)
{
	XtAppAddActions(XtWidgetToApplicationContext(pw),
		actions, XtNumber(actions));
}

