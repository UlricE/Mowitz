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

#ifndef MW_TABBING_P_H
#define MW_TABBING_P_H

#include "MwTabbing.h"
/* include superclass private header file */
#include <X11/CoreP.h>

/* define unique representation types not found in <X11/StringDefs.h> */

#define XtRTabbingResource "TabbingResource"

typedef struct {
    int empty;
} MwTabbingClassPart;

typedef struct s_TabbingClassRec {
    CoreClassPart	core_class;
    MwTabbingClassPart	tabbing_class;
} MwTabbingClassRec;

extern MwTabbingClassRec mwTabbingClassRec;

typedef struct {
    /* resources */
	XFontStruct *font;
	int selected;
#if 0
	void (*select)(Widget, int);
#else
	XtCallbackList select_callbacks;
	XtCallbackList rename_callbacks;
#endif
	int top;
	int ntext;
    /* private state */
	GC tab_gc;
	char **text;
} MwTabbingPart;

typedef struct s_TabbingRec {
    CorePart		core;
    MwTabbingPart	tabbing;
} MwTabbingRec;

#endif /* MW_TABBING_P_H */
