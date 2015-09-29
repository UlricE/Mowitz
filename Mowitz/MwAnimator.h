/*
   Copyright (C) 1996-2002  Ulric Eriksson <ulric@siag.nu>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the Licence, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free
   Foundation, Inc., 59 Temple Place - Suite 330, Boston,
   MA 02111-1307, USA.
 */

#ifndef MW_ANIMATOR_H
#define MW_ANIMATOR_H

/****************************************************************
 *
 * Animator widget
 *
 ****************************************************************/

/* Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 animatorBgPixmap    AnimatorBgPixmap	Pixmap		None
 animatorCast	     AnimatorCast			NULL
 animatorNow	     AnimatorNow			0
 animatorDelta	     AnimatorDelta			100
 animatorDuration    AnimatorDuration			1000
 animatorMode	     AnimatorMode			ANI_STOP
 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 destroyCallback     Callback		Pointer		NULL
 height		     Height		Dimension	0
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 sensitive	     Sensitive		Boolean		True
 width		     Width		Dimension	0
 x		     Position		Position	0
 y		     Position		Position	0

*/

/* define any special resource names here that are not in <X11/StringDefs.h> */

#define XtNanimatorCast "animatorCast"
#define XtNanimatorNow "animatorNow"
#define XtNanimatorDelta "animatorDelta"
#define XtNanimatorDuration "animatorDuration"
#define XtNanimatorMode "animatorMode"
#define XtNanimatorBgPixmap "animatorBgPixmap"
#define XtNanimatorPluginCoords "animatorPluginCoords"
#define XtNgradient "gradient"

#define XtCAnimatorCast "AnimatorCast"
#define XtCAnimatorNow "AnimatorNow"
#define XtCAnimatorDelta "AnimatorDelta"
#define XtCAnimatorDuration "AnimatorDuration"
#define XtCAnimatorMode "AnimatorMode"
#define XtCAnimatorBgPixmap "AnimatorBgPixmap"
#define XtCAnimatorPluginCoords "AnimatorPluginCoords"
#define XtCGradient "Gradient"

#include "MwAnimatorTypes.h"

/* declare specific AnimatorWidget class and instance datatypes */

typedef struct s_AnimatorClassRec*	MwAnimatorWidgetClass;
typedef struct s_AnimatorRec*		MwAnimatorWidget;

/* declare the class constant */

extern WidgetClass mwAnimatorWidgetClass;


extern Pixmap MwAnimatorPixmap(Widget, int);

#endif /* MW_ANIMATOR_H */

