/* $Id: MwVSlider.h,v 1.1.1.1 2003/01/16 20:31:09 ulric Exp $
 *
 * VSlider:  Slider subclass, looks like a volume control.
 */


#ifndef _MwVSlider_h
#define _MwVSlider_h

#include <X11/Xmu/Converters.h>


/***********************************************************************
 *
 * VSlider Widget (subclass of Slider)
 *
 * Identical in behavior to Slider, but looks like a volume control.
 *
 ***********************************************************************/

/* Resources:

 Same as Slider; see Slider.h

*/


extern WidgetClass mwVSliderWidgetClass;

typedef struct _MwVSliderClassRec *MwVSliderWidgetClass;
typedef struct _MwVSliderRec      *MwVSliderWidget;

#endif /* _Slider_h */
