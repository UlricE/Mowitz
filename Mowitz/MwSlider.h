/* $Id: MwSlider.h,v 1.1.1.1 2003/01/16 20:31:09 ulric Exp $
 *
 * Slider:  a widget to allow the user to enter a numeric value.
 */


#ifndef _MwSlider_h
#define _MwSlider_h

#include <X11/Xmu/Converters.h>


/***********************************************************************
 *
 * Slider Widget (subclass of CoreClass)
 *
 * The slider widget is very similar to a scrollbar except that it is
 * intended to represent a numeric value which ranges between two numbers.
 * The thumb is not normally resized, and the range of the slider goes all the
 * way from min to max (a scrollbar stops short of 1. by an amount determined
 * by the thumb size.)
 *
 ***********************************************************************/

/* Resources:

 Name			Class			RepType		Default Value
 ----			-----			-------		-------------
 font			Font			XFontStruct*	XtDefaultFont
 foreground		Foreground		Pixel		XtDefaultFore
 minimum		Minimum			int		0
 maximum		Maximum			int		100
 value			Value			int		0
 step			Step			int		(max-min)/10
 step2			Step2			int		1
 orientation		Orientation		XtOrientation	horizontal
 showValue		ShowValue		Boolean		False	[tbd]
 decimalPoints		DecimalPoints		short		0	[tbd]
 inputScale		InputScale		short		100%
 autoScale		AutoScale		Boolean		True
 thumbLength		ThumbLength		short		20

 shadowWidth		ShadowWidth		short		1
 topShadowContrast	TopShadowContrast	short		20
 bottomShadowContrast	BottomShadowContrast	short		40
 armShadowContrast	ArmShadowContrast	short		40
 insensitiveContrast	InsensitiveContrast	short		33
 beNiceToColormap	BeNiceToColormap	Boolean		False

 dragCallback		DragCallback		XtCallbackList	NULL
 valueChangedCallback	ValueChangedCallback	XtCallbackList	NULL

 Notes:

 * showValue, decimalPoints not implemented yet.

 * step is the amount to change the value when the user clicks in the
   slider background or presses a page up/down key.

 * step2 is the amount to change the value when the user presses an
   arrow key or uses a scrollwheel mouse.

 * inputScale scales pointer motion to changes in the slider.  It is
   very useful for small sliders which would otherwise be difficult
   to manipulate.  Small values for inputScale cause small slider
   changes for a given amount of pointer motion.  E.g. if inputScale
   is 10, then a 100-pixel motion of the pointer results in only a
   10-pixel change in the slider.

 * autoScale causes inputScale to be automatically set for sliders
   with a range of motion less than 100 pixels.

 * dragCallback is called continuously as the slider is adjusted.
   Call_data is the new slider value.

 * valueChangedCallback is called when the user completes an interaction.
   Call_data is the new slider value.

 * style is one of:
 	Normal		looks like Motif scale
	Eletronic	looks like a level control on a mix board.

 * thumbLength is the width of the thumb for horizontal sliders, or the
   height for vertical sliders.

*/



typedef	enum {SLNormal, SLElectronic} MwSliderStyle ;


/* New fields */

#ifndef	XtNminimum
#define	XtNminimum	"minimum"
#define	XtCMinimum	"Minimum"
#endif

#ifndef	XtNmaximum
#define	XtNmaximum	"maximum"
#define	XtCMaximum	"Maximum"
#endif

#ifndef	XtNorientation
#define	XtNorientation	"orientation"
#define	XtCOrientation	"Orientation"
#endif

#ifndef	XtNinputScale
#define	XtNinputScale	"inputScale"
#define	XtCInputScale	"InputScale"
#endif

#ifndef	XtNautoScale
#define	XtNautoScale	"autoScale"
#define	XtCAutoScale	"AutoScale"
#endif

#ifndef	XtNstep
#define	XtNstep		"step"
#define	XtCStep		"Step"
#endif

#ifndef	XtNstep2
#define	XtNstep2	"step2"
#define	XtCStep2	"Step2"
#endif

#ifndef	XtNdecimalPoints
#define	XtNdecimalPoints	"decimalPoints"
#define	XtCDecimalPoints	"DecimalPoints"
#endif

#ifndef	XtNshowValue
#define	XtNshowValue	"showValue"
#define	XtCShowValue	"ShowValue"
#endif

#ifndef	XtNautoScale
#define	XtNautoScale	"autoScale"
#define	XtCAutoScale	"AutoScale"
#endif

#ifndef	XtNthumbLength
#define	XtNthumbLength	"thumbLength"
#define	XtCThumbLength	"ThumbLength"
#endif

#ifndef	XtNsliderStyle
#define	XtNsliderStyle	"sliderStyle"
#define	XtCSliderStyle	"SliderStyle"
#define	XtRSliderStyle	"SliderStyle"
#endif

#ifndef	XtNdragCallback
#define	XtNdragCallback	"dragCallback"
#define	XtCDragCallback	"DragCallback"
#endif

#ifndef	XtNvalueChangedCallback
#define	XtNvalueChangedCallback	"valueChangedCallback"
#define	XtCValueChangedCallback	"ValueChangedCallback"
#endif

#ifndef	XtNshadowWidth
#define XtNshadowWidth "shadowWidth"
#define XtCShadowWidth "ShadowWidth"
#define XtNtopShadowPixel "topShadowPixel"
#define XtCTopShadowPixel "TopShadowPixel"
#define XtNbottomShadowPixel "bottomShadowPixel"
#define XtCBottomShadowPixel "BottomShadowPixel"
#define XtNtopShadowContrast "topShadowContrast"
#define XtCTopShadowContrast "TopShadowContrast"
#define XtNbottomShadowContrast "bottomShadowContrast"
#define XtCBottomShadowContrast "BottomShadowContrast"
#define XtNarmShadowContrast "armShadowContrast"
#define XtCArmShadowContrast "ArmShadowContrast"
#endif

#ifndef	XtNinsensitiveContrast
#define	XtNinsensitiveContrast	"insensitiveContrast"
#define	XtCInsensitiveContrast	"InsensitiveContrast"
#endif

#ifndef	XtNtopShadowPixmap
#define	XtNtopShadowPixmap	"topShadowPixmap"
#define	XtCTopShadowPixmap	"TopShadowPixmap"
#define	XtNbottomShadowPixmap	"bottomShadowPixmap"
#define	XtCBottomShadowPixmap	"BottomShadowPixmap"
#endif

#ifndef	XtNbeNiceToColormap
#define XtNbeNiceToColormap "beNiceToColormap"
#define XtCBeNiceToColormap "BeNiceToColormap"
#define XtNbeNiceToColourmap "beNiceToColormap"
#define XtCBeNiceToColourmap "BeNiceToColormap"
#endif

/* Class record constants */

extern WidgetClass mwSliderWidgetClass;

typedef struct _MwSliderClassRec *MwSliderWidgetClass;
typedef struct _MwSliderRec      *MwSliderWidget;

extern	XtAccelerators	MwSliderAccelerators ;

_XFUNCPROTOBEGIN
extern	void
XawSliderSetValue(
#if NeedFunctionPrototypes
	Widget	w,
	int	newvalue,
	Bool	callCallbacks
#endif
) ;

_XFUNCPROTOEND

#endif /* _MwSlider_h */
