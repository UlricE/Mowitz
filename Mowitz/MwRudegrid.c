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

/*
 * This widget was written by me, Ulric, by modifying the Gridbox widget
 * by Edward A Falk, hence this separate copyright notice for my code.
 * 
 * Unlike the original, this widget does not care what the children
 * would like for a size, but simply assigns positions and sizes
 * from two resources describing widths and heights. The idea is to
 * be able to handle a) widgets that insist on being a size that is
 * inappropriate for the application and b) widgets that are not
 * clueful enough to figure out what size they should be.
 * 
 * Example of a layout resource:
 * 
 * "25 25 50% 50% 15"
 * 
 * Grid has 5 columns. The first and second are 25 pixels each. The
 * fifth is 15 pixels. The third and fourth share whatever is left.
 */

/*
 * MwRudegrid.c - MwRudegrid composite widget
 *
 * The MwRudegrid widget aligns its children in a rectangular array
 * of cells.  Child widgets occupy a rectangular region of cells (default
 * size is 1x1).  A typical layout might look like this:
 *
 *	+-------+-------+--+----------------------------+
 *	|	|	|  |				|
 *	|-------+-------|  |				|
 *	|	|	|  |				|
 *	|-------+-------|  |				|
 *	|	|	|  |				|
 *	|-------+-------|  |				|
 *	|	|	|  |				|
 *	|-------+-------+--+----------------------------|
 *	|__________________|____________________________|
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Converters.h>
#include <X11/Xmu/CharSet.h>

#include "MwUtils.h"
#include "MwRudegridP.h"

#define DEFAULT_HEIGHT 100
#define DEFAULT_WIDTH 100
#define REALLYSMALL 2

#define Offset(field) XtOffsetOf(MwRudegridRec, rudegrid.field)
static XtResource resources[] = {
    {
	XtNxLayout,
	XtCXLayout,
	XtRString,
	sizeof(String),
	Offset(x_layout),
	XtRImmediate,
	(String)NULL
    }, {
	XtNyLayout,
	XtCYLayout,
	XtRString,
	sizeof(String),
	Offset(y_layout),
	XtRImmediate,
	(String)NULL
    },
};
#undef Offset

#define Offset(field) XtOffsetOf(MwRudegridConstraintsRec, rudegrid.field)
static XtResource rudegridConstraintResources[] = {
    {
	XtNgridx,
	XtCPosition,
	XtRPosition,
	sizeof(Position),
	Offset(gridx),
	XtRImmediate,
	(XtPointer)0
    }, {
	XtNgridy,
	XtCPosition,
	XtRPosition,
	sizeof(Position),
	Offset(gridy),
	XtRImmediate,
	(XtPointer)0
    }, {
	XtNgridWidth,
	XtCWidth,
	XtRDimension,
	sizeof(Dimension),
	Offset(gridWidth),
	XtRImmediate,
	(XtPointer)1
    }, {
	XtNgridHeight,
	XtCHeight,
	XtRDimension,
	sizeof(Dimension),
	Offset(gridHeight),
	XtRImmediate,
	(XtPointer)1
    },
};
#undef Offset

static	void	MwRudegridResize(Widget w) ;
static	Boolean	MwRudegridSetValues(Widget, Widget, Widget, ArgList, Cardinal *) ;

static	void	MwRudegridChangeManaged(Widget w) ;
static	XtGeometryResult
	MwRudegridGeometryManager(Widget, XtWidgetGeometry *, XtWidgetGeometry *) ;

static	Boolean
	MwRudegridConstraintSetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static void Initialize(Widget, Widget, ArgList, Cardinal *);

#ifndef	min
#define	min(a,b)	((a)<(b)?(a):(b))
#define	max(a,b)	((a)>(b)?(a):(b))
#endif

MwRudegridClassRec	mwRudegridClassRec = {
  { /* core_class fields */
    /* superclass         */    (WidgetClass) &constraintClassRec,
    /* class_name         */    "MwRudegrid",
    /* widget_size        */    sizeof(MwRudegridRec),
    /* class_initialize   */    NULL,
    /* class_part_init    */    NULL,
    /* class_inited       */    FALSE,
    /* initialize         */    Initialize/*NULL*/,
    /* initialize_hook    */    NULL,
    /* realize            */    XtInheritRealize,
    /* actions            */    NULL,
    /* num_actions        */    0,
    /* resources          */    resources,
    /* num_resources      */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion    */    TRUE,
    /* compress_exposure  */    TRUE,
    /* compress_enterleave*/    TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    NULL,
    /* resize             */    MwRudegridResize,
    /* expose             */    NULL,
    /* set_values         */    MwRudegridSetValues,
    /* set_values_hook    */    NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */    NULL,
    /* accept_focus       */    NULL,
    /* version            */    XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    NULL,
    /* query_geometry     */	XtInheritQueryGeometry/*MwRudegridQueryGeometry*/,
    /* display_accelerator*/	XtInheritDisplayAccelerator,
    /* extension          */	NULL
  },
  { /* composite_class fields */
    /* geometry_manager   */   MwRudegridGeometryManager,
    /* change_managed     */   MwRudegridChangeManaged,
    /* insert_child       */   XtInheritInsertChild,
    /* delete_child       */   XtInheritDeleteChild,
    /* extension          */   NULL
  },
  { /* constraint_class fields */
    /* subresources       */   rudegridConstraintResources,
    /* subresource_count  */   XtNumber(rudegridConstraintResources),
    /* constraint_size    */   sizeof(MwRudegridConstraintsRec),
    /* initialize         */   NULL,
    /* destroy            */   NULL,
    /* set_values         */   MwRudegridConstraintSetValues,
    /* extension          */   NULL
  },
  { /* rudegrid_class fields */
    /* extension	*/   NULL
  }
};

WidgetClass mwRudegridWidgetClass = (WidgetClass)&mwRudegridClassRec;

/****************************************************************
 *
 * Class Procedures
 *
 ****************************************************************/

/*
Allocate and return an array of column or row dimensions.
Caller must free.
*/

static int *parse_layout(int totw, char *layout, int *ncols)
{
	int w, uw, col, *widths, *cols;
	char *p, *q;
	int nw, nc, i;

	uw = 0;		/* width used so far */
	widths = NULL;	/* widths we have figured out */
	nw = 0;		/* number of column widths */
	cols = NULL;	/* column boundaries */
	nc = 0;		/* number of boundaries */
	p = layout;
	if (p == NULL) p = "";
	w = strtol(p, &q, 10);
	while (p != q) {
		if (*q == '%') {
			w = -w;
			++q;
		} else {
			uw += w;
		}
		widths = MwRealloc(widths, (nw+1)*sizeof *widths);
		widths[nw++] = w;
		p = q;
		w = strtol(p, &q, 10);
	}
	cols = MwMalloc((nw+2)*sizeof *cols);
	col = 0;
	cols[nc++] = col;
	for (i = 0; i < nw; i++) {
		if (widths[i] < 0) widths[i] = -widths[i]*(totw-uw)/100;
		col += widths[i];
		cols[nc++] = col;
	}
	cols[nc++] = totw;
	MwFree(widths);
	*ncols = nc;
	return cols;
}

static void DoLayout(Widget w)
{
	MwRudegridWidget rw = (MwRudegridWidget)w;
	Widget child;
	int *cols, *rows;
	int x, y;
	int w1, h1;
	int gx, gy;
	int gw, gh;
	int i, ncols, nrows;
	MwRudegridConstraints rc;

	cols = parse_layout(rw->core.width, rw->rudegrid.x_layout, &ncols);
	rows = parse_layout(rw->core.height, rw->rudegrid.y_layout, &nrows);

	for (i = 0; i < rw->composite.num_children; i++) {
		child = rw->composite.children[i];
		if (!child->core.managed) continue;

		rc = (MwRudegridConstraints)child->core.constraints;
		gx = rc->rudegrid.gridx;
		if (gx < 0) gx = 0;
		if (gx >= ncols) gx = ncols-1;
		gy = rc->rudegrid.gridy;
		if (gy < 0) gy = 0;
		if (gy >= nrows) gy = nrows-1;
		gw = rc->rudegrid.gridWidth;
		if (gw < 0) gw = 0;
		if (gw+gx >= ncols) gw = ncols-gx-1;
		gh = rc->rudegrid.gridHeight;
		if (gh < 0) gh = 0;
		if (gh+gy >= nrows) gh = nrows-gy-1;
		x = cols[gx];
		y = rows[gy];
		w1 = cols[gw+gx]-x;
		if (w1 < REALLYSMALL) w1 = REALLYSMALL;
		h1 = rows[gh+gy]-y;
		if (h1 < REALLYSMALL) h1 = REALLYSMALL;
		XtConfigureWidget(child, x, y, w1, h1, 0);
	}
	MwFree(cols);
	MwFree(rows);
}

static void MwRudegridResize(Widget w)
{
	DoLayout(w);
}

static void Initialize(Widget request, Widget new,
		ArgList args, Cardinal *num_args)
{
	MwRudegridWidget rw = (MwRudegridWidget)new;

	if (rw->core.height == 0) rw->core.height = DEFAULT_HEIGHT;
	if (rw->core.width == 0) rw->core.width = DEFAULT_WIDTH;
	if (rw->rudegrid.x_layout == NULL)
		rw->rudegrid.x_layout = MwStrdup("100%");
	else
		rw->rudegrid.x_layout = MwStrdup(rw->rudegrid.x_layout);
	if (rw->rudegrid.y_layout == NULL)
		rw->rudegrid.y_layout = MwStrdup("100%");
	else
		rw->rudegrid.y_layout = MwStrdup(rw->rudegrid.y_layout);
}

/* ARGSUSED */
static	Boolean
MwRudegridSetValues(current, request, new, args, num_args)
    Widget current, request, new;
    ArgList args;
    Cardinal *num_args;
{
	MwRudegridWidget oldrw = (MwRudegridWidget)current;
	MwRudegridWidget newrw = (MwRudegridWidget)new;
	if (newrw->rudegrid.x_layout != oldrw->rudegrid.x_layout) {
		MwFree(oldrw->rudegrid.x_layout);
		newrw->rudegrid.x_layout = MwStrdup(newrw->rudegrid.x_layout);
	}
	if (newrw->rudegrid.y_layout != oldrw->rudegrid.y_layout) {
		MwFree(oldrw->rudegrid.y_layout);
		newrw->rudegrid.y_layout = MwStrdup(newrw->rudegrid.y_layout);
	}
	DoLayout(new);
	return FALSE;
}


/* COMPOSITE WIDGET FUNCTIONS */

static void MwRudegridChangeManaged(Widget w)
{
	DoLayout(w);
}


/* Respond to size change requests from a child.
 */
static	XtGeometryResult
MwRudegridGeometryManager(w, request, reply)
    Widget w;
    XtWidgetGeometry *request;
    XtWidgetGeometry *reply;	/* RETURN */
{
	return XtGeometryYes;	/* don't try to be clever */
}


/* CONSTRAINT WIDGET FUNCTIONS */

/* ARGSUSED */
static	Boolean
MwRudegridConstraintSetValues(current, request, new, args, num_args)
    Widget current, request, new;
    ArgList args;
    Cardinal *num_args;
{
  return False ;		/* what does this signify? */
}

