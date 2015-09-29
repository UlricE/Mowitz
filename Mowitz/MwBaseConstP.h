/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwBaseConstP_H_
#define _MwBaseConstP_H_

#include <X11/ConstrainP.h>
#include "MwBaseConst.h"
#include "MwNws.h"


typedef struct {
	get_internal_dimension_Proc	get_internal_dimension;
	set_internal_dimension_Proc	set_internal_dimension;
	traverse_Proc			traverse;
	traverseTo_Proc			traverseTo;
	traverseOut_Proc		traverseOut;
	traverseInside_Proc		traverseInside;
	highlightBorder_Proc		highlightBorder;
	unhighlightBorder_Proc		unhighlightBorder;
} MwBaseConstClassPart;

typedef struct _MwBaseConstClassRec {
	CoreClassPart		core_class;
	CompositeClassPart	composite_class;
	ConstraintClassPart	constraint_class;
	MwBaseConstClassPart	baseConst_class;
} MwBaseConstClassRec;

extern MwBaseConstClassRec mwBaseConstClassRec;

typedef struct _MwBaseConstPart {
	/* public resources */
	Boolean		highlight_on_enter;
	int		help_show_delay;
	Cursor		cursor;
	int		box_type;
	int		box_width;
	int		box_intensity;
	Pixel		box_color;
	XFontStruct	*font;
	String		label;
	String		help_text;
	Pixel		foreground;
	int		justify;
	int		traverse_direction;
	int		bd_width;
	Pixel		bd_color;
	int             top_shadow_contrast;
        int             bot_shadow_contrast;
	/* private */
	XtIntervalId	timer;
	Widget		hlp;
	GC		gc;
	Pixel		dark,light;
	Boolean		have_shape;
	Boolean		focused;
	Boolean		traverse;
	Widget		current_focused;
} MwBaseConstPart;

typedef struct _MwBaseConstConstraintsPart {
        int		dummy;
} MwBaseConstConstraintsPart;

typedef struct _MwBaseConstConstraintsRec {
        MwBaseConstConstraintsPart  cardBox;
} MwBaseConstConstraintsRec, *MwBaseConstConstraints;
 

typedef struct _MwBaseConstRec {
	CorePart	core;
	CompositePart	composite;
	ConstraintPart	constraint;
	MwBaseConstPart	baseConst;
} MwBaseConstRec;

#endif
