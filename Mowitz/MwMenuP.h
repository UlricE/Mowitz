/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwMenuP_H_
#define _MwMenuP_H_

#include	<X11/ShellP.h>
#include	"MwMenu.h"
#include	"MwNws.h"

#if NeedFunctionPrototypes
typedef void (*popdown_submenu_Proc) (Widget);
typedef void (*popdown_all_Proc) (Widget);
typedef void (*insert_submenu_Proc) (Widget);
#else
typedef void (*popdown_submenu_Proc) ();
typedef void (*popdown_all_Proc) ();
typedef void (*insert_submenu_Proc) ();
#endif

#define  XtInheritGetInternalDimension ((get_internal_dimension_Proc) _XtInherit)
#define  XtInheritSetInternalDimension ((set_internal_dimension_Proc) _XtInherit)
#define  XtInheritPopdownSubmenu ((popdown_submenu_Proc) _XtInherit)
#define  XtInheritPopdownAll ((popdown_all_Proc) _XtInherit)

#define HIDE_PROTO 1
#include	"MwBaseMEP.h"
#undef HIDE_PROTO

typedef struct {
	get_internal_dimension_Proc	get_internal_dimension;
	set_internal_dimension_Proc	set_internal_dimension;
	int				num_submenu;
	WidgetList			submenu;
	insert_submenu_Proc		insert_submenu;
	popdown_submenu_Proc		popdown_submenu;
	popdown_all_Proc		popdown_all;
} MwMenuClassPart;

typedef struct _MwMenuClassRec {
	CoreClassPart		core_class;
	CompositeClassPart	composite_class;
	ShellClassPart		shell_class;
	OverrideShellClassPart	override_shell_class;
	MwMenuClassPart		menu_class;
} MwMenuClassRec;

extern MwMenuClassRec mwMenuClassRec;

typedef struct _MwMenuPart {
	/* public resources */
	Cursor		cursor;
	int		box_type;
	int		box_width;
	int		box_intensity;
	Pixel		box_color;
	int		backing_store;
	int             top_shadow_contrast;
        int             bot_shadow_contrast;
	/* private */
	int		menu_sub_position;	/* kolkate je to submenu */
	Pixel		dark,light;
	Boolean		down_after_br;
	XtIntervalId    timer;
	MwBaseMEObject	*last_selected;
} MwMenuPart;

typedef struct _MwMenuRec {
	CorePart		core;
	CompositePart		composite;
	ShellPart		shell;
  	OverrideShellPart	override;
	MwMenuPart		menu;
} MwMenuRec;

#endif
