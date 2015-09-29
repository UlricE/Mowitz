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

#ifndef MW_ANIMATOR_P_H
#define MW_ANIMATOR_P_H

#include "MwAnimator.h"
/* include superclass private header file */
#include <X11/CoreP.h>

/* define unique representation types not found in <X11/StringDefs.h> */

#define XtRAnimatorResource "AnimatorResource"

typedef struct {
    int empty;
} MwAnimatorClassPart;

typedef struct s_AnimatorClassRec {
    CoreClassPart	core_class;
    CompositeClassPart  composite_class;
    MwAnimatorClassPart	animator_class;
} MwAnimatorClassRec;

extern MwAnimatorClassRec mwAnimatorClassRec;

typedef struct {
    /* resources */
	MwAniObject *cast;
	int now, delta, duration;
	int newmode;
	char *bg_pixmap;
	char *bgrad;
	void (*plugin_coords)(Widget, XtPointer, int *, int *);
    /* private state */
	int mode;
	XtIntervalId timeout;
	Boolean waiting;
	GC gc;
	XImage *ximage;
} MwAnimatorPart;

typedef struct s_AnimatorRec {
    CorePart		core;
    CompositePart       composite;
    MwAnimatorPart	animator;
} MwAnimatorRec;

#endif /* MW_ANIMATOR_P_H */
