/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwRow_H_
#define _MwRow_H_

#include	"MwBaseConst.h"

#ifndef XtNhomogenous
#define XtNhomogenous		"homogenous"
#endif
#ifndef XtCHomogenous
#define XtCHomogenous		"Homogenous"
#endif

#ifndef XtNgravitation
#define XtNgravitation		"gravitation"
#endif
#ifndef	XtCGravitation
#define XtCGravitation		"Gravitation"
#endif

#ifndef XtNleft_space
#define XtNleft_space		"left_space"
#endif
#ifndef XtCLeft_space
#define XtCLeft_space		"Left_space"
#endif

#ifndef XtNright_space
#define XtNright_space		"right_space"
#endif
#ifndef XtCRight_space
#define XtCRight_space		"Right_space"
#endif

#ifndef XtNspacing
#define XtNspacing		"spacing"
#endif
#ifndef XtCSpacing
#define XtCSpacing		"Spacing"
#endif

#ifndef XtNresizable
#define XtNresizable		"resizable"
#endif
#ifndef XtCResizable
#define XtCResizable		"Resizable"
#endif

typedef struct _MwRowClassRec	*MwRowWidgetClass;
typedef struct _MwRowRec		*MwRowWidget;

extern WidgetClass rowWidgetClass;

#endif
