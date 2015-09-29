/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwRowP_H_
#define _MwRowP_H_

#include	"MwBaseConstP.h"
#include	"MwRow.h"


typedef struct {
	char	empty;
} MwRowClassPart;

typedef struct _MwRowClassRec {
	CoreClassPart		core_class;
	CompositeClassPart	composite_class;
	ConstraintClassPart	constraint_class;
	MwBaseConstClassPart	baseConst_class;
	MwRowClassPart	row_class;
} MwRowClassRec;

extern MwRowClassRec mwRowClassRec;

typedef struct {
	/* resources */
	int		spacing;
	Boolean		homogenous;
	/* private */
} MwRowPart;

typedef struct _MwRowRec {
	CorePart		core;
	CompositePart		composite;
	ConstraintPart		constraint;
	MwBaseConstPart		baseConst;
	MwRowPart		row;
} MwRowRec;

typedef struct _MwRowConstraintsPart {
	int	gravitation;
	int	left_space;
	int	right_space;
	Boolean	resizable;
} MwRowConstraintsPart;

typedef struct _MwRowConstraintsRec {
	MwRowConstraintsPart	row;
} MwRowConstraintsRec, *MwRowConstraints;

#endif
