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

#include <Mowitz/Mowitz.h>
#include <X11/xpm.h>

#include "MwTableP.h"

static void plugin_coords(Widget, XtPointer, int *, int *);

static float floatOne = 1.0;

#define offset(field) XtOffsetOf(MwTableRec, table.field)
static XtResource resources[] = {
	{
		XtNtableMaxRow,		/* name */
		XtCTableMaxRow,		/* class */
		XtRInt,			/* type */
		sizeof(int),		/* size */
		offset(max_row),	/* offset */
		XtRImmediate,		/* default_type */
		(XtPointer)1000		/* default_addr */
	}, {
		XtNtableMaxCol,
		XtCTableMaxCol,
		XtRInt,
		sizeof(int),
		offset(max_col),
		XtRImmediate,
		(XtPointer)1000
	}, {
		XtNtableProtRow,
		XtCTableProtRow,
		XtRInt,
		sizeof(int),
		offset(prot_row),
		XtRImmediate,
		(XtPointer)1,
	}, {
		XtNtableProtCol,
		XtCTableProtCol,
		XtRInt,
		sizeof(int),
		offset(prot_col),
		XtRImmediate,
		(XtPointer)1,
	}, {
		XtNtableTopRow,
		XtCTableTopRow,
		XtRInt,
		sizeof(int),
		offset(top_row),
		XtRImmediate,
		(XtPointer)1
	}, {
		XtNtableTopCol,
		XtCTableTopCol,
		XtRInt,
		sizeof(int),
		offset(top_col),
		XtRImmediate,
		(XtPointer)1
	}, {
		XtNtableSelectTopRow,
		XtCTableSelectTopRow,
		XtRInt,
		sizeof(int),
		offset(sel_top_row),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNtableSelectBottomRow,
		XtCTableSelectBottomRow,
		XtRInt,
		sizeof(int),
		offset(sel_bottom_row),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNtableSelectLeftCol,
		XtCTableSelectLeftCol,
		XtRInt,
		sizeof(int),
		offset(sel_left_col),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNtableSelectRightCol,
		XtCTableSelectRightCol,
		XtRInt,
		sizeof(int),
		offset(sel_right_col),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNtablePointRow,
		XtCTablePointRow,
		XtRInt,
		sizeof(int),
		offset(point_row),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNtablePointCol,
		XtCTablePointCol,
		XtRInt,
		sizeof(int),
		offset(point_col),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNtableDefaultWidth,
		XtCTableDefaultWidth,
		XtRInt,
		sizeof(int),
		offset(default_width),
		XtRImmediate,
		(XtPointer)80
	}, {
		XtNtableDefaultHeight,
		XtCTableDefaultHeight,
		XtRInt,
		sizeof(int),
		offset(default_height),
		XtRImmediate,
		(XtPointer)20
	}, {
		XtNtableColWidth,
		XtCTableColWidth,
		XtRPointer,
		sizeof(XtPointer),
		offset(col_width),
		XtRImmediate,
		(XtPointer)NULL
	}, {
		XtNtableRowHeight,
		XtCTableRowHeight,
		XtRPointer,
		sizeof(XtPointer),
		offset(row_height),
		XtRImmediate,
		(XtPointer)NULL
	}, {
		XtNtableType,
		XtCTableType,
		XtRPointer,
		sizeof(XtPointer),
		offset(type),
		XtRImmediate,
		(XtPointer)NULL
	}, {
		XtNtableText,
		XtCTableText,
		XtRPointer,
		sizeof(XtPointer),
		offset(text),
		XtRImmediate,
		(XtPointer)NULL
	}, {
		XtNtableFormat,
		XtCTableFormat,
		XtRPointer,
		sizeof(XtPointer),
		offset(format),
		XtRImmediate,
		(XtPointer)NULL
	}, {
		XtNtableData,
		XtCTableData,
		XtRPointer,
		sizeof(XtPointer),
		offset(data),
		XtRImmediate,
		(XtPointer)NULL
	}, {
		XtNtableRedisplay,
		XtCTableRedisplay,
		XtRBoolean,
		sizeof(Boolean),
		offset(redisplay),
		XtRImmediate,
		(XtPointer)False
	}, {
		XtNtableGridLines,
		XtCTableGridLines,
		XtRBoolean,
		sizeof(Boolean),
		offset(grid_lines),
		XtRImmediate,
		(XtPointer)True
	}, {
		XtNtableVisibleCursor,
		XtCTableVisibleCursor,
		XtRBoolean,
		sizeof(Boolean),
		offset(visible_cursor),
		XtRImmediate,
		(XtPointer)False
	}, {
		XtNtablePluginCoords,
		XtCTablePluginCoords,
		XtRPointer,
		sizeof(XtPointer),
		offset(plugin_coords),
		XtRImmediate,
		(XtPointer)plugin_coords
	}, {
		XtNtable3D,
		XtCTable3D,
		XtRBoolean,
		sizeof(Boolean),
		offset(threeD),
		XtRImmediate,
		(XtPointer)False
	}, {
		XtNzoom,
		XtCZoom,
		XtRFloat,
		sizeof(float),
		offset(zoom),
		XtRFloat,
		(XtPointer)&floatOne
	}
};
#undef offset

/* methods */
static void DoLayout(MwTableWidget);
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
static void TableAction(Widget, XEvent *, String *, Cardinal *);

static XtActionsRec actions[] =
{
	{"table", TableAction},
};

/* translations */
static char translations[] =
"<Key>:		table()		\n";

MwTableClassRec mwTableClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) &compositeClassRec,
    /* class_name		*/	"MwTable",
    /* widget_size		*/	sizeof(MwTableRec),
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
  },{ /* table fields */
    /* empty			*/	0
  }
};

WidgetClass mwTableWidgetClass = (WidgetClass)&mwTableClassRec;


/* supporting code copied from window.c */

static void TableAction(Widget w, XEvent *event, String *params, Cardinal *n)
{
	;	/* application should create its own actions */
}

static int return_type(Widget w, int row, int col)
{
	MwTableWidget tw = (MwTableWidget) w;
	if (tw->table.type)
		return (*tw->table.type)(tw->table.data, row, col);
	return MW_TABLE_TEXT;
}

static char *return_pvalue(Widget w, int row, int col)
{
	MwTableWidget tw = (MwTableWidget) w;
	if (tw->table.text)
		return (*tw->table.text)(tw->table.data, row, col);
	else return NULL;
}

static MwFmt fmt0 = {
	"Helvetica",	/* font family */
	120,		/* size in decipoints */
	False,		/* bold */
	False,		/* italic */
	False,		/* underline */
	False,		/* strikethrough */
	"black",	/* foreground */
	"white",	/* background */
	0,		/* borders */
	MW_VADJ_CENTER,	/* vertical adjust */
	MW_HADJ_LEFT,	/* horizontal adjust */
	0};		/* style */

static int return_format(Widget w, int row, int col)
{
	static int fmt = -1;
	MwTableWidget tw = (MwTableWidget) w;

	if (tw->table.format)
		return (*tw->table.format)(tw->table.data, row, col);
	if (fmt == -1) fmt = MwEncodeFormat(~0, &fmt0);
	return fmt;
}

static Dimension cell_width(MwTableWidget tw, int col)
{
	Dimension wi;

	if (tw->table.col_width) {
		wi = (*tw->table.col_width)(tw->table.data, col);
		return wi;
	}
	return tw->table.default_width;
}

static Dimension cell_height(MwTableWidget tw, int row)
{
	Dimension h;
	if (tw->table.row_height) {
		h = (*tw->table.row_height)(tw->table.data, row);
		return h;
	}
	return tw->table.default_height;
}

/* ---
*/
void table_global_coords(MwTableWidget tw,
		int row, int col, int *x, int *y)
{
	int i;

	*x = *y = 0;

	for (i = 1; i < col; i++)
		*x += cell_width(tw, i);

	for (i = 1; i < row; i++)
		*y += cell_height(tw, i);
}

static int inblock(MwTableWidget tw, int r, int c)
{
	return r >= tw->table.sel_top_row && r <= tw->table.sel_bottom_row
		&& c >= tw->table.sel_left_col && c <= tw->table.sel_right_col;
}

/*
x_base and y_base are in zoomed pixel coordinates
*/
static void draw_cell(MwTableWidget tw, Drawable scribble,
			int row, int col, int x_base, int y_base)
{
	Widget w = (Widget) tw;
	char *p;
	XColor color;
	int x_pos, y_pos;
	int text_height, text_width;
	int cw, ch;
	int ct;
	int f;
	MwFmt fmt;
	int fd;
	MwRichchar *rcp;
	float zoom = tw->table.zoom;
	GC grid_gc = tw->table.grid_gc;
	GC cell_gc = tw->table.cell_gc;
	Display *dpy = XtDisplay(w);

	if (row > tw->table.max_row || col > tw->table.max_col) return;

	cw = cell_width(tw, col);
	ch = cell_height(tw, row);

	f = return_format(w, row, col);
	MwDecodeFormat(f, ~0, &fmt);

	ct = return_type(w, row, col);
	if (ct == MW_TABLE_EMBED) {
#if 0
		char *p = return_pvalue(w, row, col);
		embed_draw(scribble, x_base, y_base, p);
#else
		fprintf(stderr, "No, no, no\n"
			"Embedded bitmaps have been deprecated for years.\n");
#endif
		return;
	}

	if (ct == MW_TABLE_PIXMAP) {
		XtWarning("The Table widget does not yet display pixmaps");
		return;
	}

	MwAllocNamedColor(dpy, fmt.fg, &color);
	XSetForeground(dpy, cell_gc, color.pixel);

	p = return_pvalue(w, row, col);
	if (p == NULL) p = "";
	rcp = MwRcMakerich(p, f);

	if (strlen(p) > 1000) p[1000] = '\0';

	text_width = MwRcStrwidth(rcp, -1);
	text_height = MwRcStrheight(rcp, -1);

	switch (fmt.hadj) {
	case MW_HADJ_CENTER:
		x_pos = (cw-text_width) / 2;
		break;
	case MW_HADJ_RIGHT:
		x_pos = cw-text_width-5;
		break;
	default:	/* HADJ_LEFT */
		x_pos = 5;
	}

	fd = text_height/4;	/* font descent heuristic */
	switch (fmt.vadj) {
	case MW_VADJ_BOTTOM:
		y_pos = ch-5;
		break;
	case MW_VADJ_TOP:
		y_pos = text_height+fd;
		break;
	default:	/* VADJ_CENTER */
		y_pos = (ch+text_height)/2 -fd;
	}

	MwRcStrdraw(scribble, cell_gc,
		x_base, y_base, x_pos, y_pos,
		rcp, -1, zoom);
	MwFree(rcp);

	/* grid lines */
	XSetForeground(dpy, grid_gc, tw->table.grey);
	if (tw->table.grid_lines) {
		int x1 = x_base, y1 = y_base;
		int x2 = x_base+zoom*(cw-1), y2 = y_base+zoom*(ch-1);
		XDrawLine(dpy, scribble, grid_gc, x1, y2, x2, y2);
		XDrawLine(dpy, scribble, grid_gc, x2, y2, x2, y1);
	}

	/* these lines are from the format */
	XSetForeground(dpy, grid_gc, tw->table.black);
	if (fmt.borders & MW_BORDER_LEFT)
		XDrawLine(dpy, scribble, grid_gc, x_base, y_base,
			x_base, y_base+zoom*(ch-2));
	if (fmt.borders & MW_BORDER_RIGHT)
		XDrawLine(dpy, scribble, grid_gc,
			x_base+zoom*(cw-2), y_base,
			x_base+zoom*(cw-2), y_base+zoom*(ch-2));
	if (fmt.borders & MW_BORDER_TOP)
		XDrawLine(dpy, scribble, grid_gc,
			x_base, y_base,
			x_base+zoom*(cw-2), y_base);
	if (fmt.borders & MW_BORDER_BOTTOM)
		XDrawLine(dpy, scribble, grid_gc,
			x_base, y_base+zoom*(ch-2),
			x_base+zoom*(cw-2), y_base+zoom*(ch-2));

	/* makebelieve 3D */
	if (tw->table.threeD) {
		XDrawLine(dpy, scribble, grid_gc,
			x_base, y_base+zoom*(ch-1),
			x_base+zoom*(cw-1), y_base+zoom*(ch-1));
		XDrawLine(dpy, scribble, grid_gc,
			x_base+zoom*(cw-1), y_base,
			x_base+zoom*(cw-1), y_base+zoom*(ch-1));
		XSetForeground(dpy, grid_gc,
				WhitePixelOfScreen(XtScreen(w)));
		XDrawLine(dpy, scribble, grid_gc,
			x_base, y_base,
			x_base, y_base+zoom*(ch-2));
		XDrawLine(dpy, scribble, grid_gc,
			x_base, y_base,
			x_base+zoom*(cw-2), y_base);
	}
}

static void erase_cell(MwTableWidget tw, Drawable scribble,
			int row, int col, int x_base, int y_base)
{
	Widget w = (Widget) tw;
	XColor color;
	int cw, ch;
	float zoom = tw->table.zoom;
	int f;
	MwFmt fmt;

	if (row > tw->table.max_row || col > tw->table.max_col) return;

	cw = cell_width(tw, col);
	ch = cell_height(tw, row);

	if (inblock(tw, row, col)) {
		XFillRectangle(XtDisplay(w), scribble, tw->table.block_gc,
			x_base, y_base, zoom*cw, zoom*ch);
	} else {
		f = return_format(w, row, col);
		MwDecodeFormat(f, MW_FMT_BG, &fmt);
		MwAllocNamedColor(XtDisplay(w), fmt.bg, &color);
		XSetForeground(XtDisplay(w), tw->table.cell_gc,
			color.pixel);
		XFillRectangle(XtDisplay(w), scribble, tw->table.cell_gc,
			x_base, y_base, zoom*cw, zoom*ch);
	}
}

/*
y_base is in zoomed pixels */
static void cell_row(MwTableWidget tw, Drawable scribble,
			int width, int y_base, int i)
{
	int j, x_base;
	float zoom = tw->table.zoom;

	x_base = 0;
	for (j = 1; (j < tw->table.prot_col) && (x_base < width); j++) {
		erase_cell(tw, scribble, i, j, x_base, y_base);
		x_base += zoom*cell_width(tw, j);
	}
	for (j = tw->table.top_col; x_base < width; j++) {
		erase_cell(tw, scribble, i, j, x_base, y_base);
		x_base += zoom*cell_width(tw, j);
	}

	x_base = 0;
	for (j = 1; (j < tw->table.prot_col) && (x_base < width); j++) {
		draw_cell(tw, scribble, i, j, x_base, y_base);
		x_base += zoom*cell_width(tw, j);
	}
	for (j = tw->table.top_col; x_base < width; j++) {
		draw_cell(tw, scribble, i, j, x_base, y_base);
		x_base += zoom*cell_width(tw, j);
	}
}

static void draw_table(MwTableWidget tw, Drawable d)
{
	int y_base = 0;
	int width = tw->core.width;
	int height = tw->core.height;
	int i;
	float zoom = tw->table.zoom;

	for (i = 1; (i < tw->table.prot_row) && (y_base < height); i++) {
		cell_row(tw, d, width, y_base, i);
		y_base += zoom*cell_height(tw, i);
	}
	for (i = tw->table.top_row; y_base < height; i++) {
		cell_row(tw, d, width, y_base, i);
		y_base += zoom*cell_height(tw, i);
	}
}

static int cell_next_row(MwTableWidget w, int row)
{
        if (row+1 == w->table.prot_row) return w->table.top_row;
        return row+1;
}

static int cell_next_col(MwTableWidget w, int col)
{
        if (col+1 == w->table.prot_col) return w->table.top_col;
        return col+1;
}

static int cell_prev_row(MwTableWidget w, int row)
{
        if (row == w->table.top_row) return w->table.prot_row-1;
        return row-1;
}

static int cell_prev_col(MwTableWidget w, int col)
{
        if (col == w->table.top_col) return w->table.prot_col-1;
        return col-1;
}

/* ---
*/
void table_cell2coords(Widget w,
			int cell_row, int cell_col,
			int *cell_x, int *cell_y)
{
	MwTableWidget tw = (MwTableWidget)w;
        int i;
        *cell_y = 0;
        for (i = 1; i < tw->table.prot_row; i++)
                *cell_y += cell_height(tw, i);

        while (cell_row < tw->table.top_row) {
                *cell_y -= cell_height(tw, cell_row);
                cell_row = cell_next_row(tw, cell_row);
        }
        while (cell_row > tw->table.top_row) {
                cell_row = cell_prev_row(tw, cell_row);
                *cell_y += cell_height(tw, cell_row);
        }
        *cell_x = 0;
        for (i = 1; i < tw->table.prot_col; i++)
                *cell_x += cell_width(tw, i);

        while (cell_col < tw->table.top_col) {
                *cell_x -= cell_width(tw, cell_col);
                cell_col = cell_next_col(tw, cell_col);
        }
        while (cell_col > tw->table.top_col) {
                cell_col = cell_prev_col(tw, cell_col);
                *cell_x += cell_width(tw, cell_col);
        }
}

void MwTableZoomedCellToCoords(Widget w,
			int cell_row, int cell_col,
			int *cell_x, int *cell_y)
{
	MwTableWidget tw = (MwTableWidget)w;
	float zoom = tw->table.zoom;
        int i;
        *cell_y = 0;

        for (i = 1; i < tw->table.prot_row; i++)
                *cell_y += zoom*cell_height(tw, i);

        while (cell_row < tw->table.top_row) {
                *cell_y -= zoom*cell_height(tw, cell_row);
                cell_row = cell_next_row(tw, cell_row);
        }

        while (cell_row > tw->table.top_row) {
                cell_row = cell_prev_row(tw, cell_row);
                *cell_y += zoom*cell_height(tw, cell_row);
        }

        *cell_x = 0;
        for (i = 1; i < tw->table.prot_col; i++)
                *cell_x += zoom*cell_width(tw, i);

        while (cell_col < tw->table.top_col) {
                *cell_x -= zoom*cell_width(tw, cell_col);
                cell_col = cell_next_col(tw, cell_col);
        }

        while (cell_col > tw->table.top_col) {
                cell_col = cell_prev_col(tw, cell_col);
                *cell_x += zoom*cell_width(tw, cell_col);
        }
}

/* ---
*/
void table_coords2cell(MwTableWidget tw,
			int *cur_row, int *cur_col,
			int cur_x, int cur_y)
{
        int prot_x = 0, prot_y = 0, i;

        for (i = 1; i < tw->table.prot_col; i++)
              cur_x -= cell_width(tw, i);
        for (i = 1; i < tw->table.prot_row; i++)
              cur_y -= cell_height(tw, i);

        *cur_row = tw->table.top_row;
        *cur_col = tw->table.top_col;
        while (cur_y < prot_y) {
                cur_y += cell_height(tw, *cur_row);
                (*cur_row) = cell_prev_row(tw, *cur_row);
        }
        while (cur_y > cell_height(tw, *cur_row)) {
                cur_y -= cell_height(tw, *cur_row);
                (*cur_row) = cell_next_row(tw, *cur_row);
        }
        while (cur_x < prot_x) {
                cur_x += cell_width(tw, *cur_col);
                (*cur_col) = cell_prev_col(tw, *cur_col);
        }
        while (cur_x > cell_width(tw, *cur_col)) {
                cur_x -= cell_width(tw, *cur_col);
                (*cur_col) = cell_next_col(tw, *cur_col);
        }
}

/* ---
create, draw and return a pixmap of the stage at time now.
   Caller must free
Strategy:
   1. Clear the window, or create a new empty Pixmap
   2. Paint a rectangle for the block, if there is one (coords != 0)
   3. Draw the grid
   4. Draw the data
   5. Draw the cursor using XOR, if one is desired (coords != 0)
   The purpose of using XOR is to make sure the cursor is visible
   on any background. Technicolor is a potential side effect.
*/

Pixmap MwTablePixmap(Widget w)
{
	Pixmap scribble;
	MwTableWidget tw = (MwTableWidget)w;
	int width, height;

	width = tw->core.width;
	height = tw->core.height;

	if (width > 2000 || height > 2000) return None;

	scribble = XCreatePixmap(XtDisplay(w), XtWindow(w),
		width, height, tw->core.depth);

	draw_table(tw, scribble);
	return scribble;
}


static GC get_gc(Widget w, unsigned long fg, unsigned long bg)
{
	unsigned long valuemask = 0;
	XGCValues values;
	GC gc = XCreateGC(XtDisplay(w), XtWindow(w),
				valuemask, &values);
	XSetForeground(XtDisplay(w), gc, fg);
	XSetBackground(XtDisplay(w), gc, bg);
	return gc;
}

static void Initialize(Widget req, Widget new,
		ArgList args, Cardinal *nargs)
{
	MwTableWidget tw = (MwTableWidget)new;

	if (tw->core.height == 0) tw->core.height = 200;
	if (tw->core.width == 0) tw->core.width = 200;
}

#define superclass (&coreClassRec)
static void Realize(Widget w, XtValueMask *valueMask,
		XSetWindowAttributes *attributes)
{
	MwTableWidget tw = (MwTableWidget) w;
	XColor screen_color, exact_color;

	(*superclass->core_class.realize) (w, valueMask, attributes);
	tw->table.black = BlackPixelOfScreen(XtScreen(w));
	tw->table.white = tw->core.background_pixel;
	XAllocNamedColor(XtDisplay(w),
		DefaultColormap(XtDisplay(w), DefaultScreen(XtDisplay(w))),
		"gray", &screen_color, &exact_color);
	tw->table.grey = screen_color.pixel;
	tw->table.clear_gc = get_gc(w, tw->table.white, tw->table.black);
	tw->table.cell_gc = get_gc(w, tw->table.black, tw->table.white);
	tw->table.grid_gc = get_gc(w, tw->table.grey, tw->table.white);
#if 1	/* this is too much */
	XSetLineAttributes(XtDisplay(w), tw->table.grid_gc,
			0, LineSolid, CapButt, JoinMiter);
#endif
	tw->table.cursor_gc = get_gc(w, tw->table.black^tw->table.white, 0);
	XSetFunction(XtDisplay(w), tw->table.cursor_gc, GXxor);
	XSetLineAttributes(XtDisplay(w), tw->table.cursor_gc,
			2, LineSolid, CapButt, JoinMiter);
#if 1	/* grey block */
	tw->table.block_gc = get_gc(w, tw->table.grey, tw->table.white);
#else	/* reverse block */
	tw->table.block_gc = get_gc(w, tw->table.black^tw->table.white, 0);
	XSetFunction(XtDisplay(w), tw->table.block_gc, GXxor);
#endif
	tw->table.timeout = None;
}

static void Destroy(Widget w)
{
	MwTableWidget tw = (MwTableWidget) w;

	XFreeGC(XtDisplay(w), tw->table.clear_gc);
	XFreeGC(XtDisplay(w), tw->table.cell_gc);
	XFreeGC(XtDisplay(w), tw->table.grid_gc);
	XFreeGC(XtDisplay(w), tw->table.cursor_gc);
	XFreeGC(XtDisplay(w), tw->table.block_gc);

	if (tw->table.timeout)
		XtRemoveTimeOut(tw->table.timeout);
}


static Boolean move_top(MwTableWidget tw)
{
	Boolean pr_scr_flag = False;
	int cur_x, cur_y, prot_x = 0, prot_y = 0, i;
	unsigned int width, height;
	float zoom = tw->table.zoom;

        for (i = 1; i < tw->table.prot_col; i++)
                prot_x += cell_width(tw, i);
        for (i = 1; i < tw->table.prot_row; i++)
                prot_y += cell_height(tw, i);

        /* Figure out how big the window is */
	width = tw->core.width/zoom;	/* unzoomed size */
	height = tw->core.height/zoom;

        table_cell2coords((Widget)tw,
                        tw->table.point_row, tw->table.point_col,
                        &cur_x, &cur_y);
        /* this isn't efficient, but it will work */
	while (cur_y < prot_y) {
		tw->table.top_row--;
		cur_y += cell_height(tw, tw->table.top_row);
		pr_scr_flag = TRUE;
	}
	while (cur_y + cell_height(tw, tw->table.point_row) > height) {
		cur_y -= cell_height(tw, tw->table.top_row);
		tw->table.top_row++;
		pr_scr_flag = TRUE;
	}
	while (cur_x < prot_x) {
		tw->table.top_col--;
		cur_x += cell_width(tw, tw->table.top_col);
		pr_scr_flag = TRUE;
	}
	while (cur_x + cell_width(tw, tw->table.point_col) > width) {
		cur_x -= cell_width(tw, tw->table.top_col);
		tw->table.top_col++;
		pr_scr_flag = TRUE;
	}
	return pr_scr_flag;
}

static void toggle_cursor(MwTableWidget tw)
{
	int row = tw->table.point_row, col = tw->table.point_col;
	int cur_x, cur_y;
	GC gc = tw->table.cursor_gc;
	unsigned long bg;
	float zoom = tw->table.zoom;

	MwTableZoomedCellToCoords((Widget)tw, row, col, &cur_x, &cur_y);
	if (row >= tw->table.sel_top_row && row <= tw->table.sel_bottom_row &&
		col >= tw->table.sel_left_col && col <= tw->table.sel_right_col) {
		bg = tw->table.grey;
	} else {
		bg = tw->table.white;
	}
	XSetForeground(XtDisplay((Widget)tw), gc, tw->table.black^bg);
	XDrawRectangle(XtDisplay((Widget)tw), XtWindow((Widget)tw),
		tw->table.cursor_gc,
		cur_x+2, cur_y+2,
		zoom*cell_width(tw, col)-5, zoom*cell_height(tw, row)-5);
}

static void do_redisplay(XtPointer client_data, XtIntervalId *id)
{
	Widget w = (Widget)client_data;
	Pixmap scribble;
	GC gc;
	unsigned long valuemask = 0;
	XGCValues values;
	MwTableWidget tw = (MwTableWidget) w;

	if (!XtIsRealized(w)) return;	/* but that doesn't work */
	if (!XtIsManaged(w)) return;	/* what about this */
	scribble = MwTablePixmap(w);
	if (scribble == None) return;
	gc = XCreateGC(XtDisplay(w), XtWindow(w),
			valuemask, &values);
	XCopyArea(XtDisplay(w), scribble, XtWindow(w),
		gc, 0, 0, tw->core.width, tw->core.height, 0, 0);
	XFreePixmap(XtDisplay(w), scribble);
	XFreeGC(XtDisplay(w), gc);

	/* draw the cursor, perhaps? */
	if (tw->table.visible_cursor) {
		toggle_cursor(tw);
	}
	/* update plugin positions */
	DoLayout(tw);

	/* clear the timeout */
	tw->table.timeout = None;
}

static void Redisplay(Widget w, XEvent *event, Region r)
{
	MwTableWidget tw = (MwTableWidget)w;
	if (tw->table.timeout) return;	/* already set */
	tw->table.timeout = XtAppAddTimeOut(
			XtWidgetToApplicationContext(w),
			50, do_redisplay, (XtPointer)w);
}

/* ---
A dilemma: if Xt handles the redisplaying, it also takes care of
   compressing expose events. On the other hand, it clears the window
   which makes it flicker. We'll redisplay ourselves and try to
   make it more efficient later.
*/

static Boolean SetValues(Widget current, Widget request, Widget new,
		ArgList args, Cardinal *n)
{
	MwTableWidget curtw = (MwTableWidget) current;
	MwTableWidget newtw = (MwTableWidget) new;
	Boolean do_redisplay = False;

	do_redisplay |= (curtw->table.sel_top_row != newtw->table.sel_top_row
		|| curtw->table.sel_bottom_row != newtw->table.sel_bottom_row
		|| curtw->table.sel_left_col != newtw->table.sel_left_col
		|| curtw->table.sel_right_col != newtw->table.sel_right_col);

	if (newtw->table.visible_cursor &&
	    (newtw->table.point_row != curtw->table.point_row ||
	     newtw->table.point_col != curtw->table.point_col ||
	     newtw->table.redisplay))
		do_redisplay |= move_top(newtw);

	if (newtw->table.redisplay) {
		do_redisplay = True;
		newtw->table.redisplay = False;
	}
	/* can't let Xt handle this because it flickers */
	if (do_redisplay) {
		Redisplay(new, NULL, None);
		do_redisplay = False;
	} else 
	if (!do_redisplay) {
		if (curtw->table.visible_cursor) {
			toggle_cursor(curtw);
		}
		if (newtw->table.visible_cursor) {
			toggle_cursor(newtw);
		}
	}
	return do_redisplay;
}

/*
 * Do a layout, actually assigning positions.
 */

static void DoLayout(MwTableWidget sw)
{
	int i;
	int x, y;

	for (i = 0; i < sw->composite.num_children; i++) {
		(*sw->table.plugin_coords)(sw->composite.children[i],
			sw->table.data, &x, &y);
		XtMoveWidget(sw->composite.children[i], x, y);
	}
}

/*
 * Actually layout the table
 */

static void Resize(Widget w)
{
        DoLayout((MwTableWidget)w);
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
    DoLayout((MwTableWidget)w);
}

void MwTableSetZoom(Widget w, float zoom)
{
	MwTableWidget tw = (MwTableWidget)w;

	if (zoom < .1) zoom = .1;
	if (zoom > 10) zoom = 10;
	if (tw->table.zoom != zoom) {
		tw->table.zoom = zoom;
		Redisplay(w, NULL, None);
	}
}

