/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xlib.h>
#include <X11/xpm.h>
#include "MwUtils.h"
#include "MwNws.h"

#include "MwButtonP.h"

#define offset(field) XtOffsetOf(MwButtonRec,button.field)

static XtResource resources [] = {
	{
	 XtNicon ,
	 XtCIcon ,
	 XtRIcon ,
	 sizeof(Icon*) ,
	 offset(icon) ,
	 XtRImmediate ,
	 (XtPointer) NULL
	},
	{
	 XtNlabel ,
	 XtCLabel ,
	 XtRString ,
	 sizeof(char *) ,
	 offset(label) ,
	 XtRString ,
	 (XtPointer) NULL
	},
	{
	 XtNicon_justify ,
	 XtCIcon_justify ,
	 XtRNwsJustify ,
	 sizeof(int) ,
	 offset(icon_justify) ,
	 XtRImmediate ,
	 (XtPointer) XtCcenter
	},
	{
	 XtNlabel_justify ,
	 XtCLabel_justify ,
	 XtRNwsJustify ,
	 sizeof(int) ,
	 offset(label_justify) ,
	 XtRImmediate ,
	 (XtPointer) XtCtop
	},
	{
	 XtNfont ,
	 XtCFont ,
	 XtRFontStruct ,
	 sizeof(XFontStruct *) ,
	 offset(font) ,
	 XtRString ,
	 (XtPointer) XtDefaultFont
	},
	{
	 XtNspacing ,
	 XtCSpacing ,
	 XtRInt ,
	 sizeof(int) ,
	 offset(spacing) ,
	 XtRImmediate ,
	 (XtPointer) 5
	},
};


static char trans_tab [] =
	"\
 	 <FocusIn>: focusIn()\n\
	 <FocusOut>: focusOut()\n\
	 ~Shift<Key>Tab: traverseForward()\n\
	 Shift<Key>Tab: traverseBackward()\n\
	 <KeyUp>Return: KBactivate()\n\
	 <KeyUp>space: KBactivate()\n\
	 <Btn1Down>: hide_help() activate() \n\
	 <Btn1Up>: deactivate() \n\
	 <Enter>: enter_leave() highlight() show_help() \n\
	 <Leave>: enter_leave() unhighlight() hide_help() \n\
	 <BtnDown>: hide_help() \n\
	 <KeyDown>: hide_help() \n\
	 ";


 
static void Class_initialize (void);
static void Initialize (Widget, Widget, ArgList, Cardinal *);
static void Destroy (Widget);
static void Redisplay (Widget, XEvent *, Region);
static Boolean SetValues (Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult QueryGeometry(Widget,
		XtWidgetGeometry *, XtWidgetGeometry *);

static void compute_dim(MwButtonWidget, Dimension *, Dimension *);
static void compute_pos(MwButtonWidget, Dimension, Dimension,
		int *, int *, int *, int *);

MwButtonClassRec mwButtonClassRec = {
/* core */
   {
    /* superclass            */ (WidgetClass) &mwSButtonClassRec,
    /* class_name            */ "MwButton",
    /* widget_size           */ sizeof(MwButtonRec),
    /* class_initialize      */ Class_initialize,
    /* class_part_initialize */ NULL,
    /* class_inited          */ FALSE,
    /* initialize            */ (XtInitProc) Initialize,
    /* initialize_hook       */ NULL,
    /* realize               */ XtInheritRealize,
    /* actions               */ NULL,
    /* num_actions           */ 0,
    /* resources             */ resources,
    /* num_resources         */ XtNumber(resources),
    /* xrm_class             */ NULLQUARK,
    /* compress_motion       */ TRUE,
    /* compress_exposure     */ TRUE,
    /* compress_enterleave   */ TRUE,
    /* visible_interest      */ FALSE,
    /* destroy               */ (XtWidgetProc) Destroy,
    /* resize                */ XtInheritResize,
    /* expose                */ (XtExposeProc) Redisplay,
    /* set_values            */ (XtSetValuesFunc) SetValues,
    /* set_values_hook       */ NULL,
    /* set_values_almost     */ XtInheritSetValuesAlmost,
    /* get_values_hook       */ NULL,
    /* accept_focus          */ XtInheritAcceptFocus,
    /* version               */ XtVersion,
    /* callback_private      */ NULL,
    /* tm_table              */ trans_tab,
    /* query_geometry        */ QueryGeometry,
    /* display_accelerator   */ XtInheritDisplayAccelerator,
    /* extension             */ NULL
   },
/* base */
   {
    /* get_internal_dimension  */ XtInheritGetInternalDimension,
    /* set_internal_dimension  */ XtInheritSetInternalDimension,
    /* highlight	       */ XtInheritHighlight,
    /* unhighlight	       */ XtInheritUnhighlight ,
    /* highlightBorder	       */ XtInheritHighlightBorder,
    /* unhighlightBorder       */ XtInheritUnhighlightBorder,
  },
/* sButton */
   {
    /* empty                 */ 0
   },
/* button */
   {
    /* empty 		     */ 0
   },
};

WidgetClass mwButtonWidgetClass = (WidgetClass) & mwButtonClassRec;

static void Class_initialize(void)
{
	_InitializeWidgetSet();

	XtSetTypeConverter(XtRString ,XtRIcon ,cvtStringToIcon ,
		NULL ,0 ,XtCacheNone ,NULL);

	XtSetTypeConverter(XtRString , XtRNwsJustify , cvtStringToJustify ,
		NULL , 0 , XtCacheNone , NULL);
}

static void Initialize(Widget req_widget, Widget new_widget,
		ArgList args, Cardinal *num_args)
{
	Display *dpy=XtDisplay(new_widget);
	MwButtonWidget nw = (MwButtonWidget) new_widget;
	Dimension width , height;
	XGCValues gc_res;
	XtGCMask  gc_mask;

	gc_res.foreground = nw->base.foreground;
	gc_res.background = nw->core.background_pixel;
	gc_res.font = nw->button.font->fid;
	gc_mask = GCForeground | GCBackground | GCFont;

	nw->button.gc = XCreateGC(dpy,DefaultRootWindow(dpy),
			gc_mask, &gc_res);

	if (nw->button.label) nw->button.label = MwStrdup(nw->button.label);

	compute_dim(nw , &width , &height);

	if (nw->core.width) width = nw->core.width;
	if (nw->core.height) height = nw->core.height;

	mwButtonClassRec.base_class.set_internal_dimension(new_widget , width , height);

}

static void Destroy(Widget w)
{
	MwButtonWidget cw = (MwButtonWidget) w;
	
	if (cw->button.icon)
	{
		XFreePixmap(XtDisplay(w) , cw->button.icon->pixmap);
		if (cw->button.icon->shape != None)
			XFreePixmap(XtDisplay(w) , cw->button.icon->shape);
	}
	if (cw->button.label)
		MwFree(cw->button.label);
}

static void Redisplay(Widget w , XEvent *event , Region region)
{
	MwButtonWidget cw = (MwButtonWidget) w;
	Display *dpy = XtDisplay(w);
	Window win = XtWindow(w);
	Dimension width , height;
	Position x,y;
	int icon_x,icon_y,text_x,text_y;
	
	((MwButtonWidgetClass)w->core.widget_class)->base_class.
		get_internal_dimension(w , &x , &y , &width , &height);

	compute_pos(cw , width , height , &text_x , &text_y , &icon_x , &icon_y);

	if (cw->button.icon) 
	{
		X_DrawIcon(dpy , win , cw->button.icon , x + icon_x , y + icon_y);
	}

	if (cw->button.label)
	{
		int i;
		char *s1,*s,*p;
		int line_width,max_width;

		if (cw->base.entered)
			XSetForeground(dpy , cw->button.gc , cw->base.activ_fg);
		else
			XSetForeground(dpy , cw->button.gc , cw->base.foreground);

		s1=s=p=MwMalloc(strlen(cw->button.label)+1);
		strcpy(s,cw->button.label);
		max_width = max_line_width(cw->button.label , cw->button.font);
		for(i = 1;; i++)
		{
			p=strchr(s,'\n');
			if (p!=NULL) *p=0;
			line_width = XTextWidth(cw->button.font , s , strlen(s));
			
			XDrawString(dpy , win , cw->button.gc , x + text_x +
				 (max_width - line_width) / 2,
				 y + text_y + cw->button.font->max_bounds.ascent +
				 (i - 1) * (cw->button.font->max_bounds.ascent +
				 cw->button.font->max_bounds.descent), s ,strlen(s));
/*
			X_DrawString(dpy , win ,cw->button.font, x + text_x +
				 (max_width - line_width) / 2,
				 y + text_y + cw->button.font->max_bounds.ascent +
				 (i - 1) * (cw->button.font->max_bounds.ascent +
				 cw->button.font->max_bounds.descent),2, s ,strlen(s),
				 cw->base.foreground,cw->base.dark);
*/
			if (p==NULL) break;
			s = p + 1;
		}
		MwFree(s1);
	}

	mwBaseClassRec.core_class.expose(w , event , region);
}

#define WidgetValuesDiffer(w1,w2,component) (w1 -> button.component != \
                                             w2 -> button.component)


static Boolean SetValues(Widget current, Widget request, Widget new_widget,
		ArgList args, Cardinal *num_args)
{
	MwButtonWidget nw = (MwButtonWidget) new_widget;
	MwButtonWidget cw = (MwButtonWidget) current;
	Boolean redraw = False;

	if WidgetValuesDiffer(nw , cw , font)
	{

		XSetFont(XtDisplay(new_widget) , nw->button.gc ,
			nw->button.font->fid);

		redraw = True;
	}

	if (nw->base.foreground != cw->base.foreground)
	{
		XSetForeground(XtDisplay(new_widget) , nw->button.gc ,
			nw->base.foreground);

		redraw = True;
	}

	if (nw->button.label != cw->button.label)
	{
		if (nw->button.label) 
			nw->button.label = MwStrdup(nw->button.label);
		if (cw->button.label) 
		{
			MwFree(cw->button.label);
			cw->button.label = NULL;
		}

		redraw = True;
	}

	if WidgetValuesDiffer(nw , cw , icon)
		redraw = True;

	return redraw;
}

#undef  WidgetValuesDiffer 

static XtGeometryResult QueryGeometry(Widget w,
		XtWidgetGeometry *intended , XtWidgetGeometry *preferred)
{
	MwButtonWidget cw = (MwButtonWidget) w;
	Dimension pwidth , pheight;
	Dimension width , height;
	Position x,y;
	
	mwButtonClassRec.base_class.get_internal_dimension(w , &x , &y ,
			&width , &height);

	preferred->request_mode = CWWidth | CWHeight;

	compute_dim (cw , &pwidth , &pheight);

	preferred->width = (Dimension)(pwidth + (cw->core.width - width));
	preferred->height = (Dimension)(pheight + (cw->core.height - height));

	if (((intended->request_mode & (CWWidth | CWHeight))
		== (CWWidth | CWHeight)) &&
		intended->width == preferred->width &&
		intended->height == preferred->height)
		return XtGeometryYes;

	else if (preferred->width == cw->core.width &&
		preferred->height == cw->core.height)
		return XtGeometryNo;

	else return XtGeometryAlmost;
	
}

static void compute_dim(MwButtonWidget w , Dimension *width , Dimension *height)
{
	Dimension widthi = 0 , widtht = 0 , heighti = 0 , heightt = 0;
	 
	if (w->button.icon)
	{
		widthi = w->button.icon->width;
		heighti = w->button.icon->height;
	}
	if (w->button.label)
	{
		widtht = max_line_width(w->button.label , w->button.font);
		heightt = (w->button.font->max_bounds.ascent +
			w->button.font->max_bounds.descent) * 
			cnt_lines(w->button.label);
	}
	switch (w->button.icon_justify)
	{
		case XtCright:
		case XtCleft:
			*width = widthi + widtht +
				(w->button.label && w->button.label) * 
				w->button.spacing + (w->button.label != NULL) *
				2 * w->button.spacing;

			*height = ((heighti > heightt) ? heighti : heightt) + 
				(w->button.label != NULL) *
				2 * w->button.spacing;
			break;
		case XtCcenter:
		case XtCtop:
		case XtCbottom:
			*height = heighti + heightt +
				(w->button.label && w->button.label) * 
				w->button.spacing + (w->button.label != NULL) *
				2 * w->button.spacing;

			*width = ((widthi > widtht) ? widthi : widtht) + 
				(w->button.label != NULL) *
				2 * w->button.spacing;
			break;
		default:
			*width = 0;
			*height = 0;
	}
}

static void compute_pos(MwButtonWidget w, Dimension width, Dimension height,
		int *text_x, int *text_y, int *icon_x, int *icon_y)
{
	int widthi = 0 , widtht = 0 , heighti = 0 , heightt = 0;
	int pom;

	if (w->button.icon)
	{
		widthi = w->button.icon->width;
		heighti = w->button.icon->height;
	}
	if (w->button.label)
	{
		widtht = max_line_width(w->button.label , w->button.font);
		heightt = (w->button.font->max_bounds.ascent +
			w->button.font->max_bounds.descent) * 
			cnt_lines(w->button.label);
	}
	switch (w->button.icon_justify)
	{
		case XtCright:
			pom = (width - widthi - widtht) / 
				((w->button.label && w->button.icon) + 2);
			*text_x = width - widtht - pom;
			*text_y = (height - heightt) / 2;
			*icon_x = pom;
			*icon_y = (height - heighti) / 2;
			break;
		case XtCleft:
			pom = (width - widthi - widtht) / 
				((w->button.label && w->button.icon) + 2);
			*text_x = pom;
			*text_y = (height - heightt) / 2 ;
			*icon_x = width - widthi - pom;
			*icon_y = (height - heighti) / 2;
			break;
		case XtCcenter:
		case XtCtop:
			pom = (height - heighti - heightt) /
				((w->button.label && w->button.icon) + 2);
			*text_x = (width - widtht) / 2;
			*text_y = height - heightt - pom;
			*icon_x = (width - widthi) / 2;
			*icon_y = pom;
			break;
		case XtCbottom:
			pom = (height - heighti - heightt) /
				((w->button.label && w->button.icon) + 2);
			*text_x = (width - widtht) / 2;
			*text_y = pom ;
			*icon_x = (width - widthi) / 2;
			*icon_y = height - heighti - pom;
			break;
		default:
			*text_x = *text_y = *icon_x = *icon_y = 0;
	}
}
