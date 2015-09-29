/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwBaseConst_H_
#define _MwBaseConst_H_

#include <X11/Constraint.h>
#include "MwNws.h"

#ifndef XtNcursor
#define XtNcursor	"cursor"
#endif
#ifndef XtCCursor
#define XtCCursor	"Cursor"
#endif

#ifndef XtNtraverse
#define XtNtraverse	"traverse"
#endif
#ifndef XtCTraverse
#define XtCTraverse	"Traverse"
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

#ifndef XtNfont
#define XtNfont		"font"
#endif
#ifndef XtCFont
#define XtCFont		"Font"
#endif

#ifndef XtNlabel
#define XtNlabel	"label"
#endif
#ifndef XtCLabel
#define XtCLabel	"Label"
#endif

#ifndef XtNjustify
#define XtNjustify	"justify"
#endif
#ifndef XtCJustify
#define XtCJustify	"Justify"
#endif

#ifndef XtNhelp_text
#define XtNhelp_text	"help_text"
#endif
#ifndef XtCHelp_text
#define XtCHelp_text	"Help_text"
#endif

#ifndef XtNforeground
#define XtNforeground   "foreground"
#endif
#ifndef XtCForeground
#define XtCForeground   "Foreground"
#endif

#ifndef XtNactiv_color
#define XtNactiv_color	"activ_color"
#endif
#ifndef XtCActiv_color
#define XtCActiv_color	"Activ_color"
#endif

#ifndef XtNhighlight_on_enter
#define XtNhighlight_on_enter	"highlight_on_enter"
#endif
#ifndef XtCHighlight_on_enter
#define XtCHighlight_on_enter	"Highlight_on_enter"
#endif

#ifndef XtNhelp_show_delay
#define XtNhelp_show_delay	"help_show_delay"
#endif
#ifndef XtCHelp_show_delay
#define XtCHelp_show_delay	"Help_show_delay"
#endif

#ifndef XtNbd_width
#define XtNbd_width     "bd_width"
#endif
#ifndef XtCBd_width
#define XtCBd_width     "Bd_width"
#endif

#ifndef XtNbd_color
#define XtNbd_color     "bd_color"
#endif
#ifndef XtCBd_color
#define XtCBd_color     "Bd_color"
#endif

/* constant for box type definition */
#define XtNo_box 		0
#define XtSimple_box		1
#define XtUp_box		2
#define XtDown_box		3
#define XtFramein_box		4
#define XtFrameout_box		5
#define XtShadow_box		6

#ifndef XtNtopShadowContrast
#define XtNtopShadowContrast "topShadowContrast"
#define XtCTopShadowContrast "TopShadowContrast"
#define XtNbottomShadowContrast "bottomShadowContrast"
#define XtCBottomShadowContrast "BottomShadowContrast"
#endif

extern WidgetClass mwBaseConstWidgetClass;

typedef struct _MwBaseConstClassRec	*MwBaseConstWidgetClass;
typedef struct _MwBaseConstRec		*MwBaseConstWidget;

#endif
