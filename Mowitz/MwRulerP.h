/* $Id: MwRulerP.h,v 1.1.1.1 2003/01/16 20:31:09 ulric Exp $
 *
 * RulerP.h - Private definitions for Index Ruler widget
 * 
 */

#ifndef _MwRulerP_h
#define _MwRulerP_h

/***********************************************************************
 *
 * Ruler Widget Private Data
 *
 ***********************************************************************/

#include <X11/IntrinsicP.h>
#include <X11/Xmu/Converters.h>
#include "MwRuler.h"

/* New fields for the Ruler widget class record */
typedef struct {XtPointer extension;} MwRulerClassPart;

/* Full class record declaration */
typedef struct _MwRulerClassRec {
    CoreClassPart	core_class;
    MwRulerClassPart	ruler_class;
} MwRulerClassRec;

extern MwRulerClassRec mwRulerClassRec;



/****************************************************************
 *
 * instance record declaration
 *
 ****************************************************************/

/* New fields for the MwRuler widget record */
typedef struct {
    /* resources */
    int		labelStep ;		/* want labels this many units apart */
    int		labelDiv ;		/* want this many labels/unit */
    XtLabelStyle labelStyle ;		/* fractions or decimal */
    int		markStep ;		/* want marks this many units apart */
    int		markDiv ;		/* want this many marks/unit */
    int		orientation ;		/* N/S/E/W */
    Pixel	foreground ;
    Pixel	pointerColor ;
    Boolean	showPointer ;		/* flag: show pointer */
    float	minValue ;		/* left/top-most value */
    float	scale ;			/* pixels/unit */
    float	value ;			/* current pointer position */
    int		iValue ;		/* same, in pixels */
    Cursor	cursor ;		/* normal cursor */
    XFontStruct	*font, *fracFont ;
    XtCallbackList callbacks ;
    XtPointer	userData ;

    /* private state */
    GC		foregroundGC ;
    GC		fracGC ;
    GC		pointerGC ;
    int		drag ;		/* cursor tracking */
    Region	clip ;		/* full region */
    Pixmap	indexPm ;	/* saved pixels under index */
    Boolean	indexSaved ;	/* pixels have been saved */

    /* Here is the info used to redraw the ruler: */

    int		length ;	/* length of working area of ruler */
    int		lStep ;		/* actual label step */
    int		lDiv ;		/* actual label divisions */
    int		mStep ;		/* actual mark step to use */
    int		mDiv ;		/* actual mark divisions */
    long	l0 ;		/* pixel value of base label */
    float	v0 ;		/* value of base label */
    int		txtWid ;
    Boolean	needs_layout ;
} MwRulerPart;


typedef struct _MwRulerRec {
    CorePart		core;
    MwRulerPart		ruler;
} MwRulerRec;


#endif /* _MwRulerP_h */
