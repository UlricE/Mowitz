/*
Copyright (C) 1999-2001  Ulric Eriksson <ulric@siag.nu>

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

#ifndef s_MwCheck_h
#define s_MwCheck_h

/****************************************************************
 *
 * MwCheck widget
 *
 ****************************************************************/

/* Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
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

typedef enum {
	MwCheckWin,
	MwRadioWin,
	MwCheckMotif,
	MwRadioMotif
} MwCheckStyle;

#ifndef XtNradioStart
#define XtNradioStart "radioStart"
#define XtCRadioStart "RadioStart"
#endif
#ifndef XtNradioGroup
#define XtNradioGroup "radioGroup"
#define XtCRadioGroup "RadioGroup"
#endif
#ifndef XtNcheckStyle
#define XtNcheckStyle "checkStyle"
#define XtCCheckStyle "CheckStyle"
#endif
#ifndef XtNstate
#define XtNstate "state"
#define XtCState "State"
#endif

/* declare specific MwCheckWidget class and instance datatypes */

typedef struct s_MwCheckClassRec*	MwCheckWidgetClass;
typedef struct s_MwCheckRec*		MwCheckWidget;

/* declare the class constant */

extern WidgetClass mwCheckWidgetClass;

#endif /* s_MwCheck_h */

