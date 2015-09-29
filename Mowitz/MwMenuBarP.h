/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwMenuBarP_H_
#define _MwMenuBarP_H_

#include	"MwRowP.h"
#include	"MwMenuBar.h"

#if NeedFunctionPrototypes
typedef void (*unactivate_Proc) (Widget);
#else
typedef void (*unactivate_Proc) ();
#endif

#define XtInheritUnactivate ((unactivate_Proc) _XtInherit)

typedef struct {
	unactivate_Proc	unactivate;
} MwMenuBarClassPart;

typedef struct _MwMenuBarClassRec {
	CoreClassPart		core_class;
	CompositeClassPart	composite_class;
	ConstraintClassPart	constraint_class;
	MwBaseConstClassPart	baseConst_class;
	MwRowClassPart		row_class;
	MwMenuBarClassPart	menuBar_class;
} MwMenuBarClassRec;

extern MwMenuBarClassRec menuBarClassRec;

typedef struct {
	/* resources */
	/* private */
	Boolean		do_popdown;
	Widget 		*last_activated;
	Boolean		is_activated;
	Boolean		have_grab;
} MwMenuBarPart;

typedef struct _MwMenuBarRec {
	CorePart		core;
	CompositePart		composite;
	ConstraintPart		constraint;
	MwBaseConstPart		baseConst;
	MwRowPart			row;
	MwMenuBarPart		menuBar;
} MwMenuBarRec;


typedef struct _MwMenuBarConstraintsRec {
	MwRowConstraintsPart	row;
} MwMenuBarConstraintsRec, *MwMenuBarConstraints;

#endif
