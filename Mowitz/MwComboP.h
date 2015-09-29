/*
Copyright (C) 1998-2001  Ulric Eriksson <ulric@siag.nu>

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
 * MwComboP.h - Private definitions for MwCombo widget
 * 
 */

#ifndef _MwComboP_h
#define _MwComboP_h

/***********************************************************************
 *
 * MwCombo Widget Private Data
 *
 ***********************************************************************/

#include "MwCombo.h"
#include "MwFrameP.h"
#include <X11/Xmu/Converters.h>

/* New fields for the MwCombo widget class record */
typedef struct {int empty;} MwComboClassPart;

/* Full class record declaration */
typedef struct _MwComboClassRec {
    CoreClassPart	core_class;
    CompositeClassPart  composite_class;
    MwFrameClassPart    frame_class;
    MwComboClassPart	combo_class;
} MwComboClassRec;

extern MwComboClassRec mwComboClassRec;

/* New fields for the MwCombo widget record */
typedef struct {
    /* resources */
	XtCallbackList text_callbacks;
	XtCallbackList list_callbacks;
	Widget topLevel;

    /* private state */
	char **data;
	int ndata;
	Widget text, cmd, shell, viewport, list;
} MwComboPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _MwComboRec {
    CorePart	    core;
    CompositePart   composite;
    MwFramePart     frame;
    MwComboPart     combo;
} MwComboRec;

#endif /* _MwComboP_h */
