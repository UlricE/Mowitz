/*
 * This widget manages one child widget, placing a decorative border
 * around it.  The frame may have an optional title, which will be placed
 * at the top, breaking the decoration.  The title may be any widget, but
 * is normally some sort of label widget.
 *
 * Border styles are as follow:
 *
 *	None		no border
 *	Solid		solid border in foreground color
 *	Raised		raised 3d look
 *	Lowered		pressed 3d look
 *	Ridge		raised ridge
 *	Groove		indented groove
 *	Trough		indented groove with flat bottom.
 */


#ifndef _MwFrame_h
#define _MwFrame_h

#include <X11/Xmu/Converters.h>


/***********************************************************************
 *
 * MwFrame Widget (subclass of CompositeClass)
 *
 ***********************************************************************/

/* Parameters:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 shadowType	     ShadowType		ShadowType	solid
 shadowWidth	     ShadowWidth	Dimension
 foreground	     Foreground		Pixel		XtDefaultForeground
 title		     Title		Widget		NULL
 justify	     Justify		XtJustify	left
 marginWidth	     Margin		Dimension	0
 marginHeight	     Margin		Dimension	0
 allowResize	     AllowResize	Boolean		True

 beNiceToColormap    BeNiceToColormap	Boolean		False
 topShadowContrast   TopShadowContrast	int		20
 bottomShadowContrast BottomShadowContrast int		40

 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	0

 destroyCallback     Callback		Pointer		NULL
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 width		     Width		Dimension	0
 height		     Height		Dimension	0
 x		     Position		Position	0
 y		     Position		Position	0

 Notes:

 1 internalWidth, internalHeight specify the margins around the child widget
 2 allowResize specifies if child widget is allowed to resize itself.
 3 BeNiceToColormap causes the MwFrame widget to use fewer colors.

*/

/* New fields */

#ifndef	XtNtitle
#define XtNtitle "title"
#define XtCTitle "Title"
#endif

#ifndef	XtNshadowType
#define XtNshadowType "shadowType"
#define XtCShadowType "ShadowType"
#define XtRShadowType "ShadowType"
#endif

#ifndef	XtNallowResize
#define XtNallowResize "allowResize"
#endif
#ifndef	XtCAllowResize
#define XtCAllowResize "AllowResize"
#endif

#ifndef	XtNmarginWidth
#define	XtNmarginWidth	"marginWidth"
#define	XtNmarginHeight	"marginHeight"
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

typedef	enum {	Blank,		/* no border */
		Solid,		/* solid border in foreground color */
		Raised,		/* raised 3d look */
		Lowered,	/* pressed 3d look */
		Ridge,		/* raised ridge */
		Groove,		/* indented groove */
		Plateau,	/* raised ridge with flat top */
		Trough}		/* indented groove with flat bottom */
	      XtShadowType ;


/* Class record constants */

extern WidgetClass mwFrameWidgetClass;

typedef struct _MwFrameClassRec	*MwFrameWidgetClass;
typedef struct _MwFrameRec	*MwFrameWidget;

#endif /* _MwFrame_h */
