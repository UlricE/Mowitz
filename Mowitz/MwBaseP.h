/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwBaseP_H_
#define _MwBaseP_H_


#include <X11/Core.h>
#include "MwBase.h"
#include "MwNws.h"

#if NeedFunctionPrototypes
typedef void (*highlight_Proc) (Widget , XEvent * , String * , Cardinal *);
typedef void (*unhighlight_Proc) (Widget , XEvent * , String * , Cardinal *);
#else
typedef void (*highlight_Proc) ();
typedef void (*unhighlight_Proc) ();
#endif

#define  XtInheritHighlight ((highlight_Proc) _XtInherit)
#define  XtInheritUnhighlight ((unhighlight_Proc) _XtInherit)


typedef struct {
	get_internal_dimension_Proc	get_internal_dimension;
	set_internal_dimension_Proc	set_internal_dimension;
	highlight_Proc			highlight;
	unhighlight_Proc		unhighlight;
	highlightBorder_Proc		highlightBorder;
	unhighlightBorder_Proc		unhighlightBorder;
} MwBaseClassPart;

typedef struct _MwBaseClassRec {
	CoreClassPart		core_class;
	MwBaseClassPart		base_class;
} MwBaseClassRec;

extern MwBaseClassRec mwBaseClassRec;

typedef struct _MwBasePart {
	/* public resources */
	void		*user_data;
	int		wid;
	Boolean		highlight_on_enter;
	int		help_show_delay;
	Cursor		cursor;
	int		box_type;
	int		box_width;
	int		box_intensity;
	Pixel		box_color;
	Pixel		activ_fg;
	Pixel		activ_bg;
	String		help_text;
	Pixel		foreground;
	int		bd_width;
	Pixel		bd_color;
	int		top_shadow_contrast;
	int		bot_shadow_contrast;
	/* private */
	XtIntervalId	timer;
	Widget		hlp;
	GC		gc;
	Pixel		dark,light;
	Boolean		have_shape;
	Boolean		entered;
	Boolean		focused;
} MwBasePart;

typedef struct _MwBaseRec {
	CorePart	core;
	MwBasePart	base;
} MwBaseRec;

#endif
