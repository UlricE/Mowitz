/*
Copyright (C) 1996-2001  Ulric Eriksson <ulric@siag.nu>

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

#ifndef s_MwCanvasP_h
#define s_MwCanvasP_h

#include "MwCanvas.h"
/* include superclass private header file */
#include <X11/CoreP.h>

/* define unique representation types not found in <X11/StringDefs.h> */

#define XtRCanvasResource "CanvasResource"

typedef struct {
    int empty;
} MwCanvasClassPart;

typedef struct s_MwCanvasClassRec {
    CoreClassPart	core_class;
    MwCanvasClassPart	canvas_class;
} MwCanvasClassRec;

extern MwCanvasClassRec mwCanvasClassRec;

typedef struct {
    /* resources */
	XtCallbackList callbacks;
    /* private state */
} MwCanvasPart;

typedef struct s_MwCanvasRec {
    CorePart		core;
    MwCanvasPart	canvas;
} MwCanvasRec;

#endif /* s_MwCanvasP_h */
