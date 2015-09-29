/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwLabelME_H_
#define _MwLabelME_H_

#include	"MwBaseME.h"
#include	"MwNws.h"


#ifndef XtNleft_icon
#define XtNleft_icon		"left_icon"
#endif
#ifndef XtCLeft_icon
#define XtCLeft_icon		"Left_icon"
#endif

#ifndef XtNright_icon
#define XtNright_icon		"right_icon"
#endif
#ifndef XtCRight_icon
#define XtCRight_icon		"Right_icon"
#endif

#ifndef XtNlabel
#define XtNlabel		"label"
#endif
#ifndef XtCLabel
#define XtCLabel		"Label"
#endif

#ifndef XtNlabel_justify
#define XtNlabel_justify	"label_justify"
#endif
#ifndef XtCLabel_justify
#define XtCLabel_justify	"Label_justify"
#endif

#ifndef XtNfont
#define XtNfont			"font"
#endif
#ifndef XtCFont
#define XtCFont			"Font"
#endif

#ifndef XtNspacing
#define XtNspacing		"spacing"
#endif
#ifndef XtCSpacing
#define XtCSpacing		"Spacing"
#endif

#ifndef XtNlabel_position
#define XtNlabel_position	"label_position"
#endif
#ifndef XtCLabel_position
#define XtCLabel_position	"Label_position"
#endif

typedef struct _MwLabelMEClassRec		*MwLabelMEObjectClass;
typedef	struct _MwLabelMERec		*MwLabelMEObject;

extern WidgetClass	mwLabelMEObjectClass;

#endif
