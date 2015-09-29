/*
 * MwTabsP.h - Private definitions for Index MwTabs widget
 * 
 */

#ifndef _MwTabsP_h
#define _MwTabsP_h

/***********************************************************************
 *
 * MwTabs Widget Private Data
 *
 ***********************************************************************/

#include <X11/IntrinsicP.h>
#include "MwTabs.h"

/* New fields for the MwTabs widget class record */
typedef struct {XtPointer extension;} MwTabsClassPart;

/* Full class record declaration */
typedef struct _MwTabsClassRec {
    CoreClassPart	core_class;
    CompositeClassPart  composite_class;
    ConstraintClassPart	constraint_class;
    MwTabsClassPart	tabs_class;
} MwTabsClassRec;

extern MwTabsClassRec mwTabsClassRec;



/****************************************************************
 *
 * instance record declaration
 *
 ****************************************************************/

/* New fields for the MwTabs widget record */
typedef struct {
    /* resources */
    XFontStruct	*font ;
    Dimension   internalHeight, internalWidth ;
    Widget	topWidget ;
    XtCallbackList callbacks ;
    Boolean	selectInsensitive ;
    Boolean	be_nice_to_cmap ;
    int		top_shadow_contrast ;
    int		bot_shadow_contrast ;
    int		insensitive_contrast ;

    /* private state */
    GC		foregroundGC ;
    GC		backgroundGC ;
    GC		greyGC ;
    GC		topGC ;
    GC		botGC ;
    Dimension	tab_height ;		/* height of tabs (all the same) */
    					/* Note: includes top shadow only */
    Dimension	tab_total ;		/* total height of all tabs */
    Dimension	child_width, child_height; /* child size, including borders */
    Cardinal	numRows ;
    XtGeometryMask last_query_mode;
    Boolean	needs_layout ;
    Pixmap	grey50 ;		/* TODO: cache this elsewhere */
} MwTabsPart;


typedef struct _MwTabsRec {
    CorePart		core;
    CompositePart	composite;
    ConstraintPart	constraint;
    MwTabsPart		tabs;
} MwTabsRec;




/****************************************************************
 *
 * constraint record declaration
 *
 ****************************************************************/

typedef	struct _MwTabsConstraintsPart {
	/* resources */
	String	label ;
	Pixmap	left_bitmap ;
	Pixel	foreground ;
	Boolean	resizable ;

	/* private state */
	Pixel		grey ;
	Boolean		greyAlloc ;
	Dimension	width ;		/* tab width */
	Position	x,y ;		/* tab base position */
	short		row ;		/* tab row */
	Dimension	bwid ;		/* desired border width */
	Position	l_x, l_y ;	/* label position */
	Position	lbm_x, lbm_y ;	/* bitmap position */
	unsigned int	lbm_width, lbm_height, lbm_depth ;
} MwTabsConstraintsPart ;

typedef	struct _MwTabsConstraintsRec {
	MwTabsConstraintsPart	tabs ;
} MwTabsConstraintsRec, *MwTabsConstraints ;


#endif /* _MwTabsP_h */
