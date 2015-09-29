
/* 
 * MwHandleP.h - Private definitions for MwHandle widget
 * 
 */

#ifndef s_MwHandleP_h
#define s_MwHandleP_h

#include "MwHandle.h"
#include "MwFrameP.h"

/************************************
 *
 *  Class structure
 *
 ***********************************/

   /* New fields for the MwHandle widget class record */
typedef struct s_MwHandleClass {
    int makes_compiler_happy;  /* not used */
} MwHandleClassPart;

   /* Full class record declaration */
typedef struct s_MwHandleClassRec {
    CoreClassPart	core_class;
    CompositeClassPart	composite_class;
    MwFrameClassPart	frame_class;
    MwHandleClassPart    handle_class;
} MwHandleClassRec;

extern MwHandleClassRec mwHandleClassRec;

/***************************************
 *
 *  Instance (widget) structure 
 *
 **************************************/

    /* New fields for the MwHandle widget record */
typedef struct {
    /* resources */
	Widget victim;
	XtCallbackList detach_callbacks;
	XtCallbackList attach_callbacks;

    /* private state */
	Boolean detached;
	Widget shell;
	Widget parent;
	Position old_x, old_y;
	Time oldtime;
	Boolean mwm, managed;
} MwHandlePart;


   /* Full widget declaration */
typedef struct s_MwHandleRec {
    CorePart		core;
    CompositePart	composite;
    MwFramePart		frame;
    MwHandlePart		handle;
} MwHandleRec;

#endif /* s_MwHandleP_h */


