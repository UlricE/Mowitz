/*
Copyright (C) 1996-2001  Ulric Eriksson <ulric@siag.nu>

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

#ifndef s_MwImage_h
#define s_MwImage_h

/****************************************************************
 *
 * MwImage widget
 *
 ****************************************************************/

/* Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 imageData	     ImageData		Pointer		NULL
 imageExpose	     ImageExpose	Pointer		NULL
 destroyCallback     Callback		Pointer		NULL
 height		     Height		Dimension	0
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 sensitive	     Sensitive		Boolean		True
 width		     Width		Dimension	0
 x		     Position		Position	0
 y		     Position		Position	0

*/

/* define any special resource names here that are not in <X11/StringDefs.h> */

#define XtNimageExpose "imageExpose"
#define XtNimageData "imageData"

#define XtCImageExpose "ImageExpose"
#define XtCImageData "ImageData"

/* declare specific MwImageWidget class and instance datatypes */

typedef struct s_MwImageClassRec*	MwImageWidgetClass;
typedef struct s_MwImageRec*		MwImageWidget;

/* declare the class constant */

extern WidgetClass mwImageWidgetClass;


#endif /* s_MwImage_h */

