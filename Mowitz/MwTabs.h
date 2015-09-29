/*
 * This widget manages one or more child widgets, exactly one of which is
 * visible.  Above the child widgets is a graphic that looks like index
 * tabs from file folders.  Each tab corresponds to one of the child widgets.
 * By clicking on a tab, the user can bring the corresponding widget to
 * the top of the stack.
 */


#ifndef _MwTabs_h
#define _MwTabs_h

#include <X11/Constraint.h>


/***********************************************************************
 *
 * MwTabs Widget (subclass of CompositeClass)
 *
 ***********************************************************************/

/* Parameters:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 font		     Font		XFontStruct*	XtDefaultFont
 internalWidth	     Width		Dimension	4	*1
 internalHeight	     Height		Dimension	2	*1
 topWidget	     TopWidget		Widget			*2
 callback	     Callback		XtCallbackList	NULL	*3
 selectInsensitive   SelectInsensitive	Boolean		True	*4
 beNiceToColormap    BeNiceToColormap	Boolean		False	*5
 topShadowContrast   TopShadowContrast	int		20
 bottomShadowContrast BottomShadowContrast int		40
 insensitiveContrast InsensitiveContrast int		33	*6

 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 destroyCallback     Callback		Pointer		NULL
 hSpace 	     HSpace		Dimension	4
 height		     Height		Dimension	0
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 orientation	     Orientation	XtOrientation	vertical
 vSpace 	     VSpace		Dimension	4
 width		     Width		Dimension	0
 x		     Position		Position	0
 y		     Position		Position	0

 Notes:

 1 internalWidth, internalHeight specify the margins around the text
   in the tabs.
 2 topWidget identifies the widget which is currently visible.
 3 callbacks are called whenever the user selects a tab.  Call_data is
   the new top widget.
 4 SelectInsensitive determines whether or not insensitive children may
   be selected anyway.
 5 BeNiceToColormap causes the MwTabs widget to use fewer colors.
 6 InsensitiveContrast sets the contrast used for labels of insensitive widgets.

*/

/* Constraint parameters:
 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 tabLabel	     Label		String		widget name
 tabLeftBitmap	     LeftBitmap		Pixmap		None
 tabForeground	     Foreground		Pixel		XtDefaultForeground
 resizable	     Resizable		Boolean		False
*/

/* New fields */

#ifndef	XtNtabLabel
#define	XtNtabLabel		"tabLabel"
#define	XtNtabForeground	"tabForeground"
#endif

#ifndef	XtNtabLeftBitmap
#define	XtNtabLeftBitmap	"tabLeftBitmap"
#endif

#ifndef	XtCLeftBitmap
#define	XtCLeftBitmap	"LeftBitmap"
#endif

#ifndef	XtCResizable
#define	XtCResizable	"Resizable"
#endif

#ifndef	XtNselectInsensitive
#define	XtNselectInsensitive	"selectInsensitive"
#define	XtCSelectInsensitive	"SelectInsensitive"
#endif

#ifndef	XtNnlabels
#define	XtNnlabels	"nlabels"
#define	XtCNLabels	"NLabels"
#endif
#ifndef	XtNlabels
#define	XtNlabels	"labels"
#define	XtCLabels	"Labels"
#endif

#ifndef	XtNtopWidget
#define	XtNtopWidget	"topWidget"
#define	XtCTopWidget	"TopWidget"
#endif

#ifndef	XtNhSpace
#define	XtNhSpace	"hSpace"
#define	XtCHSpace	"HSpace"
#define	XtNvSpace	"vSpace"
#define	XtCVSpace	"VSpace"
#endif

#ifndef	XtNresizable
#define	XtNresizable	"resizable"
#endif

#ifndef	XtNinsensitiveContrast
#define	XtNinsensitiveContrast	"insensitiveContrast"
#define	XtCInsensitiveContrast	"InsensitiveContrast"
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

extern WidgetClass mwTabsWidgetClass;

typedef struct _MwTabsClassRec *MwTabsWidgetClass;
typedef struct _MwTabsRec      *MwTabsWidget;

_XFUNCPROTOBEGIN
extern	void
XawTabsSetTop(
#if NeedFunctionPrototypes
	Widget	w,
	int	callCallbacks
#endif
) ;

_XFUNCPROTOEND

#endif /* _MwTabs_h */
