/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwMenu_H_
#define _MwMenu_H_

#include	<X11/Shell.h>
#include	"MwNws.h"


#ifndef XtNcursor
#define XtNcursor	"cursor"
#endif
#ifndef XtCCursor
#define XtCCursor	"Cursor"
#endif

#ifndef XtNbox_type
#define XtNbox_type	"box_type"
#endif
#ifndef XtCBox_type
#define XtCBox_type	"Box_type"
#endif

#ifndef XtNbacking_store
#define XtNbacking_store	"backing_store"
#endif
#ifndef XtCBacking_store
#define XtCBacking_store	"Backing_store"
#endif

#ifndef XtNbox_width
#define XtNbox_width	"box_width"
#endif
#ifndef XtCBox_width
#define XtCBox_width	"Box_width"
#endif

#ifndef XtNbox_intensity
#define XtNbox_intensity	"box_intensity"
#endif
#ifndef XtCBox_intensity
#define XtCBox_intensity	"Box_intensity"
#endif

#ifndef XtNbox_color
#define XtNbox_color	"box_color"
#endif
#ifndef XtCBox_color
#define XtCBox_color	"Box_color"
#endif

#ifndef XtNtopShadowContrast
#define XtNtopShadowContrast "topShadowContrast"
#define XtCTopShadowContrast "TopShadowContrast"
#define XtNbottomShadowContrast "bottomShadowContrast"
#define XtCBottomShadowContrast "BottomShadowContrast"
#endif

extern WidgetClass mwMenuWidgetClass;

typedef struct _MwMenuClassRec	*MwMenuWidgetClass;
typedef struct _MwMenuRec		*MwMenuWidget;

extern void MwMenuDestroyChildren(Widget);

#endif
