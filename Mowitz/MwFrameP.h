/*
 * MwFrameP.h - Private definitions for MwFrame widget
 * 
 */

#ifndef _MwFrameP_h
#define _MwFrameP_h

/***********************************************************************
 *
 * MwFrame Widget Private Data
 *
 ***********************************************************************/

#include <X11/IntrinsicP.h>
#include "MwFrame.h"

/* New fields for the MwFrame widget class record */
typedef struct {XtPointer extension;} MwFrameClassPart;

/* Full class record declaration */
typedef struct _MwFrameClassRec {
    CoreClassPart	core_class;
    CompositeClassPart  composite_class;
    MwFrameClassPart	frame_class;
} MwFrameClassRec;

extern MwFrameClassRec mwFrameClassRec;



/****************************************************************
 *
 * instance record declaration
 *
 ****************************************************************/

/* New fields for the MwFrame widget record */
typedef struct {
    /* resources */
    XtShadowType type ;
    Dimension	shadowWidth ;
    Pixel	foreground ;
    Widget	title ;
    XtJustify	justify ;
    Dimension   marginHeight, marginWidth ;
    Boolean	allowResize ;
    Boolean	be_nice_to_cmap ;
    int		top_shadow_contrast ;
    int		bot_shadow_contrast ;

    /* private state */
    GC		foregroundGC ;		/* for solid borders */
    GC		topGC ;
    GC		botGC ;
    GC		top_half_GC;
    GC		bot_half_GC;
    Boolean	needs_layout ;
    Position	tx,ty ;			/* title posn */
    Position	sy,sh ;			/* shadow position, height */
} MwFramePart;


typedef struct _MwFrameRec {
    CorePart		core;
    CompositePart	composite;
    MwFramePart		frame;
} MwFrameRec;



#endif /* _MwFrameP_h */
