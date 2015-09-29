/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwLineMEP_H_
#define _MwLineMEP_H_

#include	<Mowitz/MwBaseMEP.h>
#include	<Mowitz/MwLineME.h>

typedef struct _MwLineMEClassPart {
	char	empty;
} MwLineMEClassPart;

typedef struct _MwLineMEClassRec {
	RectObjClassPart	rect_class;
	MwBaseMEClassPart		baseME_class;
	MwLineMEClassPart		lineME_class;
} MwLineMEClassRec;

extern MwLineMEClassRec	mwLineMEClassRec;

typedef struct {
	/* resources */
	/* private */
	GC	gc;
} MwLineMEPart;

typedef struct _MwLineMERec {
	ObjectPart	object;
	RectObjPart	rectangle;
	MwBaseMEPart	baseME;
        MwLineMEPart	lineME;
} MwLineMERec;

#endif
