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

#ifndef MW_TABBING_H
#define MW_TABBING_H

/****************************************************************
 *
 * Tabbing widget
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

#define XtNtabbingSelected "tabbingSelected"
#define XtCTabbingSelected "TabbingSelected"
#define XtNtabbingTop "tabbingTop"
#define XtCTabbingTop "TabbingTop"
#define XtNtabbingCount "tabbingCount"
#define XtCTabbingCount "TabbingCount"
#ifndef XtNselectCallback
#define XtNselectCallback "selectCallback"
#define XtCSelectCallback "SelectCallback"
#endif
#ifndef XtNrenameCallback
#define XtNrenameCallback "renameCallback"
#define XtCRenameCallback "RenameCallback"
#endif

/* declare specific TabbingWidget class and instance datatypes */

typedef struct s_TabbingClassRec*	MwTabbingWidgetClass;
typedef struct s_TabbingRec*		MwTabbingWidget;

/* declare the class constant */

extern WidgetClass mwTabbingWidgetClass;

extern int MwTabbingInsert(Widget, char *, int);
extern void MwTabbingRemove(Widget, int);
extern int MwTabbingTextToPos(Widget, char *);
extern char *MwTabbingPosToText(Widget, int);

#endif /* MW_TABBING_H */

