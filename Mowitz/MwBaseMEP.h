/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwBaseMEP_H_
#define _MwBaseMEP_H_

#include <X11/RectObjP.h>

#include <Mowitz/MwBaseME.h>
#include <Mowitz/MwNws.h>

#if NeedFunctionPrototypes
typedef void (*enternotify_Proc) (Widget);
typedef void (*leavenotify_Proc) (Widget);
typedef void (*activate_Proc) (Widget);
#else
typedef void (*enternotify_Proc) ();
typedef void (*leavenotify_Proc) ();
typedef void (*activate_Proc) ();
#endif


#define  XtInheritEnternotify ((enternotify_Proc) _XtInherit)
#define  XtInheritLeavenotify ((leavenotify_Proc) _XtInherit)
#define  XtInheritActivate ((activate_Proc) _XtInherit)


typedef struct _MwBaseMEClassPart {
        get_internal_dimension_Proc     get_internal_dimension;
        set_internal_dimension_Proc     set_internal_dimension;
	enternotify_Proc		enternotify;
	leavenotify_Proc		leavenotify;
	activate_Proc			activate;
} MwBaseMEClassPart;

typedef struct _MwBaseMEClassRec {
	RectObjClassPart	rect_class;
	MwBaseMEClassPart		baseME_class;
} MwBaseMEClassRec;

extern MwBaseMEClassRec mwBaseMEClassRec;

typedef struct {
	/* resources */
	int	wid;
	XtCallbackList  activate;
	Pixel	foreground;
	Pixel	background;
	Pixel	activ_bg;
	Pixel	activ_fg;
	Pixel	box_color;
	int	box_width;
	int	box_intensity;
	int	box_type;
	int             top_shadow_contrast;
        int             bot_shadow_contrast;
	/* private */
	Pixel	light,dark;
	GC	gc;
	Boolean	entered;
	void	*related_data;
} MwBaseMEPart;

typedef struct _MwBaseMERec {
	ObjectPart		object;
	RectObjPart		rectangle;
	MwBaseMEPart		baseME;
} MwBaseMERec;

#endif
