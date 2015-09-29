/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwSButton_H
#define _MwSButton_H

#include	"MwBase.h"
#include	"MwNws.h"

#ifndef XtNon
#define XtNon   "on"
#endif
#ifndef XtCOn
#define XtCOn   "On"
#endif

#ifndef XtNinitDelay
#define XtNinitDelay	"initDelay"
#endif
#ifndef XtCInitDelay
#define XtCInitDelay	"InitDelay"
#endif
#ifndef XtRCardinal
#define XtRCardinal	"Cardinal"
#endif

#ifndef XtNrepeatDelay
#define XtNrepeatDelay	"repeatDelay"
#endif
#ifndef XtCRepeatDelay
#define XtCRepeatDelay	"RepeatDelay"
#endif

#ifndef XtNbuttonMode
#define XtNbuttonMode	"buttonMode"
#endif
#ifndef XtCButtonMode
#define XtCButtonMode	"ButtonMode"
#endif

#ifndef XtNswitch
#define XtNswitch       "switch"
#endif
#ifndef XtCSwitch
#define XtCSwitch       "Switch"
#endif

typedef struct _MwSButtonClassRec *MwSButtonWidgetClass;
typedef struct _MwSButtonRec *MwSButtonWidget;

extern WidgetClass mwSButtonWidgetClass;

#endif
