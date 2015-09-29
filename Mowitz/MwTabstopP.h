/*
Copyright (C) 2000-2001  Ulric Eriksson <ulric@siag.nu>

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

#ifndef s_MwTabstopP_h
#define s_MwTabstopP_h

#include "MwTabstop.h"
/* include superclass private header file */
#include <X11/CoreP.h>

/* define unique representation types not found in <X11/StringDefs.h> */

#define XtRMwTabstopResource "MwTabstopResource"

typedef struct {
    int empty;
} MwTabstopClassPart;

typedef struct s_MwTabstopClassRec {
    CoreClassPart	core_class;
    MwTabstopClassPart	mwTabstop_class;
} MwTabstopClassRec;

extern MwTabstopClassRec mwTabstopClassRec;

typedef struct {
    /* resources */
	XFontStruct *font;
	int left_margin, right_margin;
	int paper_width;
	float zoom;
	int top_col;
	String tabs;
	XtCallbackList callbacks;
    /* private state */
	GC gc;
	MwTabstop *tt;
	Pixmap ltab, ctab, rtab;
} MwTabstopPart;

typedef struct s_MwTabstopRec {
    CorePart		core;
    MwTabstopPart		mwTabstop;
} MwTabstopRec;

#endif /* s_MwTabstopP_h */
