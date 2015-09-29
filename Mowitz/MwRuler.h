/* $Id: MwRuler.h,v 1.1.1.1 2003/01/16 20:31:09 ulric Exp $
 *
 * This widget draws a ruler.  An arrow may be drawn on the ruler to
 * indicate a value, typically a cursor position.  The user may also grip
 * the ruler and slide it, typically to scroll a window.
 *
 * The ruler consists of three elements:
 *	Numeric labels
 *	Tic marks
 *	An optional sliding arrow indicating a value.
 *
 * Numeric labels occur at every unit along the ruler (space
 * permitting). In addition, fractions will also be displayed between
 * the labels.
 *
 * Likewise, major tic marks will be displayed at every unit along the ruler,
 * with minor tic marks in between.
 *
 */


#ifndef _MwRuler_h
#define _MwRuler_h

#include <X11/Core.h>


/***********************************************************************
 *
 * Ruler Widget (subclass of Core)
 *
 ***********************************************************************/

/* Parameters:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 markStep	     Step		int		1
 markDiv	     Divisions		int		32
 labelStep	     Step		int		1
 labelDiv	     Divisions		int		32
 labelStyle	     LabelStyle		XtLabelStyle	Fraction
 orientation	     Orientation	int		North
 foreground	     Foreground		Pixel		XtDefaultForeground
 pointerColor	     Foreground		Pixel		XtDefaultForeground
 showPointer	     ShowPointer	Boolean		True
 minValue	     MinValue		float		0.0
 scale		     Scale		float		100.0
 value		     Value		float		0.0
 iValue		     IValue		int		0
 font		     Font		XFontStruct*	XtDefaultFont
 fracFont	     Font		XFontStruct*	font
 cursor		     Cursor		Cursor*
 internalWidth	     Width		Dimension	4
 internalHeight	     Height		Dimension	2
 callback	     Callback		XtCallbackList	NULL
 userData	     UserData		XtPointer	NULL

 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 destroyCallback     Callback		Pointer		NULL
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 width		     Width		Dimension	0
 height		     Height		Dimension	0
 x		     Position		Position	0
 y		     Position		Position	0

 Notes:

  * MarkStep controls how many units apart major tic marks are drawn.
    Set to zero to disable.  MarkDiv controls how many minor tic marks
    are drawn between major tic marks.  Set to zero to disable.  If there
    is not enough room for tic marks, the ruler widget will choose a
    lower value for MarkDiv and/or a higher value for MarkStep.
  * LabelStep, LabelDiv are the same, but for labels.
  * LabelStyle is either "Fraction" or "Decimal"
  * Orientation controls the orientation of the ruler.  North is horizontal
    orientation with tic marks at the bottom (useful for rulers North of
    drawing windows.)  South is horizontal with tic marks at top.  East is
    vertical with tic marks on left.  West is vertical with tic marks on right.
  * PointerColor is the color of the arrow.
  * minValue is the minimum value displayed by the ruler, shown at the left
    or top of the ruler.
  * Scale is the number of pixels per unit in the ruler.
  * Value is the ruler value indicated by the pointer.
  * Ivalue is the pointer position in pixels.  Setting Ivalue sets Value
    to the nearest approximation, and visa-versa
  * Callback is called whenever the user manipulates the ruler.  Call_data
    is a pointer to a RulerReport structure.
  * FracFont is the font to use for fractions.
  * Cursor is either XC_sb_h_double_arrow or XC_sb_v_double_arrow,
    depending on orientation.  If the ruler is insensitive, the
    cursor is None.
*/

/* New fields */

#ifndef	XtNmarkStep
#define	XtNmarkStep	"markStep"
#define	XtNlabelStep	"labelStep"
#define	XtCStep		"Step"
#define	XtNlabelStyle	"labelStyle"
#define	XtCLabelStyle	"LabelStyle"
#define	XtRLabelStyle	XtCLabelStyle
#define	XtNmarkDiv	"markDiv"
#define	XtNlabelDiv	"labelDiv"
#define	XtCDivisions	"Divisions"
#define	XtNminValue	"minValue"
#define	XtCMinValue	"MinValue"
#define	XtNscale	"scale"
#define	XtCScale	"Scale"
#define	XtNiValue	"iValue"
#define	XtCIValue	"IValue"
#endif

#ifndef	XtNorientation
#define	XtNorientation	"orientation"
#define	XtCOrientation	"Orientation"
#endif

#ifndef	XtNpointerColor
#define	XtNpointerColor	"pointerColor"
#define	XtCPointerColor	"PointerColor"
#endif

#ifndef	XtNshowPointer
#define	XtNshowPointer	"showPointer"
#define	XtCShowPointer	"ShowPointer"
#endif

#ifndef	XtNcursor
#define	XtNcursor	"cursor"
#endif

#ifndef	XtNuserData
#define	XtNuserData	"userData"
#define	XtCUserData	"UserData"
#endif

#ifndef	XtNfracFont
#define	XtNfracFont	"fracFont"
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

extern WidgetClass mwRulerWidgetClass;

typedef struct _MwRulerClassRec *MwRulerWidgetClass;
typedef struct _MwRulerRec      *MwRulerWidget;

typedef	struct {
	  enum {START,SCROLL,STOP} what ;
	  float	minValue ;	/* new minValue */
	  int	dx, dy ;	/* scroll amount, pixels */
	} MwRulerReport ;

typedef	enum {FRACTION, DECIMAL} XtLabelStyle ;

_XFUNCPROTOBEGIN

extern	void			/* set minimum value and scale */
MwRulerSetMinScale(
#if NeedFunctionPrototypes
	Widget	w,
	double	minValue,
	double	scale
#endif
) ;

extern	void		/* set minimum value */
MwRulerSetMin(
#if NeedFunctionPrototypes
	Widget	w,
	double	minValue
#endif
) ;

extern	void		/* set minimum value at position other than 0 */
MwRulerSetMinPosition(
#if NeedFunctionPrototypes
	Widget	w,
	double	minValue,
	int	position
#endif
) ;

extern	void			/* set scale */
MwRulerSetScale(
#if NeedFunctionPrototypes
	Widget	w,
	double	scale
#endif
) ;

extern	double			/* get minimum value */
MwRulerGetMin(
#if NeedFunctionPrototypes
	Widget	w
#endif
) ;

extern	double			/* get scale */
MwRulerGetScale(
#if NeedFunctionPrototypes
	Widget	w
#endif
) ;

extern	void			/* set pointer value */
MwRulerSetValue(
#if NeedFunctionPrototypes
	Widget	w,
	double	value
#endif
) ;

extern	void			/* set pointer pixel position */
MwRulerSetIValue(
#if NeedFunctionPrototypes
	Widget	w,
	int	value
#endif
) ;

extern	void			/* set pointer pixel position */
MwRulerShowPointer(
#if NeedFunctionPrototypes
	Widget	w,
	Bool	show	/* true or false */
#endif
) ;

extern	double			/* get pointer value */
MwRulerGetValue(
#if NeedFunctionPrototypes
	Widget	w
#endif
) ;

extern	int			/* get pointer pixel position */
MwRulerGetIValue(
#if NeedFunctionPrototypes
	Widget	w
#endif
) ;


extern	double			/* map pixel position to value */
MwRulerPosition2Value(
#if NeedFunctionPrototypes
	Widget	w,
	int	position
#endif
) ;


extern	int			/* map value to pixel position */
MwRulerValue2Position(
#if NeedFunctionPrototypes
	Widget	w,
	double	value
#endif
) ;

_XFUNCPROTOEND

#endif /* _MwRuler_h */
