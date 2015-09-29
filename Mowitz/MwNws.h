/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef MW_NWS_H
#define MW_NWS_H

#include <X11/Xlib.h>
#include <X11/xpm.h>

#ifndef XtRBox_type
#define XtRBox_type	"Box_type"
#endif

#ifndef XtRNwsJustify
#define XtRNwsJustify	"NwsJustify"
#endif

#ifndef XtRIcon
#define XtRIcon		"Icon"
#endif

#ifndef XtRCheck_type
#define XtRCheck_type		"Check_type"
#endif

#ifndef XtRObject_type
#define XtRObject_type		"Object_type"
#endif

#ifndef XtROrientation
#define XtROrientation		"Orientation"
#endif

#ifndef XtRListStruct
#define XtRListStruct		"ListStruct"
#endif

#ifndef XtRButtonMode
#define XtRButtonMode		"ButtonMode"
#endif

#ifndef XtRStringList
#define XtRStringList		"StringList"
#endif

/* constant for box type definition */
#define XtCno_box 		0
#define XtCsimple_box		1
#define XtCup_box		2
#define XtCdown_box		3
#define XtCframein_box		4
#define XtCframeout_box		5
#define XtCshadow_box		6

/* constant for justifycation definition */
#define XtCcenter		0
#define XtCright		1
#define XtCleft			2
#define XtCbottom		3
#define XtCtop			4

/* constant for check type definition */
#define XtCcheck		0
#define XtCcircle		1
#define XtCrectangle		2
#define XtCdiamond		3
#define XtCcross		4
#define XtCcircle2		5

/* constant for object type definition */
#define XtCright_arrow		0
#define XtCleft_arrow		1
#define XtCtop_arrow		2
#define XtCbottom_arrow		3

/* constant for orientation definition */
#define XtCvertical		0
#define XtChorizontal		1

/* button mode constants */
#define XtCnormalMode	0
#define XtCcyclicMode	1
#define	XtCtoggleMode	2

typedef struct {
	Pixmap		pixmap;
	Pixmap		shape;
	unsigned int	width;
	unsigned int	height;
} Icon;

typedef struct {
	int		index;
	char		*label;
	Icon		*left_icon;
	Icon		*right_icon;
	Boolean		sensitive;
	void		*related_info;
} ListStruct;

#if  NeedFunctionPrototypes
extern Boolean  cvtStringToStringList(Display *,XrmValuePtr,Cardinal *,XrmValuePtr,XrmValuePtr,XtPointer *);
extern Boolean  cvtStringToBoxType(Display *,XrmValuePtr,Cardinal *,XrmValuePtr,XrmValuePtr,XtPointer *);
extern Boolean  cvtBoxTypeToString(Display *,XrmValuePtr,Cardinal *,XrmValuePtr,XrmValuePtr,XtPointer *);
extern Boolean  cvtStringToJustify(Display *,XrmValuePtr,Cardinal *,XrmValuePtr,XrmValuePtr,XtPointer *);
extern Boolean  cvtStringToIcon(Display *,XrmValuePtr,Cardinal *,XrmValuePtr,XrmValuePtr,XtPointer *);
extern Boolean  cvtStringToCheckType(Display *,XrmValuePtr,Cardinal *,XrmValuePtr,XrmValuePtr,XtPointer *);
extern Boolean  cvtStringToObjectType(Display *,XrmValuePtr,Cardinal *,XrmValuePtr,XrmValuePtr,XtPointer *);
extern Boolean  cvtStringToOrientation(Display *,XrmValuePtr,Cardinal *,XrmValuePtr,XrmValuePtr,XtPointer *);
extern Boolean  cvtStringToListStruct(Display *,XrmValuePtr,Cardinal *,XrmValuePtr,XrmValuePtr,XtPointer *);
extern Boolean  cvtStringToButtonMode(Display *,XrmValuePtr,Cardinal *,XrmValuePtr,XrmValuePtr,XtPointer *);
#else
extern Boolean  cvtStringToStringList();
extern Boolean  cvtStringToBoxType();
extern Boolean  cvtBoxTypeToString();
extern Boolean  cvtStringToJustify();
extern Boolean  cvtStringToIcon();
extern Boolean  cvtStringToCheckType();
extern Boolean  cvtStringToObjectType();
extern Boolean  cvtStringToOrientation();
extern Boolean  cvtStringToListStruct();
extern Boolean  cvtStringToButtonMode();
#endif


#define MwNwsDone(type, value) do {\
	if (to->addr != NULL) {\
		if (to->size < sizeof(type)) {\
			to->size = sizeof(type);\
			return False;\
		}\
		*(type*)(to->addr) = (value);\
	} else {\
		static type static_val;\
		static_val = (value);\
		to->addr = (XtPointer)&static_val;\
	}\
	to->size = sizeof(type);\
	return True;\
    }while (0 )

#endif
/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _dimdefP_H_
#define _dimdefP_H_

#if NeedFunctionPrototypes
typedef void (*set_internal_dimension_Proc) (Widget,Dimension,Dimension);
#else
typedef void (*set_internal_dimension_Proc) ();
#endif

#if NeedFunctionPrototypes
typedef void (*get_internal_dimension_Proc) (Widget,Position *,Position *,Dimension *,Dimension *);
#else
typedef void (*get_internal_dimension_Proc) ();
#endif

#if NeedFunctionPrototypes
typedef Widget (*traverse_Proc) (Widget, int ,Time *);
typedef void (*traverseTo_Proc) (Widget, Widget ,Time *);
typedef void (*traverseOut_Proc) (Widget);
typedef Widget (*traverseInside_Proc) (Widget, int ,Time *);
typedef void (*highlightBorder_Proc) (Widget);
typedef void (*unhighlightBorder_Proc) (Widget);
#else
typedef Widget (*traverse_Proc) ();
typedef void (*traverseTo_Proc) ();
typedef void (*traverseOut_Proc) ();
typedef Widget (*traverseInside_Proc) ();
typedef void (*highlightBorder_Proc) ();
typedef void (*unhighlightBorder_Proc) ();
#endif

#define  XtInheritGetInternalDimension ((get_internal_dimension_Proc) _XtInherit)
#define  XtInheritSetInternalDimension ((set_internal_dimension_Proc) _XtInherit)
#define  XtInheritTraverse ((traverse_Proc) _XtInherit)
#define  XtInheritTraverseTo ((traverseTo_Proc) _XtInherit)
#define  XtInheritTraverseOut ((traverseOut_Proc) _XtInherit)
#define  XtInheritTraverseInside ((traverseInside_Proc) _XtInherit)
#define  XtInheritHighlightBorder ((highlightBorder_Proc) _XtInherit)
#define  XtInheritUnhighlightBorder ((unhighlightBorder_Proc) _XtInherit)

#endif
/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _Init_h_
#define _Init_h_

extern void _InitializeWidgetSet(void);

#endif

/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _utils_h_
#define _utils_h_

#include <X11/Xutil.h>

extern void X_DrawSoft3DFrame(Display *, Drawable, int, int, int, int,
			int, Pixel, Pixel, Pixel);

extern void X_DrawSimple3DFrame(Display * , Drawable , int , int , int , 
					int , int , Pixel , Pixel );
extern void X_DrawSimpleRawFrame(Display * , Drawable , int , int , int ,
					int , int , Pixel);
extern void X_DrawIcon(Display * , Drawable , Icon * , int , int );
extern void X_Draw3DArrow(Display * , Window , int , int , int , int ,
					int , Pixel , Pixel , Pixel);
extern void X_Draw3DRectangle(Display * , Drawable , int , int , int ,
					int , int , Pixel , Pixel , Pixel);
extern void X_Draw3DString(Display * , Drawable , XFontStruct * , int , int ,
					int , String , int , Pixel , Pixel);
extern void X_DrawTextCursor(Display * , Window , int , int , int , Pixel);
extern Icon * X_XpmDataToIcon(Display * , char **);
extern void X_GetWindowRootPosition(Display * , Window , int * , int *);
extern void Xt_SetInsensitive(Widget);
extern Widget Xt_GetShell(Widget);
extern Boolean Xt_IsUp(Widget);

/* Original code */
/* makro na vypocet a alokaciu bledsej farby */
#define LightColor(intensity,fromcolor,tocolor) \
	tocolor.red = (fromcolor.red + intensity) <= 65280 ? \
	fromcolor.red + intensity : 65280 ; \
	tocolor.green = (fromcolor.green + intensity) <= 65280 ? \
	fromcolor.green + intensity : 65280 ; \
	tocolor.blue = (fromcolor.blue + intensity) <= 65280 ? \
	fromcolor.blue + intensity : 65280; \
	if (!XAllocColor(dpy, DefaultColormap(dpy ,DefaultScreen(dpy)),&tocolor)) \
	{ \
		XtWarning("unable to allocate color for border"); \
		tocolor.pixel=WhitePixel(dpy,DefaultScreen(dpy)); \
	}

/* makro na vypocet a alokaciu tmavsej farby */
#define DarkColor(intensity,fromcolor,tocolor) \
	tocolor.red = (fromcolor.red - intensity) <= 0 ? \
	0 : fromcolor.red - intensity  ; \
	tocolor.green = (fromcolor.green - intensity) <= 0 ? \
	0 : fromcolor.green - intensity ; \
	tocolor.blue = (fromcolor.blue - intensity) <= 0 ? \
	0 : fromcolor.blue - intensity ; \
	if (!XAllocColor(dpy, DefaultColormap(dpy ,DefaultScreen(dpy)),&tocolor)) \
	{ \
		XtWarning("unable to allocate color for border"); \
		tocolor.pixel=BlackPixel(dpy,DefaultScreen(dpy)); \
	}

/* More compatible with e.g. Xaw3d */
extern Pixel AllocShadowPixel(Widget, int);	/* from xcommon/Gcs.h */
#define LightColor2(widget, tocolor, contrast) \
	tocolor.pixel = AllocShadowPixel(widget, 100+(contrast))
#define DarkColor2(widget, tocolor, contrast) \
	tocolor.pixel = AllocShadowPixel(widget, 100-(contrast))


#endif
/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#ifndef _misc_h_
#define _misc_h_

#define MAX_PATH_LEN   		2048
#define _DCtimeout		300
#define DefaultIntensity	10000

#if NeedFunctionPrototypes
	extern int cnt_lines(char *);
	extern int max_line_width(char * , XFontStruct *);
	extern char * get_abs_path(char *);
	extern int x_atoi(char *);
#else
	extern int cnt_lines();
	extern int max_line_width();
	extern char * get_abs_path();
	extern int x_atoi();
#endif

#ifndef MW_MAX
#define MW_MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

#ifndef MW_MIN
#define MW_MIN(x,y) ((x) < (y) ? (x) : (y))
#endif

#ifndef MW_BETWEEN
#define MW_BETWEEN(x,a,b) ((x)>=(a) && (x)<=(b))
#endif

#endif /* MW_NWS_H */
