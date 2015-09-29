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

#ifndef MW_RICHTEXT_P_H
#define MW_RICHTEXT_P_H

#include "MwRichtext.h"
/* include superclass private header file */
#include <X11/CoreP.h>

/* define unique representation types not found in <X11/StringDefs.h> */

#define XtRRichtextResource "RichtextResource"

typedef struct {
    int empty;
} MwRichtextClassPart;

typedef struct s_RichtextClassRec {
    CoreClassPart	core_class;
    CompositeClassPart	composite_class;
    MwRichtextClassPart	richtext_class;
} MwRichtextClassRec;

extern MwRichtextClassRec mwRichtextClassRec;

typedef struct {
    /* resources */
	int top_row, top_col;
	int sel_top_row, sel_top_col;
	int sel_bottom_row, sel_bottom_col;
	int point_row, point_col;
	int (*row_height)(XtPointer, int);
	int (*style)(XtPointer, int);
	int (*adj_horiz)(XtPointer, int);
	MwRichchar *(*text)(XtPointer, int);
	XtPointer data;
	Boolean redisplay;
	Boolean visible_cursor;
	int paper_width, left_margin, right_margin;
	void (*plugin_coords)(Widget, XtPointer, int *, int *);
	Boolean (*bop)(XtPointer, int);
	float zoom;
	int format;
	MwRichchar *rc_string;
	int delay;
	Widget ruler;
    /* private state */
	GC clear_gc;
	GC cell_gc;
	GC cursor_gc;
	GC block_gc;
	XtIntervalId timeout;
	char *plain;
#ifdef HAVE_XCREATEIC
	XIM xim;
	XIC xic;
#endif
} MwRichtextPart;

typedef struct s_RichtextRec {
    CorePart		core;
    CompositePart	composite;
    MwRichtextPart	richtext;
} MwRichtextRec;

#endif /* MW_RICHTEXT_P_H */
