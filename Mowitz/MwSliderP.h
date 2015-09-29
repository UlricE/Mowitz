/* $Id: MwSliderP.h,v 1.1.1.1 2003/01/16 20:31:09 ulric Exp $
 *
 * SliderP.h - Private definitions for Slider widget
 * 
 */

#ifndef _MwSliderP_h
#define _MwSliderP_h

/***********************************************************************
 *
 * Slider Widget Private Data
 *
 ***********************************************************************/

#include <X11/IntrinsicP.h>
#include "MwSlider.h"

typedef	void	(*sliderProc)(MwSliderWidget) ;
typedef	void	(*sliderBgProc)(MwSliderWidget, int x, int y, int wid, int hgt);
typedef	void	(*sliderMoveProc)(MwSliderWidget, int newpos) ;
typedef	void	(*sliderPrefSize)(MwSliderWidget, Dimension *, Dimension *,
			Dimension *, Dimension *) ;

/* programming note:  The convention is to define these as _XtInherit, but
 * since NULL is not permitted in the class structure, things work out
 * better if I do it this way.
 */

#define	XtInheritDrawBorder	NULL
#define	XtInheritDrawBackground	NULL
#define	XtInheritDrawThumb	NULL
#define	XtInheritMoveThumb	NULL
#define	XtInheritFocusProc	NULL
#define	XtInheritPreferredSize	NULL



/* New fields for the Slider widget class record */
typedef struct {
    sliderProc	draw_border ;
    sliderBgProc draw_background ;
    sliderProc	draw_thumb ;
    sliderMoveProc move_thumb ;
    sliderProc	focus_proc ;
    sliderPrefSize preferred_size ;
    XtPointer	extension;
} MwSliderClassPart;

/* Full class record declaration */
typedef struct _MwSliderClassRec {
    CoreClassPart	core_class;
    MwSliderClassPart	slider_class;
} MwSliderClassRec;

extern MwSliderClassRec mwSliderClassRec;



/****************************************************************
 *
 * instance record declaration
 *
 ****************************************************************/

/* New fields for the Slider widget record */
typedef struct {
    /* resources */
    XFontStruct	*font ;
    Pixel	foreground ;
    int		minimum, maximum, value ;
    int		step, step2 ;
    XtOrientation orientation ;
    short	decimalPoints ;
    short	inputScale ;
    short	thumbLength ;
    Boolean	showValue ;
    Boolean	autoScale ;
    XtCallbackList dragCallback ;
    XtCallbackList valueChangedCallback ;
    short	shadowWidth ;
    short	top_contrast ;
    short	bot_contrast ;
    short	arm_contrast ;
    short	insensitive_contrast ;
    Boolean	be_nice_to_cmap ;

    /* private state */
    Boolean	needs_layout ;
    Boolean	has_focus ;
    GC		foregroundGC ;
    GC		backgroundGC ;
    GC		greyGC ;
    GC		topGC ;
    GC		botGC ;
    Pixmap	grey50 ;		/* TODO: cache this elsewhere */
    int		oldvalue ;
    short	start ;			/* first pixel thumb may occupy */
    short	length ;		/* working length */
    short	thumbpos ;
    short	adjustPt ;
    short	thumbWidth ;
    Boolean	adjusting ;
    XtPointer	extension ;
} MwSliderPart;


typedef struct _MwSliderRec {
    CorePart	core;
    MwSliderPart	slider;
} MwSliderRec;



	/* internals */

#define	SliderClass(w)	((MwSliderWidgetClass)(w)->core.widget_class)


_XFUNCPROTOBEGIN
extern	void
_MwSliderThumb(
#if NeedFunctionPrototypes
	MwSliderWidget sw,
	int	*x,
	int	*y,
	int	*wid,
	int	*hgt
#endif
) ;

_XFUNCPROTOEND

#endif /* _MwSliderP_h */
