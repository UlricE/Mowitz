/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwSubME_H_
#define _MwSubME_H_

#include	"MwLabelME.h"


#ifndef XtNmenu_name
#define XtNmenu_name		"menu_name"
#endif
#ifndef XtCMenu_name
#define XtCMenu_name		"Menu_name"
#endif

#ifndef XtNdelay_to_show_submenu
#define XtNdelay_to_show_submenu	"delay_to_show_submenu"
#endif
#ifndef XtCDelay_to_show_submenu
#define XtCDelay_to_show_submenu	"Delay_to_show_submenu"
#endif

typedef struct _MwSubMEClassRec      *MwSubMEObjectClass;
typedef struct _MwSubMERec           *MwSubMEObject;

extern WidgetClass mwSubMEObjectClass;

#endif
