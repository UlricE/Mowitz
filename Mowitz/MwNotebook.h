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

#ifndef s_MwNotebook_h
#define s_MwNotebook_h

/****************************************************************
 *
 * MwNotebook widget
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

#define XtNnotebookSelected "notebookSelected"
#define XtCNotebookSelected "NotebookSelected"
#define XtNnotebookSelect "notebookSelect"
#define XtCNotebookSelect "NotebookSelect"
#define XtNnotebookTop "notebookTop"
#define XtCNotebookTop "NotebookTop"
#define XtNnotebookCount "notebookCount"
#define XtCNotebookCount "NotebookCount"

/* declare specific MwNotebookWidget class and instance datatypes */

typedef struct s_MwNotebookClassRec*	MwNotebookWidgetClass;
typedef struct s_MwNotebookRec*		MwNotebookWidget;

/* declare the class constant */

extern WidgetClass mwNotebookWidgetClass;

extern int MwNotebookInsert(Widget, char *, int);
extern void MwNotebookRemove(Widget, int);
extern int MwNotebookTextToPos(Widget, char *);
extern char *MwNotebookPosToText(Widget, int);

#endif /* s_MwNotebook_h */

