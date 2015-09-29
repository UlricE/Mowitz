/*
Copyright (C) 2001  Ulric Eriksson <ulric@siag.nu>

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

#ifndef _MwSpinner_h
#define _MwSpinner_h

#include <X11/Xmu/Converters.h>
#include "MwFrame.h"

/***********************************************************************
 *
 * MwSpinner Widget (subclass of FrameClass)
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

#ifndef XtNmin
#define XtNmin "min"
#define XtCMin "Min"
#endif
#ifndef XtNmax
#define XtNmax "max"
#define XtCMax "Max"
#endif
#ifndef XtNstep
#define XtNstep "step"
#define XtCStep "Step"
#endif
#ifndef XtNvalue
#define XtNvalue "value"
#define XtCValue "Value"
#endif

/* Class record constants */

extern WidgetClass mwSpinnerWidgetClass;

typedef struct _MwSpinnerClassRec *MwSpinnerWidgetClass;
typedef struct _MwSpinnerRec      *MwSpinnerWidget;

#endif /* _MwSpinner_h */
