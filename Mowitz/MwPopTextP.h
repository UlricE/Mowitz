/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwPopTextP_H_
#define _MwPopTextP_H_

#include <X11/ShellP.h>
#include "MwPopText.h"

typedef struct {
	int	dummy;
} MwPopTextClassPart;

typedef struct _MwPopTextClassRec {
	CoreClassPart		core_class;
	CompositeClassPart	composite_class;
	ShellClassPart		shell_class;
	OverrideShellClassPart	override_shell_class;
	MwPopTextClassPart	popText_class;
} MwPopTextClassRec;

extern MwPopTextClassRec mwPopTextClassRec;

typedef struct _MwPopTextPart {
	/* public resources */
	char		* text;
	Pixel		foreground;
	XFontStruct	* font;
	int		spacing;
	/* private */
	GC		gc;
} MwPopTextPart;

typedef struct _MwPopTextRec {
	CorePart		core;
	CompositePart		composite;
	ShellPart		shell;
	OverrideShellPart	override;
	MwPopTextPart		popText;
} MwPopTextRec;

#endif
