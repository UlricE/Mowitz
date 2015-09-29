/*
Copyright (C) 2001  Ulric Eriksson <ulric@siag.nu>

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

/* 
 * MwSpinnerP.h - Private definitions for MwSpinner widget
 */

#ifndef _MwSpinnerP_h
#define _MwSpinnerP_h

/***********************************************************************
 *
 * MwSpinner Widget Private Data
 *
 ***********************************************************************/

#include "MwSpinner.h"
#include <X11/Xmu/Converters.h>

/* New fields for the MwSpinner widget class record */
typedef struct {int empty;} MwSpinnerClassPart;

/* Full class record declaration */
typedef struct _MwSpinnerClassRec {
    CoreClassPart	core_class;
    CompositeClassPart  composite_class;
    MwFrameClassPart	frame_class;
    MwSpinnerClassPart	spinner_class;
} MwSpinnerClassRec;

extern MwSpinnerClassRec mwSpinnerClassRec;

/* New fields for the MwSpinner widget record */
typedef struct {
    /* resources */
	int min, max, step, value;
	XtCallbackList callbacks;

    /* private state */
	Widget up, down, text;
} MwSpinnerPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _MwSpinnerRec {
    CorePart	    core;
    CompositePart   composite;
    MwFramePart	    frame;
    MwSpinnerPart	    spinner;
} MwSpinnerRec;

#endif /* _MwSpinnerP_h */
