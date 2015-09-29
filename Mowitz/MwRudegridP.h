/*
 * MwRudegridP.h - Private definitions for MwRudegrid widget
 *
 * Loosely based on Ed Falk's Gridbox widget.
 */

#ifndef _MwRudegridP_h
#define _MwRudegridP_h

#include "MwRudegrid.h"
#include <X11/ConstrainP.h>

typedef struct {
    XtPointer	extension ;
} MwRudegridClassPart;

typedef struct _MwRudegridClassRec {
    CoreClassPart	core_class;
    CompositeClassPart	composite_class;
    ConstraintClassPart	constraint_class;
    MwRudegridClassPart	rudegrid_class;
} MwRudegridClassRec;

extern MwRudegridClassRec mwRudegridClassRec;

typedef struct _MwRudegridPart {
    /* resources */
    String	x_layout, y_layout;

    /* private state */
} MwRudegridPart;

typedef struct _MwRudegridRec {
    CorePart		core;
    CompositePart	composite;
    ConstraintPart	constraint;
    MwRudegridPart	rudegrid;
} MwRudegridRec;

typedef struct _MwRudegridConstraintsPart {
/*
 * Constraint Resources.
 */
    Position	gridx, gridy ;	/* position in the grid			*/
    Dimension	gridWidth,	/* size in cells			*/
    		gridHeight ;
/*
 * Private contstraint resources.
 */
} MwRudegridConstraintsPart;

typedef struct _MwRudegridConstraintsRec {
    MwRudegridConstraintsPart	rudegrid;
} MwRudegridConstraintsRec, *MwRudegridConstraints;

#endif /* _MwRudegridP_h */
