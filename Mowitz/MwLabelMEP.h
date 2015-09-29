/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwLabelMEP_H_
#define _MwLabelMEP_H_

#include	<Mowitz/MwBaseMEP.h>
#include	<Mowitz/MwLabelME.h>


typedef struct _MwLabelMEClassPart {
	char	empty;
} MwLabelMEClassPart;

typedef struct _MwLabelMEClassRec {
	RectObjClassPart	rect_class;
	MwBaseMEClassPart		baseME_class;
	MwLabelMEClassPart	labelME_class;
} MwLabelMEClassRec;

extern MwLabelMEClassRec mwLabelMEClassRec;

typedef struct {
	/* resources */
	char		*label;
	XFontStruct	*font;
	Icon		*left_icon;
	Icon		*right_icon;
	int		label_justify;
	int		spacing;
	int		label_position;
	/* private */
	GC	gc;
} MwLabelMEPart;

typedef struct _MwLabelMERec {
	ObjectPart		object;
	RectObjPart		rectangle;
	MwBaseMEPart		baseME;
	MwLabelMEPart		labelME;
} MwLabelMERec;

#endif
