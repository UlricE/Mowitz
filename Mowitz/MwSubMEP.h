/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwSubMEP_H_
#define _MwSubMEP_H_

#include	"MwLabelMEP.h"
#include	"MwSubME.h"
#include	"MwNws.h"

typedef struct {
	char	empty;
} MwSubMEClassPart;

typedef struct _MwSubMEClassRec {
	RectObjClassPart	rect_class;
	MwBaseMEClassPart		baseME_class;
	MwLabelMEClassPart	labelME_class;
	MwSubMEClassPart		subME_class;
} MwSubMEClassRec;

extern MwSubMEClassRec mwSubMEClassRec;

typedef struct {
	/* resources */
	String		menu_name;
	int		delay_to_show_submenu;
	/* private */
	Boolean		is_menu_up;
	Icon		*i_on , *i_off;
	XtIntervalId	timer;
} MwSubMEPart;

typedef struct _MwSubMERec {
	ObjectPart	object;
	RectObjPart	rectangle;
	MwBaseMEPart	baseME;
	MwLabelMEPart	labelME;
	MwSubMEPart	subME;
} MwSubMERec;

#endif
