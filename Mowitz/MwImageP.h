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

#ifndef s_MwImageP_h
#define s_MwImageP_h

#include "MwImage.h"
/* include superclass private header file */
#include <X11/CoreP.h>

/* define unique representation types not found in <X11/StringDefs.h> */

#define XtRImageResource "ImageResource"

typedef struct {
    int empty;
} MwImageClassPart;

typedef struct s_MwImageClassRec {
    CoreClassPart	core_class;
    MwImageClassPart	image_class;
} MwImageClassRec;

extern MwImageClassRec mwImageClassRec;

typedef struct {
    /* resources */
	Pixmap bitmap;
    /* private state */
	GC gc;
	Pixmap bg;
	XtIntervalId timeout;
} MwImagePart;

typedef struct s_MwImageRec {
    CorePart		core;
    MwImagePart	image;
} MwImageRec;

#endif /* s_MwImageP_h */
