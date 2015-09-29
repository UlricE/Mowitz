/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwSButtonP_H_
#define _MwSButtonP_H_


#include	"MwBaseP.h"
#include	"MwSButton.h"


typedef struct {
	char empty;
} MwSButtonClassPart;

typedef struct _MwSButtonClassRec {
	CoreClassPart		core_class;
	MwBaseClassPart		base_class;
	MwSButtonClassPart	sButton_class;
} MwSButtonClassRec;

extern MwSButtonClassRec mwSButtonClassRec;

typedef struct {
	/* resources */
	XtCallbackList  activate;
	XtCallbackList	switchcb;
	Cardinal	init_delay;
	Cardinal	repeat_delay;
	int		mode;
	/* hidden */
	XtIntervalId	timer;
	Boolean		pressed;
	Boolean		on;
} MwSButtonPart;

typedef struct _MwSButtonRec {
	CorePart	core;
	MwBasePart	base;
	MwSButtonPart	sButton;
} MwSButtonRec;

#endif
