/*
Copyright (C) 1998-2001  Ulric Eriksson <ulric@siag.nu>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the Licence, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA.
*/

#ifndef _MwCombo_h
#define _MwCombo_h

#include "MwCombo.h"
#include <X11/Xmu/Converters.h>

/***********************************************************************
 *
 * MwCombo Widget (subclass of CompositeClass)
 *
 ***********************************************************************/

/* Parameters:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 destroyCallback     Callback		Pointer		NULL
 hSpace 	     HSpace		Dimension	4
 height		     Height		Dimension	0
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 orientation	     Orientation	XtOrientation	vertical
 vSpace 	     VSpace		Dimension	4
 width		     Width		Dimension	0
 x		     Position		Position	0
 y		     Position		Position	0

*/


/* New fields */
#ifndef XtNtextCallback
#define XtNtextCallback "textCallback"
#define XtCTextCallback "TextCallback"
#endif
#ifndef XtNlistCallback
#define XtNlistCallback "listCallback"
#define XtCListCallback "ListCallback"
#endif

#define XtNcomboTop "comboTop"
#define XtCComboTop "ComboTop"
#define XtNcomboData "comboData"
#define XtCComboData "ComboData"
#define XtNcomboNData "comboNData"
#define XtCComboNData "ComboNData"

/* Class record constants */

extern WidgetClass mwComboWidgetClass;

typedef struct _MwComboClassRec *MwComboWidgetClass;
typedef struct _MwComboRec      *MwComboWidget;

extern void MwComboTextChange(Widget, char *);

#endif /* _MwCombo_h */
