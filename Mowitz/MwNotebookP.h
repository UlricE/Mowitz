/*
Copyright (C) 1999  Ulric Eriksson <ulric@siag.nu>

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

#ifndef s_MwNotebookP_h
#define s_MwNotebookP_h

#include "MwNotebook.h"
/* include superclass private header file */
#include <X11/CoreP.h>

/* define unique representation types not found in <X11/StringDefs.h> */

#define XtRNotebookResource "NotebookResource"

typedef struct {
    int empty;
} MwNotebookClassPart;

typedef struct s_MwNotebookClassRec {
    CoreClassPart	core_class;
    MwNotebookClassPart	notebook_class;
} MwNotebookClassRec;

extern MwNotebookClassRec mwNotebookClassRec;

typedef struct {
    /* resources */
	XFontStruct *font;
	int selected;
	void (*select)(Widget, int);
	int top;
	int ntext;
    /* private state */
	GC tab_gc;
	char **text;
} MwNotebookPart;

typedef struct s_MwNotebookRec {
    CorePart		core;
    MwNotebookPart	notebook;
} MwNotebookRec;

#endif /* s_MwNotebookP_h */
