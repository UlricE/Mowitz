/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwMenuButtonP_H_
#define _MwMenuButtonP_H_

#include	"MwButtonP.h"
#include	"MwMenuButton.h"

typedef struct {
	char	empty;
} MwMenuButtonClassPart;

typedef struct _MwMenuButtonClassRec {
	CoreClassPart		core_class;
	MwBaseClassPart		base_class;
	MwSButtonClassPart	sButton_class;
	MwButtonClassPart		button_class;
	MwMenuButtonClassPart	mwMenuButton_class;
} MwMenuButtonClassRec;

typedef struct {
	/* resoureces */
	String		menu_name;
	/* private */
} MwMenuButtonPart;

typedef struct _MwMenuButtonRec{
	CorePart	core;
	MwBasePart	base;
	MwSButtonPart	sButton;
	MwButtonPart	button;
	MwMenuButtonPart	mwMenuButton;
} MwMenuButtonRec;
	
#endif
