/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#include <stdio.h>

#include <X11/IntrinsicP.h>
#include <X11/Xlib.h>
#include <stdlib.h>
#include <string.h>
#include <X11/ShellP.h>
#include "MwUtils.h"
#include "MwXutils.h"

#include "MwNws.h"
#include "MwBaseMEP.h"

#include "MwUtils.h"

static char stiple_bitmap[] = {0x01, 0x02};

/* Draws a 3D frame with an extra line inside if the environment
   variable XAWM_FRAME is set to "soft". The colour of the extra line
   is the average between the bevel colour and the background.
   In addition, draws the frame IN if thickness is negative and OUT
   if thickness is positive.
*/
void X_DrawSoft3DFrame(Display *dpy, Drawable win, int x, int y,
		int width, int height, int thickness,
		Pixel top_left_color, Pixel bottom_right_color, Pixel bg)
{
	char *theme = getenv("XAWM_THEME");
	int soft = (theme != NULL);
	unsigned long vm = 0;
	XGCValues v;
	XColor color1, color2;
	GC gc = XCreateGC(dpy, win, vm, &v);
	int red, green, blue;

	color1.pixel = bottom_right_color;
	color2.pixel = bg;
	MwQueryColor(dpy, None, &color1);
	MwQueryColor(dpy, None, &color2);
	red = color1.red+color2.red;
	green = color1.green+color2.green;
	blue = color1.blue+color2.blue;
	color1.red = red/2;
	color1.green = green/2;
	color1.blue = blue/2;
	MwAllocColor(dpy, None, &color1);
	XSetForeground(dpy, gc, color1.pixel);

	if (thickness < 0) {
		X_DrawSimple3DFrame(dpy, win, x, y, width, height,
			-thickness, bottom_right_color, top_left_color);
		if (soft) {
			int x1 = x-thickness;
			int x2 = x+width+thickness-1;
			int y1 = y-thickness;
			int y2 = y+height+thickness-1;
			XDrawLine(dpy, win, gc, x1, y2, x1, y1);
			XDrawLine(dpy, win, gc, x1, y1, x2, y1);
		}
	} else {
		X_DrawSimple3DFrame(dpy, win, x, y, width, height,
			thickness, top_left_color, bottom_right_color);
		if (soft) {
			int x1 = x+thickness+1;
			int x2 = x+width-thickness-1;
			int y1 = y+thickness+1;
			int y2 = y+height-thickness-1;
			XDrawLine(dpy, win, gc, x1, y2, x2, y2);
			XDrawLine(dpy, win, gc, x2, y2, x2, y1);
		}
	}

	XFreeGC(dpy, gc);
}

void 
X_DrawSimple3DFrame(dpy,win,x,y,width,height,thickness,top_left_color,bottom_right_color)
Display *dpy;
Drawable win;
int  x;
int  y;
int  width;
int  height;
int  thickness;
Pixel  top_left_color;
Pixel bottom_right_color;
{

	if (thickness == 0) return;
	else {
		XPoint top_leftPoints[6];

		XPoint bottom_rightPoints[6];

		GC gc;
		XGCValues gc_res;
		XtGCMask gc_mask;

		top_leftPoints[0].x = x;
		top_leftPoints[0].y = y;
		top_leftPoints[1].x = x + width;
		top_leftPoints[1].y = y;
		top_leftPoints[2].x = x + width - thickness;
		top_leftPoints[2].y = y + thickness;
		top_leftPoints[3].x = x + thickness;
		top_leftPoints[3].y = y + thickness;
		top_leftPoints[4].x = x + thickness;
		top_leftPoints[4].y = y + height - thickness;
		top_leftPoints[5].x = x;
		top_leftPoints[5].y = y + height;

		bottom_rightPoints[0].x = x + width;
		bottom_rightPoints[0].y = y + height;
		bottom_rightPoints[1].x = x;
		bottom_rightPoints[1].y = y + height;
		bottom_rightPoints[2].x = x + thickness;
		bottom_rightPoints[2].y = y + height - thickness;
		bottom_rightPoints[3].x = x + width - thickness;
		bottom_rightPoints[3].y = y + height - thickness;
		bottom_rightPoints[4].x = x + width - thickness;
		bottom_rightPoints[4].y = y + thickness;
		bottom_rightPoints[5].x = x + width;
		bottom_rightPoints[5].y = y;


		gc_res.foreground =  top_left_color;
		gc_mask = GCForeground ;
		gc = XCreateGC(dpy , win , gc_mask , &gc_res);
		
		XFillPolygon(dpy , win , gc , top_leftPoints , 
			XtNumber(top_leftPoints) , Nonconvex , CoordModeOrigin);

		XSetForeground(dpy , gc , bottom_right_color);

		XFillPolygon(dpy, win , gc , bottom_rightPoints , 
			XtNumber(bottom_rightPoints), Nonconvex, CoordModeOrigin);

		XFreeGC(dpy , gc);
	}
}


void X_DrawSimpleRawFrame(dpy,win,x,y,width,height,thickness,color)
Display *dpy;
Drawable win;
int  x;
int  y;
int  width;
int  height;
int  thickness;
Pixel  color;
{
	if (thickness == 0) return;
	else
	{
		XPoint points[10];
		
		GC gc;
		XGCValues gc_res;
		XtGCMask gc_mask;

		points[0].x = x;
		points[0].y = y;
		points[1].x = x + width;
		points[1].y = y;
		points[2].x = x + width;
		points[2].y = y  + height;
		points[3].x = x;
		points[3].y = y + height;
		points[4].x = x;
		points[4].y = y + thickness;
		points[5].x = x + thickness;
		points[5].y = y + thickness;
		points[6].x = x + thickness;
		points[6].y = y + height - thickness;
		points[7].x = x + width - thickness;
		points[7].y = y + height - thickness;
		points[8].x = x + width - thickness;
		points[8].y = y + thickness;
		points[9].x = x;
		points[9].y = y + thickness;

		gc_res.foreground =  color;
		gc_mask = GCForeground ;
		gc = XCreateGC(dpy , win , gc_mask , &gc_res);
		
		XFillPolygon(dpy , win , gc , points , 
			XtNumber(points) , Nonconvex , CoordModeOrigin);

		XFreeGC(dpy , gc);
	}
}

void X_DrawIcon(dpy,win,icon,x,y)
Display *dpy;
Drawable win;
Icon *icon;
int  x;
int  y;
{
	GC	gc;
	XGCValues gc_res;
	XtGCMask  gc_mask;
	
	if (icon == NULL) return;
	
	gc_res.fill_style = FillTiled;
	gc_res.fill_style = FillTiled;
	gc_res.tile = icon->pixmap;
	gc_res.clip_mask = icon->shape;
	gc_res.ts_x_origin = x;
	gc_res.ts_y_origin = y;
	gc_res.clip_x_origin = x;
	gc_res.clip_y_origin = y;

	gc_mask = GCTile | GCClipMask | GCFillStyle | GCTileStipXOrigin | 
		  GCTileStipYOrigin | GCClipXOrigin | GCClipYOrigin;

	gc = XCreateGC(dpy,DefaultRootWindow(dpy),gc_mask, &gc_res);

	XFillRectangle(dpy , win , gc , x , y , icon->width , icon->height);

	XFreeGC(dpy , gc);
}

void X_Draw3DArrow(dpy , win , x , y , width , height , type , top , bottom , normal)
Display * dpy;
Window win;
int  x;
int  y;
int  width;
int  height;
int  type;
Pixel top;
Pixel bottom;
Pixel normal;
{
	GC	gc;
	XGCValues gc_res;
	
	gc_res.foreground = normal;

	gc = XCreateGC(dpy , win , GCForeground, &gc_res );

	switch (type)
	{
	    case XtCtop:
		{
			XPoint poly[3];

			poly[0].x = x;
			poly[0].y = y + height;
			poly[1].x = x + width / 2;
			poly[1].y = y;
			poly[2].x = x + width;
			poly[2].y = y + height;

			XFillPolygon(dpy , win , gc , poly , XtNumber(poly) ,
				 Nonconvex , CoordModeOrigin);

			XSetForeground(dpy , gc , top);
			XDrawLine(dpy , win , gc , x , y + height , x + width / 2 , y);
			XSetForeground(dpy , gc , bottom);
			XDrawLine(dpy , win , gc , x , y + height , x + width ,
				 y + height);
			XDrawLine(dpy , win , gc , x + width , y + height ,
				x+ width / 2 , y);
		}
		break;
	    case XtCbottom:
		{
			XPoint poly[3];

			poly[0].x = x;
			poly[0].y = y;
			poly[1].x = x + width / 2;
			poly[1].y = y + height;
			poly[2].x = x + width;
			poly[2].y = y;

			XFillPolygon(dpy , win , gc , poly , XtNumber(poly) ,
				 Nonconvex , CoordModeOrigin);

			XSetForeground(dpy , gc , top);
			XDrawLine(dpy , win , gc , x , y  , x + width , y);
			XDrawLine(dpy , win , gc , x , y , x + width / 2 ,
				 y + height);
			XSetForeground(dpy , gc , bottom);
			XDrawLine(dpy , win , gc , x + width , y ,
				x + width / 2 , y + height);

		}
		break;
	    case XtCright:
		{
			XPoint poly[3];

			poly[0].x = x;
			poly[0].y = y;
			poly[1].x = x + width;
			poly[1].y = y + height / 2;
			poly[2].x = x;
			poly[2].y = y + height;

			XFillPolygon(dpy , win , gc , poly , XtNumber(poly) ,
				 Nonconvex , CoordModeOrigin);

			XSetForeground(dpy , gc , top);
			XDrawLine(dpy , win , gc , x , y , x , y + height);
			XDrawLine(dpy , win , gc , x , y , x + width ,
				 y + height / 2);
			XSetForeground(dpy , gc , bottom);
			XDrawLine(dpy , win , gc , x + width , y + height / 2 ,
				x , y + height);

		}
		break;
	    case XtCleft:
	    default:
		{

			XPoint poly[3];

			poly[0].x = x + width;
			poly[0].y = y;
			poly[1].x = x;
			poly[1].y = y + height / 2;
			poly[2].x = x + width;
			poly[2].y = y + height;

			XFillPolygon(dpy , win , gc , poly , XtNumber(poly) ,
				 Nonconvex , CoordModeOrigin);

			XSetForeground(dpy , gc , bottom);
			XDrawLine(dpy , win , gc , x , y + height / 2 ,
				x + width , y + height);
			XDrawLine(dpy , win , gc , x + width  , y , x + width ,
				 y + height );
			XSetForeground(dpy , gc , top);
			XDrawLine(dpy , win , gc , x , y + height / 2 ,
				x + width , y );
		}
	}
	XFreeGC(dpy , gc);
}

void X_Draw3DRectangle(dpy,win,x,y,width,height,thickness,top,bottom,normal)
Display *dpy;
Drawable win;
int  x;
int  y;
int  width;
int  height;
int  thickness;
Pixel  top;
Pixel bottom;
Pixel normal;
{
	GC	gc;
	XGCValues gc_res;
	
	gc_res.foreground = normal;

	gc = XCreateGC(dpy , win , GCForeground, &gc_res );

	XFillRectangle(dpy , win , gc , x + thickness , y + thickness ,
		width - 2 * thickness , height - 2 * thickness);

	XFreeGC(dpy , gc);
	
	X_DrawSimple3DFrame(dpy , win , x , y , width , height , thickness ,
		top , bottom);
		
}

void X_Draw3DString(dpy,win,font,x,y,offset,text,length,normal,shadow)
Display *dpy;
Drawable win;
XFontStruct *font;
int  x;
int  y;
int offset;
String text;
int length;
Pixel normal;
Pixel shadow;
{
	GC	gc;
	XGCValues gc_res;
	
	gc_res.foreground = shadow;
	gc_res.font = font->fid;
	gc = XCreateGC(dpy , win , GCForeground | GCFont , &gc_res );

	XDrawString(dpy , win , gc , x + offset , y + offset , text , length);

	XSetForeground(dpy , gc , normal);

	XDrawString(dpy , win , gc , x , y , text , length);

	XFreeGC(dpy , gc);
}

void X_DrawTextCursor(dpy , win , x , y , height , color)
Display *dpy;
Window win;
int x;
int y;
int height;
Pixel color;
{
	GC	gc;
	XGCValues gc_res;

	gc_res.foreground = color;

	gc = XCreateGC(dpy , win , GCForeground , &gc_res );

	XDrawLine(dpy , win , gc , x , y , x , y + height);
	XDrawLine(dpy, win , gc , x - 2 , y , x + 2 , y);
	XDrawLine(dpy , win , gc, x - 2 , y + height , x + 2 ,y + height);

	XFreeGC(dpy , gc);

}

Icon *X_XpmDataToIcon(dpy , data)
Display *dpy;
char **data;
{
        int status;
        Icon *icon = MwMalloc(sizeof(Icon));
        unsigned int b_width,depth;
        int x,y;

        status = XpmCreatePixmapFromData(dpy , DefaultRootWindow(dpy) , data ,
                        &icon->pixmap , &icon->shape , NULL);

        switch (status) {
                case XpmOpenFailed:
                case XpmFileInvalid:
                case XpmNoMemory:
			XtWarning("Unable to create icon from data");
                        break;

                case XpmColorError:
                case XpmColorFailed:
                        XtWarning("Could not get (all) colors for icon");
                case XpmSuccess:
                	if (icon->pixmap)
	                        status=XGetGeometry(dpy , icon->pixmap , &DefaultRootWindow(dpy) ,
                                        &x , &y , &icon->width , &icon->height ,
					&b_width , &depth);
        		else
        		{
        			free(icon);
        			icon = NULL;
				XtWarning("Unable to create icon from data");
        		}
	}

	return icon;
}

Region X_ClipWindowByParent(Display *dpy , Window win)
{
	XPoint clipb[4];
	unsigned int pw,ph,bw,dpt,ww,wh;
	int wx,wy,px,py;
        Window root , parent , *childs ;
        unsigned int nchilds;

	XQueryTree(dpy , win , &root , &parent , &childs , &nchilds);
	if (childs) MwFree(childs);

	XGetGeometry(dpy , parent , &root , &px , &py , &pw , &ph , &bw , &dpt); 
	XGetGeometry(dpy , win , &root , &wx , &wy , &ww , &wh , &bw , &dpt); 

	clipb[0].x = 0;
	clipb[0].y = (wy < 0) ? -wy : 0;
	clipb[1].x = ww;
	clipb[1].y = (wy < 0) ? -wy : 0;
	clipb[2].x = 0;
	clipb[2].y = (wy + wh > ph) ? ph - wy : wh;
	clipb[1].x = ww;
	clipb[2].y = (wy + wh > ph) ? ph - wy : wh;
		
	return XPolygonRegion(clipb , 4 , EvenOddRule);
}

void X_GetWindowRootPosition(dpy , win , x , y)
Display *dpy;
Window win;
int *x;
int *y;
{
        Window root , parent , *childs ;
        unsigned int nchilds;
	unsigned int bw,dpt,ww,wh;
	int wx,wy;
	int rx = 0 , ry = 0;
	
	XQueryTree(dpy , win , &root , &parent , &childs , &nchilds);
	MwFree(childs);

	while(win != root)
	{
		XGetGeometry(dpy , win , &root , &wx , &wy , &ww , &wh , &bw , &dpt); 
		rx += wx;
		ry +=wy;
		XQueryTree(dpy , win , &root , &parent , &childs , &nchilds);
		MwFree(childs);
		win = parent;
	}

	*x = rx;
	*y = ry;
}

void Xt_SetInsensitive(w)
Widget w;
{
	Pixmap p;
	GC gc;
	XGCValues gc_res;
	Display *dpy;
	Window win;


	if (!XtIsRealized(w)) return;

	if (XtIsSubclass(w , coreWidgetClass))
	{
		dpy = XtDisplay(w);
		win = XtWindow(w);
	}
	else
	{
		dpy = XtDisplayOfObject(w);
		win = XtWindowOfObject(w);
	}

	p = XCreateBitmapFromData(dpy,win,stiple_bitmap,2,2);
		
	if (XtIsSubclass(w , coreWidgetClass))
	{
		gc_res.foreground = w->core.background_pixel;
	}
	else
	{
		gc_res.foreground = ((MwBaseMEObject) w)->baseME.background;
	}
	
	gc_res.fill_style = FillStippled;
	gc_res.stipple = p;

	gc = XCreateGC(dpy , win , 
		GCForeground | GCStipple | GCFillStyle , &gc_res );

	if (XtIsSubclass(w , coreWidgetClass))
	{
		XFillRectangle(dpy , win , gc , 0 , 0 , 
			w->core.width , w->core.height);
	}
	else
	{
		XFillRectangle(dpy , win , gc , w->core.x , w->core.y ,
			w->core.width , w->core.height);
	}

	XFreeGC(dpy , gc);
	XFreePixmap(dpy , p);
}

Widget Xt_GetShell(w)
Widget w;
{
	Widget pw;

	pw = w;

	while(pw && !XtIsShell(pw)) pw = XtParent(pw);

	return pw;
}

Boolean Xt_IsUp(w)
Widget w;
{
	ShellWidget sw = (ShellWidget) Xt_GetShell(w);

	return sw ?  sw->shell.popped_up : False;
}
/* $XConsortium: Vendor.c,v 1.21 91/07/30 15:29:56 rws Exp $ */

/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/*
 * This is a copy of Xt/Vendor.c with an additional ClassInitialize
 * procedure to register Xmu resource type converters.
 *
 */

/* Make sure all wm properties can make it out of the resource manager */

/* I stole this from Xaw sources */

#include <stdio.h>

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/ShellP.h>
#include <X11/Vendor.h>
#include <X11/VendorP.h>
#include <X11/Xmu/Converters.h>
#include <X11/Xmu/Editres.h>

static XtResource resources[] = {
  {XtNinput, XtCInput, XtRBool, sizeof(Bool),
		XtOffsetOf(VendorShellRec, wm.wm_hints.input),
		XtRImmediate, (XtPointer)True}
};

/***************************************************************************
 *
 * Vendor shell class record
 *
 ***************************************************************************/

static void VendorShellClassInitialize();
static void VendorShellInitialize();
static void ChangeManaged();
static XtGeometryResult QueryGeometry();

#define SuperClass (&wmShellClassRec)
externaldef(vendorshellclassrec) VendorShellClassRec vendorShellClassRec = {
  {
    /* superclass	  */	(WidgetClass)SuperClass,
    /* class_name	  */	"VendorShell",
    /* size		  */	sizeof(VendorShellRec),
    /* class_initialize	  */	VendorShellClassInitialize,
    /* class_part_initialize*/	NULL,
    /* Class init'ed ?	  */	FALSE,
    /* initialize	  */	VendorShellInitialize,
    /* initialize_hook	  */	NULL,		
    /* realize		  */	XtInheritRealize,
    /* actions		  */	NULL,
    /* num_actions	  */	0,
    /* resources	  */	resources,
    /* resource_count	  */	XtNumber(resources),
    /* xrm_class	  */	NULLQUARK,
    /* compress_motion	  */	FALSE,
    /* compress_exposure  */	TRUE,
    /* compress_enterleave*/	FALSE,
    /* visible_interest	  */	FALSE,
    /* destroy		  */	NULL,
    /* resize		  */	XtInheritResize,
    /* expose		  */	NULL,
    /* set_values	  */	NULL,
    /* set_values_hook	  */	NULL,			
    /* set_values_almost  */	XtInheritSetValuesAlmost,  
    /* get_values_hook	  */	NULL,
    /* accept_focus	  */	NULL,
    /* intrinsics version */	XtVersion,
    /* callback offsets	  */	NULL,
    /* tm_table		  */	NULL,
    /* query_geometry	  */	QueryGeometry,
    /* display_accelerator*/	NULL,
    /* extension	  */	NULL
  },{
    /* geometry_manager	  */	XtInheritGeometryManager,
    /* change_managed	  */	ChangeManaged,
    /* insert_child	  */	XtInheritInsertChild,
    /* delete_child	  */	XtInheritDeleteChild,
    /* extension	  */	NULL
  },{
    /* extension	  */	NULL
  },{
    /* extension	  */	NULL
  },{
    /* extension	  */	NULL
  }
};

externaldef(vendorshellwidgetclass) WidgetClass vendorShellWidgetClass =
	(WidgetClass) (&vendorShellClassRec);

static void VendorShellClassInitialize()
{
    static XtConvertArgRec screenConvertArg[] = {
        {XtWidgetBaseOffset, (XtPointer) XtOffsetOf(WidgetRec, core.screen),
	     sizeof(Screen *)}
    };

    XtAddConverter(XtRString, XtRCursor, XmuCvtStringToCursor,      
		   screenConvertArg, XtNumber(screenConvertArg));

    XtAddConverter(XtRString, XtRBitmap, XmuCvtStringToBitmap,
		   screenConvertArg, XtNumber(screenConvertArg));
}

/* ARGSUSED */
static void VendorShellInitialize(req, new)
	Widget req, new;
{
    XtAddEventHandler(new, (EventMask) 0, TRUE, _XEditResCheckMessages, NULL);
}

static void ChangeManaged(wid)
Widget wid;
{
	ShellWidget w = (ShellWidget) wid;
	Widget* childP;
	int i;
	XtWidgetGeometry intended , preferred;

	(*SuperClass->composite_class.change_managed)(wid);
	for (i = w->composite.num_children, childP = w->composite.children;
	     i; i--, childP++) {
	    if (XtIsManaged(*childP)) {
		XtSetKeyboardFocus(wid, *childP);
		break;
	    }
	}

	if (w->composite.num_children)
	{
	        preferred.request_mode = CWWidth | CWHeight;
        	preferred.width = w->core.width;
	        preferred.height = w->core.height;

	        intended.request_mode = CWWidth | CWHeight;
        	intended.width = w->core.width;
	        intended.height = w->core.height;

		XtQueryGeometry(w->composite.children[0] , &intended , &preferred);

		if (!preferred.width) preferred.width = 30;
		if (!preferred.height) preferred.height = 30;

		XtResizeWidget(wid , preferred.width , preferred.height , w->core.border_width);
	}
}

static XtGeometryResult QueryGeometry(w, intended , preferred)
Widget w;
XtWidgetGeometry *intended;
XtWidgetGeometry *preferred;
{
	ShellWidget cw = (ShellWidget)w;

	if (cw->composite.num_children)
	{
		XtQueryGeometry(cw->composite.children[0] , intended , preferred);
	}
	return XtGeometryAlmost;
}

void _InitializeWidgetSet ()
{
    static int firsttime = 1;

    if (firsttime) {
        firsttime = 0;
        XtInitializeWidgetClass (vendorShellWidgetClass);
    }
}
/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#include <X11/Intrinsic.h>
#include <X11/Xmu/CharSet.h>
#include <stdlib.h>
#include <ctype.h>
#include "MwUtils.h"
#include "MwNws.h"

Boolean  cvtStringToBoxType (display,args,num_args,from,to,converter_data)
Display * display;
XrmValuePtr  args;
Cardinal * num_args;
XrmValuePtr  from;
XrmValuePtr  to;
XtPointer * converter_data;
{
	int a = 0;
	char c, *t, *s = (char*) from->addr;

	if (*num_args != 0)
		XtAppErrorMsg(XtDisplayToApplicationContext(display),
			"cvtStringToBoxType", "wrongParameters",
			"XtToolkitError",
			"String to BoxType conversion needs no arguments",
			(String*) NULL, (Cardinal*) NULL);

	while (*s) {
		/* vynechaj medzery */
		for (; isspace(*s); s++) ;
		for (t = s; *t && ! isspace(*t); t++) ;
		c = *t;
		*t = '\0';
		if (XmuCompareISOLatin1(s, "no") == 0) a = XtCno_box;
		else if (XmuCompareISOLatin1(s, "simple") == 0) a = XtCsimple_box;
		else if (XmuCompareISOLatin1(s, "up") == 0) a = XtCup_box;
		else if (XmuCompareISOLatin1(s, "down") == 0) a = XtCdown_box;
		else if (XmuCompareISOLatin1(s, "framein") == 0) a = XtCframein_box;
		else if (XmuCompareISOLatin1(s, "frameout") == 0) a = XtCframeout_box;
		else if (XmuCompareISOLatin1(s, "shadow") == 0) a = XtCshadow_box;
		
		else {
			XtDisplayStringConversionWarning(display, (char*) from->addr, 
							XtRBox_type);
		break;
		}
		*t = c;
		s = t;
	}
	MwNwsDone(int, a);
}

Boolean  cvtBoxTypeToString(display,args,num_args,from,to,converter_data)
Display * display;
XrmValuePtr  args;
Cardinal * num_args;
XrmValuePtr  from;
XrmValuePtr  to;
XtPointer * converter_data;
{
	int *a = (int*) from->addr;

	if (*num_args != 0)
		XtAppErrorMsg(XtDisplayToApplicationContext(display),
			"cvtBoxTypeToString", "wrongParameters",
			"XtToolkitError",
			"BoxType to String conversion needs no arguments",
			(String*) NULL, (Cardinal*) NULL);
	switch (*a) {
		case XtCno_box: MwNwsDone(String, "no");
		case XtCsimple_box: MwNwsDone(String , "simple");
		case XtCup_box: MwNwsDone(String, "up");
		case XtCdown_box: MwNwsDone(String, "down");
		case XtCframein_box: MwNwsDone(String, "framein");
		case XtCframeout_box: MwNwsDone(String, "frameout");
		case XtCshadow_box: MwNwsDone(String , "shadow");
		default: MwNwsDone(String, "unknown");
	}
}

Boolean  cvtStringToJustify (display,args,num_args,from,to,converter_data)
Display * display;
XrmValuePtr  args;
Cardinal * num_args;
XrmValuePtr  from;
XrmValuePtr  to;
XtPointer * converter_data;
{
	int a = 0;
	char c, *t, *s = (char*) from->addr;


	if (*num_args != 0)
		XtAppErrorMsg(XtDisplayToApplicationContext(display),
			"cvtStringToJustify", "wrongParameters",
			"XtToolkitError",
			"String to Justify conversion needs no arguments",
			(String*) NULL, (Cardinal*) NULL);

	while (*s) {
		/* vynechaj medzery */
		for (; isspace(*s); s++) ;
		for (t = s; *t && ! isspace(*t); t++) ;
		c = *t;
		*t = '\0';
		if (XmuCompareISOLatin1(s, "center") == 0) a = XtCcenter;
		else if (XmuCompareISOLatin1(s, "left") == 0) a = XtCleft;
		else if (XmuCompareISOLatin1(s, "right") == 0) a = XtCright;
		else if (XmuCompareISOLatin1(s, "bottom") == 0) a = XtCbottom;
		else if (XmuCompareISOLatin1(s, "top") == 0) a = XtCtop;
		else {
			XtDisplayStringConversionWarning(display, (char*) from->addr, 
							XtRNwsJustify);
			break;
		}
		*t = c;
		s = t;
	}
	MwNwsDone(int, a);
}

Boolean  cvtStringToIcon(dpy,args,num_args,from,to,converter_data)
Display * dpy;
XrmValuePtr  args;
Cardinal * num_args;
XrmValuePtr  from;
XrmValuePtr  to;
XtPointer * converter_data;
{
	Icon *icon = MwMalloc(sizeof(Icon));
	char *s = (char*) from->addr;
	char *name ,  *t;
	int status;
	Cardinal n_arg = 1;
  	unsigned int b_width,depth;
  	int x,y;

	for (; isspace(*s); s++) ;
	for (t = s; *t && ! isspace(*t); t++) ;
	*t = '\0';
	name = s;

	status = XpmReadFileToPixmap(dpy , DefaultRootWindow(dpy) , name ,
			&icon->pixmap , &icon->shape , NULL);

	switch (status) {
		case XpmOpenFailed:
		case XpmFileInvalid:
		case XpmNoMemory:
			XtAppWarningMsg
				(XtDisplayToApplicationContext(dpy),
				 "cvtStringToPixmap", "fileError",
				 "XtToolkitError",
				 "Failed to read pixmap from \"%s\"",
				 &name, &n_arg);
			break;

		case XpmColorError:
		case XpmColorFailed:
			XtAppWarningMsg
				(XtDisplayToApplicationContext(dpy),
				 "cvtStringToPixmap", "allocColor",
				 "XtToolkitError",
				 "Could not get (all) colors for pixmap \"%s\"",
				 &name, &n_arg);
		case XpmSuccess:
			if (icon->pixmap)
				status=XGetGeometry(dpy , icon->pixmap , &DefaultRootWindow(dpy) ,
					&x , &y , &icon->width , &icon->height ,
					&b_width , &depth);
			else
			{
				MwFree(icon);
				icon = NULL;
				XtAppWarningMsg
					(XtDisplayToApplicationContext(dpy),
					 "cvtStringToPixmap", "allocColor",
					 "XtToolkitError",
					 "Could not create pixmap \"%s\"",
				 	 &name, &n_arg);
			}
			MwNwsDone(Icon*,icon);
	}
	return False;
}

Boolean  cvtStringToCheckType (display,args,num_args,from,to,converter_data)
Display * display;
XrmValuePtr  args;
Cardinal * num_args;
XrmValuePtr  from;
XrmValuePtr  to;
XtPointer * converter_data;
{
	int a = 0;
	char c, *t, *s = (char*) from->addr;

	if (*num_args != 0)
		XtAppErrorMsg(XtDisplayToApplicationContext(display),
			"cvtStringToCheckType", "wrongParameters",
			"XtToolkitError",
			"String to CheckType conversion needs no arguments",
			(String*) NULL, (Cardinal*) NULL);

	while (*s) {
		/* vynechaj medzery */
		for (; isspace(*s); s++) ;
		for (t = s; *t && ! isspace(*t); t++) ;
		c = *t;
		*t = '\0';
		if (XmuCompareISOLatin1(s, "check") == 0) a = XtCcheck;
		else if (XmuCompareISOLatin1(s, "rectangle") == 0) a = XtCrectangle;
		else if (XmuCompareISOLatin1(s, "diamond") == 0) a = XtCdiamond;
		else if (XmuCompareISOLatin1(s, "circle") == 0) a = XtCcircle;
		else if (XmuCompareISOLatin1(s, "cross") == 0) a = XtCcross;
		else if (XmuCompareISOLatin1(s, "circle2") == 0) a = XtCcircle2;
		else {
			XtDisplayStringConversionWarning(display, (char*) from->addr, 
							XtRCheck_type);
		break;
		}
		*t = c;
		s = t;
	}
	MwNwsDone(int, a);
}

Boolean  cvtStringToObjectType (display,args,num_args,from,to,converter_data)
Display * display;
XrmValuePtr  args;
Cardinal * num_args;
XrmValuePtr  from;
XrmValuePtr  to;
XtPointer * converter_data;
{
	int a = 0;
	char c, *t, *s = (char*) from->addr;

	if (*num_args != 0)
		XtAppErrorMsg(XtDisplayToApplicationContext(display),
			"cvtStringToObjectType", "wrongParameters",
			"XtToolkitError",
			"String to ObjectType conversion needs no arguments",
			(String*) NULL, (Cardinal*) NULL);

	while (*s) {
		/* vynechaj medzery */
		for (; isspace(*s); s++) ;
		for (t = s; *t && ! isspace(*t); t++) ;
		c = *t;
		*t = '\0';
		if (XmuCompareISOLatin1(s, "left_arrow") == 0) a = XtCleft_arrow;
		else if (XmuCompareISOLatin1(s, "right_arrow") == 0) a = XtCright_arrow;
		else if (XmuCompareISOLatin1(s, "top_arrow") == 0) a = XtCtop_arrow;
		else if (XmuCompareISOLatin1(s, "bottom_arrow") == 0) a = XtCbottom_arrow;
		else {
			XtDisplayStringConversionWarning(display, (char*) from->addr, 
							XtRObject_type);
		break;
		}
		*t = c;
		s = t;
	}
	MwNwsDone(int, a);
}

Boolean  cvtStringToOrientation (display,args,num_args,from,to,converter_data)
Display * display;
XrmValuePtr  args;
Cardinal * num_args;
XrmValuePtr  from;
XrmValuePtr  to;
XtPointer * converter_data;
{
	int a = 0;
	char c, *t, *s = (char*) from->addr;


	if (*num_args != 0)
		XtAppErrorMsg(XtDisplayToApplicationContext(display),
			"cvtStringToOrientation", "wrongParameters",
			"XtToolkitError",
			"String to Orientation conversion needs no arguments",
			(String*) NULL, (Cardinal*) NULL);

	while (*s) {
		/* vynechaj medzery */
		for (; isspace(*s); s++) ;
		for (t = s; *t && ! isspace(*t); t++) ;
		c = *t;
		*t = '\0';
		if (XmuCompareISOLatin1(s, "vertical") == 0) a = XtCvertical;
		else if (XmuCompareISOLatin1(s, "horizontal") == 0) a = XtChorizontal;
		else {
			XtDisplayStringConversionWarning(display, (char*) from->addr, 
							XtROrientation);
			break;
		}
		*t = c;
		s = t;
	}
	MwNwsDone(int, a);
}

Boolean  cvtStringToListStruct(display,args,num_args,from,to,converter_data)
Display * display;
XrmValuePtr  args;
Cardinal * num_args;
XrmValuePtr  from;
XrmValuePtr  to;
XtPointer * converter_data;
{
	ListStruct *list = NULL;
	Boolean koniec = 0;
	long len , cnt = 0;
	char *st , *en , *s = (char*) from->addr;

	if (*num_args != 0)
		XtAppErrorMsg(XtDisplayToApplicationContext(display),
			"cvtStringToListStruct", "wrongParameters",
			"XtToolkitError",
			"String to ListStruct conversion needs no arguments",
			(String*) NULL, (Cardinal*) NULL);

	while (!koniec)
	{
		cnt ++;

		st = strchr(s , '\"');
		st ++;
		en = strchr(st , '\"');
		len = en - st;

		list = MwRealloc((void *) list , cnt * sizeof(ListStruct));

		list[cnt - 1].label = MwMalloc(len + 1);
		strncpy(list[cnt - 1].label , st , len);
		*(list[cnt - 1].label + len) = '\0';
		list[cnt - 1].left_icon = None;
		list[cnt - 1].right_icon = None;
		list[cnt - 1].related_info = NULL;
		list[cnt - 1].index = cnt;

		s = en + 1;

		koniec = ( strchr(s , '\"') == NULL);
	}

	MwNwsDone(ListStruct * , list);
}

Boolean  cvtStringToButtonMode(display,args,num_args,from,to,converter_data)
Display * display;
XrmValuePtr  args;
Cardinal * num_args;
XrmValuePtr  from;
XrmValuePtr  to;
XtPointer * converter_data;
{
	int a = 0;
	char c, *t, *s = (char*) from->addr;

	if (*num_args != 0)
		XtAppErrorMsg(XtDisplayToApplicationContext(display),
			"cvtStringToButtonMode", "wrongParameters",
			"XtToolkitError",
			"String to ButtonMode conversion needs no arguments",
			(String*) NULL, (Cardinal*) NULL);

	while (*s) {
		/* vynechaj medzery */
		for (; isspace(*s); s++) ;
		for (t = s; *t && ! isspace(*t); t++) ;
		c = *t;
		*t = '\0';
		if (XmuCompareISOLatin1(s, "normal") == 0) a = XtCnormalMode;
		else if (XmuCompareISOLatin1(s, "cyclic") == 0) a = XtCcyclicMode;
		else if (XmuCompareISOLatin1(s, "toggle") == 0) a = XtCtoggleMode;
		else {
			XtDisplayStringConversionWarning(display, (char*) from->addr, 
							XtRObject_type);
		break;
		}
		*t = c;
		s = t;
	}
	MwNwsDone(int, a);
}

Boolean  cvtStringToStringList(display,args,num_args,from,to,converter_data)
Display * display;
XrmValuePtr  args;
Cardinal * num_args;
XrmValuePtr  from;
XrmValuePtr  to;
XtPointer * converter_data;
{
	char **list;
	Boolean koniec = 0;
	long len , cnt = 0;
	char *st , *en , *s = (char*) from->addr;

	list = MwMalloc(sizeof(char *));
	list[0] = NULL;

	if (*num_args != 0)
		XtAppErrorMsg(XtDisplayToApplicationContext(display),
			"cvtStringToStringList", "wrongParameters",
			"XtToolkitError",
			"String to StringList conversion needs no arguments",
			(String*) NULL, (Cardinal*) NULL);

	while (!koniec)
	{
		cnt ++;

		st = strchr(s , '\"');
		st ++;
		en = strchr(st , '\"');
		len = en - st;

		list = MwRealloc((void *) list , (cnt + 1) * sizeof(char *));

		list[cnt - 1] = MwMalloc(len + 1);
		strncpy(list[cnt - 1] , st , len);
		*(list[cnt - 1] + len) = '\0';
		list[cnt] = NULL;

		s = en + 1;

		koniec = (strchr(s , '\"') == NULL);
	}

	MwNwsDone(char ** , list);
}
/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#include <X11/Intrinsic.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>

int cnt_lines(in_text)
char *in_text;
{
	char *p,*s1,*s;
	int i=0;
	
	if (in_text)
	{
		p=s=s1=MwMalloc(strlen(in_text)+1);
		strcpy(s,in_text);
		for(i=1; ;i++)
		{
			p=strchr(s,'\n');
			if (p==NULL) break;
			s=++p;
		}
		MwFree(s1);
	}
	
	return i;
}

int max_line_width(in_text,font)
char *in_text;
XFontStruct *font;
{
	char *p,*s1,*s;
	int i=0,maxw=0,pom;
	
	if (in_text)
	{
		p=s=s1=MwMalloc(strlen(in_text)+1);
		strcpy(s,in_text);
		for(i=1; ;i++)
		{
			p=strchr(s,'\n');
			if (p!=NULL) *p=0;

			pom = XTextWidth(font,s,strlen(s));

			if (pom > maxw) maxw = pom;
			if (p==NULL) break;
			s=++p;
		}
		MwFree(s1);
	}
	return maxw;
}

char *get_abs_path(path)
char *path;
{
	char *p,pom[MAX_PATH_LEN],*tmp,result[MAX_PATH_LEN]="/";
	
	

	for(p = path; isspace(*p) && *p ; p++);

	if (! (*p == '/'))
	{
		tmp = (char *) getcwd(NULL,MAX_PATH_LEN);
		sprintf(pom , "%s/%s" , tmp ,p);
	}
	else
	{
		sprintf(pom , "%s" , p);
	}

	p = strtok(pom , "/");
	
	while(p)
	{
		if (strcmp(p , "."))
		{
			if (strcmp(p , ".."))
			{
				if (result[strlen(result) - 1] != '/') strcat(result , "/");
				strcat(result , p);
			}
			else
			{
				tmp = strrchr(result , '/');
				*(tmp + 1 - (tmp != result) ) = '\0';
			}
		}
		p = strtok(NULL , "/");
	}
	if (result[strlen(result) - 1] != '/') strcat(result , "/");

	tmp = MwMalloc(strlen(result) + 1);
	strcpy(tmp , result);

	return tmp;
}

int x_atoi(str) 
char *str;
{
        char *__eptr__;
        int rv;

        rv = strtol(str , (char **) &__eptr__ , 10);
        if (*__eptr__ != '\0') errno = ERANGE;
        else errno = 0;

        return rv;
}
