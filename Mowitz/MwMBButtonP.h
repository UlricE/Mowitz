/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwMBButtonP_H_
#define _MwMBButtonP_H_

#include	"MwLabelMEP.h"
#include	"MwMBButton.h"

#if NeedFunctionPrototypes
typedef	void (*popdown_menu_Proc) (Widget);
#else
typedef void (*popdown_menu_Proc) ();
#endif

#define XtInheritPopdownMenu ((popdown_menu_Proc) _XtInherit)

typedef struct {
	popdown_menu_Proc	popdown_menu;	
} MwMBButtonClassPart;

typedef struct _MwMBButtonClassRec {
	RectObjClassPart        rect_class;
	MwBaseMEClassPart         baseME_class;
	MwLabelMEClassPart        labelME_class;
	MwMBButtonClassPart	mBButton_class;
} MwMBButtonClassRec;

extern MwMBButtonClassRec mwMBButtonClassRec;

typedef struct {
	/* resources */
	String		menu_name;
	/* private */
	Widget		menu;
} MwMBButtonPart;

typedef struct _MwMBButtonRec {
	ObjectPart              object;
	RectObjPart             rectangle;
	MwBaseMEPart              baseME;
	MwLabelMEPart             labelME;
	MwMBButtonPart		mBButton;
} MwMBButtonRec;

#endif
