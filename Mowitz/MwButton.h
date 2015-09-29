/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwButton_H
#define _MwButton_H

#include	"MwSButton.h"
#include	"MwNws.h"

#ifndef XtNicon
#define XtNicon		"icon"
#endif
#ifndef XtCIcon
#define XtCIcon		"Icon"
#endif

#ifndef XtNlabel
#define XtNlabel	"label"
#endif
#ifndef XtCLabel
#define XtCLabel	"Label"
#endif

#ifndef XtNicon_justify
#define XtNicon_justify		"icon_justify"
#endif
#ifndef XtCIcon_justify
#define XtCIcon_justify		"Icon_justify"
#endif

#ifndef XtNlabel_justify
#define XtNlabel_justify	"label_justify"
#endif
#ifndef XtCLabel_justify
#define XtCLabel_justify	"Label_justify"
#endif

#ifndef XtNfont
#define XtNfont		"font"
#endif
#ifndef XtCFont
#define XtCFont		"Font"
#endif

#ifndef XtNspacing
#define XtNspacing	"spacing"
#endif
#ifndef XtCSpacing
#define XtCSpacing	"Spacing"
#endif

typedef struct _MwButtonClassRec *MwButtonWidgetClass;
typedef struct _MwButtonRec *MwButtonWidget;

extern WidgetClass mwButtonWidgetClass;

#endif
