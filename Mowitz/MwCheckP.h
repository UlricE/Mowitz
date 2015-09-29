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

#ifndef s_MwCheckP_h
#define s_MwCheckP_h

#include "MwCheck.h"
/* include superclass private header file */
#include <X11/CoreP.h>

/* define unique representation types not found in <X11/StringDefs.h> */

#define XtRCheckResource "CheckResource"

typedef struct {
    int empty;
} MwCheckClassPart;

typedef struct s_MwCheckClassRec {
    CoreClassPart	core_class;
    MwCheckClassPart	check_class;
} MwCheckClassRec;

extern MwCheckClassRec checkClassRec;

typedef struct {
    /* resources */
	XFontStruct *font;
	Boolean state;		/* True or False */
	MwCheckStyle style;	/* Siag[Check|Radio][Win|Motif] */
	Widget start;		/* Where to start searching for siblings */
	Widget group;		/* First widget in group, None for check btn */
	char *label;		/* text to display or NULL */
	XtCallbackList callbacks;
    /* private state */
	GC gc;
	Pixmap cmon, cmoff, tbon, tboff, r16on, r16off, rmon, rmoff;
} MwCheckPart;

typedef struct s_MwCheckRec {
    CorePart		core;
    MwCheckPart	check;
} MwCheckRec;

#endif /* s_MwCheckP_h */
