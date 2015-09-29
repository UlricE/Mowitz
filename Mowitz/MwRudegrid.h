/*
 * This widget manages multiple child widgets, arranging them in a
 * rectangular grid.  Child widgets may occupy multiple grid cells.
 */



#ifndef _MwRudegrid_h
#define _MwRudegrid_h

/***********************************************************************
 *
 * MwRudegrid Widget -- based on Ed Falk's Gridbox widget
 * which is in turn based on the Java Gridbag layout.
 * 
 * Unlike the Gridbox and most other geometry managing widgets, this
 * one doesn't care what size the children would like to be.
 *
 ***********************************************************************/

/* Parameters:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 defaultDistance     Thickness		int		4
 destroyCallback     Callback		Pointer		NULL
 width		     Width		Dimension	computed at realize
 height		     Height		Dimension	computed at realize
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 x		     Position		Position	0
 y		     Position		Position	0


  xLayout	     XLayout		String		NULL
  yLayout	     YLayout		String		NULL

  Specify cell widths and heights in pixels and/or percent

  All other resources are the same as for Constraint.

*/

/* Constraint parameters:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 gridx		     Position		Position	0
 gridy		     Position		Position	0
 gridWidth	     Width		Dimension	1
 gridHeight	     Height		Dimension	1

  gridx, gridy		position of child in grid.  Upper-left cell is 0,0
  gridWidth,gridHeight	size of child in cells.
*/


#ifndef	XtNgridx
#define	XtNgridx	"gridx"
#define	XtNgridy	"gridy"
#define	XtNgridWidth	"gridWidth"
#define	XtNgridHeight	"gridHeight"
#define XtNxLayout	"xLayout"
#define XtCXLayout	"XLayout"
#define XtNyLayout	"yLayout"
#define XtCYLayout	"YLayout"
#endif

typedef	struct _MwRudegridClassRec	*MwRudegridWidgetClass ;
typedef	struct _MwRudegridRec	*MwRudegridWidget ;

extern	WidgetClass	mwRudegridWidgetClass ;

_XFUNCPROTOBEGIN

_XFUNCPROTOEND
 
#endif /* _MwRudegrid_h */
