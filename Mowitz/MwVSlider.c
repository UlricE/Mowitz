static	char	rcsid[] = "$Id: MwVSlider.c,v 1.1.1.1 2003/01/16 20:31:17 ulric Exp $" ;

/* VSlider.c -- VSlider widget
 *
 * Author: Edward A. Falk
 *	   falk@falconer.vip.best.com
 *
 * Date: 16 Jan 2000
 *
 * Copyright © 2000 by Edward A. Falk.  This widget may be used freely in
 * any software.  Source code is freely distributable provided that my name
 * is not removed from it.
 *
 * The X Consortium, and any party obtaining a copy of these files
 * from the X Consortium, directly or indirectly, is granted, free of
 * charge, a full and unrestricted irrevocable, world-wide, paid up,
 * royalty-free, nonexclusive right and license to deal in this
 * software and documentation files (the "Software"), including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and sell copies of the Software, and to
 * permit persons who receive copies from any such party to do so.
 *
 *
 *
 * This widget is a subclass of Slider.  It behaves in the same way,
 * but looks like a volume control.
 *
 * $Log: MwVSlider.c,v $
 * Revision 1.1.1.1  2003/01/16 20:31:17  ulric
 * Mowitz
 *
 * Revision 1.1  2000/01/18 20:42:36  falk
 * Initial revision
 *
 */

#include	<stdio.h>
#include	<stdlib.h>
#include	<ctype.h>

#include	<X11/Xlib.h>
#include	<X11/IntrinsicP.h>
#include	<X11/StringDefs.h>
#include	<X11/Xmu/Drawing.h>
#include	<X11/Xmu/Misc.h>
#include	<X11/Xmu/CharSet.h>

#include	"MwVSliderP.h"
#include	"MwGcs.h"

/* note: these dimensions are for a horizontal slider; transpose
 * accordingly for vertical.
 */

#define	DEF_WID		100	/* default width */
#define	DEF_HGT		20	/* default height */
#define	THUMB_WID	8	/* default thumb width */
#define	THUMB_HGT	20	/* default thumb height */


/****************************************************************
 *
 * VSlider Resources
 *
 ****************************************************************/


#define	offset(field)	XtOffsetOf(MwSliderRec, slider.field)
static XtResource resources[] = {
  {XtNthumbLength, XtCThumbLength, XtRShort, sizeof(short),
	offset(thumbLength), XtRImmediate, (XtPointer) THUMB_WID},
};
#undef	offset



	/* FORWARD REFERENCES: */

#ifdef	NeedFunctionPrototypes

	/* member functions */

static	void	VSliderResize(Widget) ;
static	void	VSliderDrawBorder(MwSliderWidget) ;
static	void	VSliderDrawBackground(MwSliderWidget, int, int, int, int) ;
static	void	VSliderFocusProc(MwSliderWidget sw) ;

static	void	VSliderPreferredSize( MwSliderWidget,
			Dimension *wid, Dimension *hgt,
			Dimension *min_wid, Dimension *min_hgt) ;

#else

static	void	VSliderResize() ;
static	void	VSliderDrawBorder() ;
static	void	VSliderDrawBackground() ;
static	void	VSliderFocusProc() ;
static	void	VSliderPreferredSize() ;

#endif




/****************************************************************
*
* Full class record constant
*
****************************************************************/

#define	SuperClass	(&mwSliderClassRec)

MwVSliderClassRec mwVSliderClassRec = {
  {
/* core_class fields      */
    /* superclass         */    (WidgetClass) SuperClass,
    /* class_name         */    "MwVSlider",
    /* widget_size        */    sizeof(MwVSliderRec),
    /* class_initialize   */    NULL,
    /* class_part_init    */	NULL,
    /* class_inited       */	FALSE,
    /* initialize         */    NULL,
    /* initialize_hook    */	NULL,
    /* realize            */    XtInheritRealize,
    /* actions            */    NULL,
    /* num_actions	  */	0,
    /* resources          */    resources,
    /* num_resources      */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion	  */	TRUE,
    /* compress_exposure  */	XtExposeCompressMaximal|
				XtExposeGraphicsExpose|
    				XtExposeGraphicsExposeMerged|
				XtExposeNoRegion,
    /* compress_enterleave*/	TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    NULL,
    /* resize             */    VSliderResize,
    /* expose             */    XtInheritExpose,
    /* set_values         */    NULL,
    /* set_values_hook    */	NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */	NULL,
    /* accept_focus       */    XtInheritAcceptFocus,
    /* version            */	XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    XtInheritTranslations,
    /* query_geometry     */	XtInheritQueryGeometry,
    /* display_accelerator*/	XtInheritDisplayAccelerator,
    /* extension          */	NULL
  },
  {
/* Slider class fields */
    /* draw_border	*/	VSliderDrawBorder,
    /* draw_background	*/	VSliderDrawBackground,
    /* draw_thumb	*/	XtInheritDrawThumb,
    /* move_thumb	*/	XtInheritMoveThumb,
    /* focus_proc	*/	VSliderFocusProc,
    /* preferred_size	*/	VSliderPreferredSize,
    /* extension	*/	NULL,
  },
  {
/* VSlider class fields */
    /* extension	*/	NULL,
  }
};

WidgetClass mwVSliderWidgetClass = (WidgetClass)&mwVSliderClassRec;

#define	MwVSliderClass(w)	((MwVSliderWidgetClass)(w)->core.widget_class)






/****************************************************************
 *
 * Slider Class functions.  See Slider.c for specs.
 *
 ****************************************************************/


static void
VSliderResize(Widget w)
{
	MwSliderWidget	sw = (MwSliderWidget) w;
	int		length ;
	int		minimum, maximum ;

	sw->slider.start = 0 ;

	if( sw->slider.orientation == XtorientHorizontal ) {
	  length = sw->core.width ;
	  sw->slider.thumbWidth = sw->core.height ;
	}
	else {
	  length = sw->core.height ;
	  sw->slider.thumbWidth = sw->core.width ;
	}

	length -= + sw->slider.thumbLength ;
	if( length < 0 ) length = 0 ;
	sw->slider.length = length ;

	/* TODO: shrink thumb if it will make things fit? */

	/* recompute thumb position */
	minimum = sw->slider.minimum ;
	maximum = sw->slider.maximum ;
	if( maximum != minimum )
	  sw->slider.thumbpos =
	    length*(sw->slider.value-minimum)/(maximum-minimum) ;
	else
	  sw->slider.thumbpos = 0 ;

	if( sw->slider.autoScale )
	  sw->slider.inputScale = length < 100 ? length : 100 ;

	sw->slider.needs_layout = False ;
}



static	void
VSliderDrawBorder(MwSliderWidget sw)
{
	/* Nothing.  This function draws any part of the
	 * slider that is never obscured by the thumb, but
	 * there are no such decorations in this widget.
	 */
}


static	void
VSliderDrawBackground(MwSliderWidget sw, int x, int y, int wid, int hgt)
{
	Widget	w = (Widget)sw ;
	int	bgwid ;
	int	x2 = x+wid-1 ;
	int	y2 = y+hgt-1 ;
	int	x0,x1,y0,y1 ;
	int	xx0,xx1,yy0,yy1 ;
	Display	*dpy = XtDisplay(w) ;
	Window	win = XtWindow(w) ;
	GC	topgc = sw->slider.topGC ;
	GC	botgc = sw->slider.botGC ;

	/* Draw background and border which is 1/4 the widget
	 * height (width for vertical sliders)
	 */

	/* clear background */
	XClearArea(dpy,win, x,y,wid,hgt, False) ;


	/* Now find the dimensions of the underlying decorations. */

	if( sw->slider.orientation == XtorientHorizontal ) {
	  bgwid = sw->core.height/4 ;
	  x0 = sw->slider.thumbLength/2 ;
	  x1 = sw->core.width - sw->slider.thumbLength/2 ;
	  y0 = sw->core.height/2 - bgwid/2 ;
	  y1 = sw->core.height/2 + bgwid/2 ;
	}
	else {
	  bgwid = sw->core.width/4 ;
	  x0 = sw->core.width/2 - bgwid/2 ;
	  x1 = sw->core.width/2 + bgwid/2 ;
	  y0 = sw->slider.thumbLength/2 ;
	  y1 = sw->core.height - sw->slider.thumbLength/2 ;
	}

	/* clip to the input region */
	xx0 = Max(x,x0) ;
	xx1 = Min(x2,x1) ;
	yy0 = Max(y,y0) ;
	yy1 = Min(y2,y1) ;

	if( xx0 > xx1 || yy0 > yy1 )
	  return ;

	if( !sw->slider.has_focus )
	  XFillRectangle(dpy, win, sw->slider.greyGC,
		xx0,yy0, xx1-xx0+1, yy1-yy0+1) ;

	/* Fat shadows would be nice, but they're too much work for now, so
	 * we just have 1-pixel shadows.
	 */

	if( xx0 == x0 ) XDrawLine(dpy,win,botgc, xx0,yy0, xx0,yy1) ;
	if( xx1 == x1 ) XDrawLine(dpy,win,topgc, xx1,yy0, xx1,yy1) ;
	if( yy0 == y0 ) XDrawLine(dpy,win,botgc, xx0,yy0, xx1,yy0) ;
	if( yy1 == y1 ) XDrawLine(dpy,win,topgc, xx0,yy1, xx1,yy1) ;
}





	/* In the base class, we indicate focus by a solid border that
	 * will at least partially overlay the 3-d border.  Undraw the
	 * solid border by simply redrawing the 3-d border.
	 */

static	void
VSliderFocusProc(MwSliderWidget sw)
{
#ifdef	COMMENT
	Widget	w = (Widget)sw ;
	int	x,y, wid,hgt ;

	if( sw->slider.has_focus ) {
	  _SliderThumb(sw, &x, &y, &wid, &hgt) ;
	  XDrawRectangle(XtDisplay(w), XtWindow(w), sw->slider.foregroundGC,
	  	x,y, wid-1, hgt-1);
	}
	else
	  SliderClass(sw)->slider_class.draw_thumb(sw) ;
#endif	/* COMMENT */
	/* redraw background to show focus, redraw thumb to repair
	 * damage.
	 */
	SliderClass(sw)->slider_class.draw_background(sw,
		0,0, sw->core.width, sw->core.height) ;
	SliderClass(sw)->slider_class.draw_thumb(sw) ;
}










/****************************************************************
 *
 * Geometry Utilities
 *
 ****************************************************************/




	/*
	 * Find preferred size.  Depends on widget contents.
	 */


static	void
VSliderPreferredSize(MwSliderWidget sw,
	Dimension *reply_width, Dimension *reply_height,
	Dimension *min_wid, Dimension *min_hgt)
{
	int			s = sw->slider.shadowWidth ;

	if( sw->slider.orientation == XtorientHorizontal ) {
	  *reply_width = DEF_WID ;
	  *reply_height = THUMB_HGT ;
	  *min_wid = sw->slider.thumbLength + s*2 + 1 ;
	  *min_hgt = s*2 + 3 ;
	}
	else {
	  *reply_width = THUMB_HGT ;
	  *reply_height = DEF_WID ;
	  *min_wid = s*2 + 3 ;
	  *min_hgt = sw->slider.thumbLength + s*2 + 1 ;
	}
}
