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

#ifndef MW_TABLE_P_H
#define MW_TABLE_P_H

#include "MwTable.h"
/* include superclass private header file */
#include <X11/CoreP.h>

/* define unique representation types not found in <X11/StringDefs.h> */

#define XtRTableResource "TableResource"

typedef struct {
    int empty;
} MwTableClassPart;

typedef struct s_TableClassRec {
    CoreClassPart	core_class;
    CompositeClassPart	composite_class;
    MwTableClassPart	table_class;
} MwTableClassRec;

extern MwTableClassRec mwTableClassRec;

typedef struct {
    /* resources */
	int max_row, max_col;
	int prot_row, prot_col;
	int top_row, top_col;
	int sel_top_row, sel_bottom_row;
	int sel_left_col, sel_right_col;
	int point_row, point_col;
	int default_width, default_height;
	int (*col_width)(XtPointer, int);
	int (*row_height)(XtPointer, int);
	int (*type)(XtPointer, int, int);
	char *(*text)(XtPointer, int, int);
	unsigned long (*format)(XtPointer, int, int);
	XtPointer data;
	Boolean redisplay;
	Boolean grid_lines;
	Boolean visible_cursor;
	void (*plugin_coords)(Widget, XtPointer, int *, int *);
	Boolean threeD;
	float zoom;
    /* private state */
	unsigned long white, grey, black;
	GC clear_gc;
	GC cell_gc;
	GC grid_gc;
	GC cursor_gc;
	GC block_gc;
	XtIntervalId timeout;
} MwTablePart;

typedef struct s_TableRec {
    CorePart		core;
    CompositePart	composite;
    MwTablePart	table;
} MwTableRec;

#endif /* MW_TABLE_P_H */
