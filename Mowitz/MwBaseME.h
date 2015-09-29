/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwBase_H_
#define _MwBase_H_

#include <X11/RectObj.h>
#include <Mowitz/MwNws.h>

#ifndef XtNwid
#define XtNwid		"wid"
#endif
#ifndef XtCWid
#define XtCWid		"Wid"
#endif

#ifndef XtNbox_type
#define XtNbox_type	"box_type"
#endif
#ifndef XtCBox_type
#define XtCBox_type	"Box_type"
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

#ifndef XtNforeground
#define XtNforeground   "foreground"
#endif
#ifndef XtCForeground
#define XtCForeground   "Foreground"
#endif

#ifndef XtNbackground
#define XtNbackground   "background"
#endif
#ifndef XtCBackground
#define XtCBackground   "Background"
#endif

#ifndef XtNactiv_fg
#define XtNactiv_fg	"activ_fg"
#endif
#ifndef XtCActiv_fg
#define XtCActiv_fg	"Activ_fg"
#endif

#ifndef XtNactiv_bg
#define XtNactiv_bg	"activ_bg"
#endif
#ifndef XtCActiv_bg
#define XtCActiv_bg	"Activ_bg"
#endif

#ifndef XtNtopShadowContrast
#define XtNtopShadowContrast "topShadowContrast"
#define XtCTopShadowContrast "TopShadowContrast"
#define XtNbottomShadowContrast "bottomShadowContrast"
#define XtCBottomShadowContrast "BottomShadowContrast"
#endif

extern WidgetClass mwBaseMEObjectClass;

typedef struct _MwBaseMEClassRec	*MwBaseMEObjectClass;
typedef struct _MwBaseMERec	*MwBaseMEObject;

#endif
