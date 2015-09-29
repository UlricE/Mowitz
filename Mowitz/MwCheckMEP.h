/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwCheckMEP_H_
#define _MwCheckMEP_H_

#include	"MwLabelMEP.h"
#include	"MwCheckME.h"

typedef struct _MwCheckMEClassPart {
	char	empty;
} MwCheckMEClassPart;

typedef struct _MwCheckMEClassRec {
	RectObjClassPart	rect_class;
	MwBaseMEClassPart		baseME_class;
	MwLabelMEClassPart	labelME_class;
	MwCheckMEClassPart	checkME_class;
} MwCheckMEClassRec;

extern	MwCheckMEClassRec	mwCheckMEClassRec;

typedef struct {
	/* resources */
	int		type;
	Boolean		on;
	XtCallbackList	set;
	XtCallbackList	reset;
	XtCallbackList	switchcb;
	/* private */
	Icon		*i_on , *i_off;
} MwCheckMEPart;

typedef struct _MwCheckMERec {
	ObjectPart	object;
	RectObjPart	rectangle;
	MwBaseMEPart	baseME;
	MwLabelMEPart	labelME;
	MwCheckMEPart	checkME;
} MwCheckMERec;

#endif
