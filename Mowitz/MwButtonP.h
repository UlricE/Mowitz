/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwButtonP_H_
#define _MwButtonP_H_


#include	"MwSButtonP.h"
#include	"MwButton.h"


typedef struct {
	int empty;
} MwButtonClassPart;

typedef struct _MwButtonClassRec {
	CoreClassPart		core_class;
	MwBaseClassPart		base_class;
	MwSButtonClassPart	sButton_class;
	MwButtonClassPart		button_class;
} MwButtonClassRec;

extern MwButtonClassRec mwButtonClassRec;

typedef struct {
	/* resources */
	Icon		*icon;
	int		icon_justify;
	char 		*label;
	int		label_justify;
	XFontStruct	*font;
	int		spacing;
	/* hidden */
	GC		gc;
} MwButtonPart;

typedef struct _MwButtonRec {
	CorePart	core;
	MwBasePart	base;
	MwSButtonPart	sButton;
	MwButtonPart	button;
} MwButtonRec;

#endif
