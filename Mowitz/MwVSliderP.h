/* $Id: MwVSliderP.h,v 1.1.1.1 2003/01/16 20:31:09 ulric Exp $
 *
 * VSliderP.h - Private definitions for VSlider widget
 * 
 */

#ifndef _MwVSliderP_h
#define _MwVSliderP_h

/***********************************************************************
 *
 * VSlider Widget Private Data
 *
 ***********************************************************************/

#include "MwSliderP.h"
#include "MwVSlider.h"



/* New fields for the VSlider widget class record */
typedef struct {
    XtPointer	extension;
} MwVSliderClassPart;

/* Full class record declaration */
typedef struct _MwVSliderClassRec {
    CoreClassPart	core_class;
    MwSliderClassPart	slider_class;
    MwVSliderClassPart	vslider_class;
} MwVSliderClassRec;

extern MwVSliderClassRec mwVsliderClassRec;



/****************************************************************
 *
 * instance record declaration
 *
 ****************************************************************/

/* New fields for the VSlider widget record */
typedef struct {
    /* resources */

    /* private state */
    XtPointer	extension ;
} MwVSliderPart;


typedef struct _MwVSliderRec {
    CorePart	core;
    MwSliderPart	slider;
    MwVSliderPart	vslider;
} MwVSliderRec;




#endif /* _MwVSliderP_h */
