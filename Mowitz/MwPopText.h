/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwPopText_H_
#define _MwPopText_H_

#include <X11/Shell.h>

#ifndef XtNforeground
#define XtNforeground   "foreground"
#endif
#ifndef XtCForeground
#define XtCForeground   "Foreground"
#endif

#ifndef XtNfont
#define XtNfont		"font"
#endif
#ifndef XtCFont
#define XtCFont		"Font"
#endif

#ifndef XtNtext
#define XtNtext		"text"
#endif
#ifndef XtCText
#define XtCText		"Text"
#endif

#ifndef XtNspacing
#define XtNspacing      "spacing"
#endif
#ifndef XtCSpacing
#define XtCSpacing      "Spacing"
#endif


extern WidgetClass	mwPopTextWidgetClass;

typedef struct _MwPopTextClassRec	*MwPopTextWidgetClass;
typedef struct _MwPopTextRec	*MwPopTextWidget;

#endif
