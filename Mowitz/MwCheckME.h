/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwCheckME_H_
#define _MwCheckME_H_

#include	"MwLabelME.h"

#ifndef	XtNon
#define	XtNon	"on"
#endif
#ifndef XtCOn
#define XtCOn	"On"
#endif

#ifndef XtNset
#define XtNset	"set"
#endif
#ifndef	XtCSet
#define XtCSet	"Set"
#endif

#ifndef XtNreset
#define XtNreset	"reset"
#endif
#ifndef	XtCReset
#define XtCReset	"Reset"
#endif

#ifndef XtNswitch
#define XtNswitch       "switch"
#endif
#ifndef XtCSwitch
#define XtCSwitch       "Switch"
#endif

#ifndef	XtNtype	
#define	XtNtype		"type"
#endif
#ifndef XtCType
#define	XtCType		"Type"
#endif

typedef struct	_MwCheckMEClassRec	*MwCheckMEObjectClass;
typedef struct	_MwCheckMERec		*MwCheckMEObject;

extern WidgetClass	checkMEObjectClass;

#endif
