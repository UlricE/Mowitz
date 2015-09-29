/*
Copyright (C) 2000-2001  Ulric Eriksson <ulric@siag.nu>

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

#ifndef s_MwTabstop_h
#define s_MwTabstop_h

/****************************************************************
 *
 * MwTabstop widget
 *
 ****************************************************************/

/* Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 mwTabstopData	     MwTabstopData		Pointer		NULL
 mwTabstopExpose	     MwTabstopExpose	Pointer		NULL
 destroyCallback     Callback		Pointer		NULL
 height		     Height		Dimension	0
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 sensitive	     Sensitive		Boolean		True
 width		     Width		Dimension	0
 x		     Position		Position	0
 y		     Position		Position	0

*/

/* define any special resource names here that are not in <X11/StringDefs.h> */

#ifndef XtNleftMargin
#define XtNleftMargin "leftMargin"
#define XtCLeftMargin "LeftMargin"
#endif
#ifndef XtNrightMargin
#define XtNrightMargin "rightMargin"
#define XtCRightMargin "RightMargin"
#endif
#ifndef XtNpaperWidth
#define XtNpaperWidth "paperWidth"
#define XtCPaperWidth "PaperWidth"
#endif
#ifndef XtNtabstopTopCol
#define XtNtabstopTopCol "tabstopTopCol"
#define XtCTabstopTopCol "TabstopTopCol"
#endif
#ifndef XtNzoom
#define XtNzoom "zoom"
#define XtCZoom "Zoom"
#endif
#ifndef XtNtabs
#define XtNtabs "tabs"
#define XtCTabs "Tabs"
#endif

/* declare specific MwTabstopWidget class and instance datatypes */

typedef struct s_MwTabstopClassRec*	MwTabstopWidgetClass;
typedef struct s_MwTabstopRec*		MwTabstopWidget;

/* declare the class constant */

extern WidgetClass mwTabstopWidgetClass;

typedef struct MwTabstop {
        char j; /* l(eft), c(enter), r(ight) */
        int x;
} MwTabstop;

extern MwTabstop MwTabstopNextStop(Widget, int);
extern void MwTabstopSetZoom(Widget, float);
extern MwTabstop *MwGetTabs(char *);
extern MwTabstop MwNextTab(MwTabstop *, int);

#endif /* s_MwTabstop_h */

