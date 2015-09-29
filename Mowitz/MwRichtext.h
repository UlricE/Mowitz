/*
   Copyright (C) 1998-2002  Ulric Eriksson <ulric@siag.nu>

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

#ifndef MW_RICHTEXT_H
#define MW_RICHTEXT_H

/****************************************************************
 *
 * Richtext widget
 *
 ****************************************************************/

/* Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 richtextData	     RichtextData		Pointer		NULL
 richtextExpose	     RichtextExpose	Pointer		NULL
 destroyCallback     Callback		Pointer		NULL
 height		     Height		Dimension	0
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 sensitive	     Sensitive		Boolean		True
 width		     Width		Dimension	0
 x		     Position		Position	0
 y		     Position		Position	0
 zoom		     Float		Float		1.0

*/

/* define any special resource names here that are not in <X11/StringDefs.h> */

#define XtNrichtextTopRow "richtextTopRow"
#define XtNrichtextTopCol "richtextTopCol"
#define XtNrichtextSelectTopRow "richtextSelectTopRow"
#define XtNrichtextSelectTopCol "richtextSelectTopCol"
#define XtNrichtextSelectBottomRow "richtextSelectBottomRow"
#define XtNrichtextSelectBottomCol "richtextSelectBottomCol"
#define XtNrichtextPointRow "richtextPointRow"
#define XtNrichtextPointCol "richtextPointCol"
#define XtNrichtextRowHeight "richtextRowHeight"
#define XtNrichtextAdjHoriz "richtextAdjHoriz"
#define XtNrichtextStyle "richtextStyle"
#define XtNrichtextText "richtextText"
#define XtNrichtextData "richtextData"
#define XtNrichtextRedisplay "richtextRedisplay"
#define XtNrichtextVisibleCursor "richtextVisibleCursor"
#define XtNrichtextPaperWidth "richtextPaperWidth"
#define XtNrichtextLeftMargin "richtextLeftMargin"
#define XtNrichtextRightMargin "richtextRightMargin"
#define XtNrichtextPluginCoords "richtextPluginCoords"
#define XtNrichtextBop "richtextBop"
#define XtNrichtextFormat "richtextFormat"
#define XtNrichtextString "richtextString"
#define XtNrichtextDelay "richtextDelay"
#define XtNrichtextRuler "richtextRuler"
#define XtNzoom "zoom"

#define XtCRichtextTopRow "RichtextTopRow"
#define XtCRichtextTopCol "RichtextTopCol"
#define XtCRichtextSelectTopRow "RichtextSelectTopRow"
#define XtCRichtextSelectTopCol "RichtextSelectTopCol"
#define XtCRichtextSelectBottomRow "RichtextSelectBottomRow"
#define XtCRichtextSelectBottomCol "RichtextSelectBottomCol"
#define XtCRichtextPointRow "RichtextPointRow"
#define XtCRichtextPointCol "RichtextPointCol"
#define XtCRichtextRowHeight "RichtextRowHeight"
#define XtCRichtextAdjHoriz "RichtextAdjHoriz"
#define XtCRichtextStyle "RichtextStyle"
#define XtCRichtextText "RichtextText"
#define XtCRichtextData "RichtextData"
#define XtCRichtextRedisplay "RichtextRedisplay"
#define XtCRichtextVisibleCursor "RichtextVisibleCursor"
#define XtCRichtextPaperWidth "RichtextPaperWidth"
#define XtCRichtextLeftMargin "RichtextLeftMargin"
#define XtCRichtextRightMargin "RichtextRightMargin"
#define XtCRichtextPluginCoords "RichtextPluginCoords"
#define XtCRichtextBop "RichtextBop"
#define XtCRichtextFormat "RichtextFormat"
#define XtCRichtextString "RichtextString"
#define XtCRichtextDelay "RichtextDelay"
#define XtCRichtextRuler "RichtextRuler"
#define XtCZoom "Zoom"

/* declare specific RichtextWidget class and instance datatypes */

typedef struct s_RichtextClassRec*	MwRichtextWidgetClass;
typedef struct s_RichtextRec*		MwRichtextWidget;

/* declare the class constant */

extern WidgetClass mwRichtextWidgetClass;

/* a few functions */
extern void MwRichtextDrawLine(Widget, int);
extern void MwRichtextCharToCoords(MwRichtextWidget, int, int, int *, int *);
extern void MwRichtextCoordsToChar(MwRichtextWidget, int *, int *, int, int);
extern int MwRichtextLookupString(Widget, XEvent *, char *, int, KeySym *);
extern void MwRichtextSetZoom(Widget, float);
extern void MwRichtextSetString(Widget, char *);
extern char *MwRichtextGetString(Widget);
extern void MwRichtextInsertText(Widget, MwRichchar *, int);

#endif /* MW_RICHTEXT_H */

