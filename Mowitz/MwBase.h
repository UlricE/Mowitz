/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwBase_H_
#define _MwBase_H_

#include <X11/Core.h>
#include "MwNws.h"

#ifndef XtNuser_data
#define XtNuser_data	"user_data"
#endif
#ifndef XtCUser_data
#define XtCUser_data	"User_data"
#endif

#ifndef XtNwid
#define XtNwid		"wid"
#endif
#ifndef XtCWid
#define XtCWid		"Wid"
#endif

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
#define XtNbd_width	"bd_width"
#endif
#ifndef XtCBd_width
#define XtCBd_width	"Bd_width"
#endif

#ifndef XtNbd_color
#define XtNbd_color	"bd_color"
#endif
#ifndef XtCBd_color
#define XtCBd_color	"Bd_color"
#endif

#ifndef XtNtopShadowContrast
#define XtNtopShadowContrast "topShadowContrast"
#define XtCTopShadowContrast "TopShadowContrast"
#define XtNbottomShadowContrast "bottomShadowContrast"
#define XtCBottomShadowContrast "BottomShadowContrast"
#endif

extern WidgetClass baseWidgetClass;

typedef struct _MwBaseClassRec	*MwBaseWidgetClass;
typedef struct _MwBaseRec		*MwBaseWidget;

#endif
