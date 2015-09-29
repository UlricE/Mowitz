/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _MwMenuButton_H_
#define _MwMenuButton_H_

#include	"MwButton.h"

#ifndef XtNmenu_name
#define XtNmenu_name		"menu_name"
#endif
#ifndef XtCMenu_name
#define XtCMenu_name		"Menu_name"
#endif

typedef struct _MwMenuButtonClassRec	*MwMenuButtonWidgetClass;
typedef struct _MwMenuButtonRec		*MwMenuButtonWidget;

extern WidgetClass mwMenuButtonWidgetClass;

#endif
