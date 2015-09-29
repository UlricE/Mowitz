/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "MwUtils.h"

#include "MwLabelMEP.h"
#include "MwNws.h"

#define offset(field) XtOffsetOf(MwLabelMERec,labelME.field)

static XtResource resources [] = {
	{
	 XtNleft_icon ,
	 XtCLeft_icon ,
	 XtRIcon ,
	 sizeof(Icon*) ,
	 offset(left_icon) ,
	 XtRImmediate ,
	 (XtPointer) NULL
	},
	{
	 XtNright_icon ,
	 XtCRight_icon ,
	 XtRIcon ,
	 sizeof(Icon*) ,
	 offset(right_icon) ,
	 XtRImmediate ,
	 (XtPointer) NULL
	},

	{
	 XtNlabel ,
	 XtCLabel ,
	 XtRString ,
	 sizeof(char *) ,
	 offset(label) ,
	 XtRImmediate ,
	 (XtPointer) NULL
	},
	{
	 XtNlabel_justify ,
	 XtCLabel_justify ,
	 XtRNwsJustify ,
	 sizeof(int) ,
	 offset(label_justify) ,
	 XtRImmediate ,
	 (XtPointer) XtCleft
	},
	{
	 XtNlabel_position ,
	 XtCLabel_position ,
	 XtRInt ,
	 sizeof(int) ,
	 offset(label_position) ,
	 XtRImmediate ,
	 (XtPointer) -1
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
	 (XtPointer) 3
	},
};

static void Redisplay (Widget, XEvent *, Region);
static void Destroy (Widget);
static void ClassInitialize (void);
static void Initialize (Widget, Widget, ArgList, Cardinal *);
static Boolean SetValues (Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult QueryGeometry(Widget,
                XtWidgetGeometry *, XtWidgetGeometry *);

MwLabelMEClassRec mwLabelMEClassRec = {
/* core */
   {
    /* superclass            */ (WidgetClass) &mwBaseMEClassRec,
    /* class_name            */ "MwLabelME",
    /* widget_size           */ sizeof(MwLabelMERec),
    /* class_initialize      */ ClassInitialize,
    /* class_part_initialize */ NULL,
    /* class_inited          */ FALSE,
    /* initialize            */ (XtInitProc) Initialize,
    /* initialize_hook       */ NULL,
    /* rect1                 */ NULL,
    /* rect2                 */ NULL,
    /* rect3                 */ 0,
    /* resources             */ resources,
    /* num_resources         */ XtNumber(resources),
    /* xrm_class             */ NULLQUARK,
    /* rect4                 */ False,
    /* rect5                 */ False,
    /* rect6                 */ False,
    /* rect7                 */ False,
    /* destroy               */ Destroy,
    /* resize                */ XtInheritResize,
    /* expose                */ Redisplay,
    /* set_values            */ (XtSetValuesFunc) SetValues,
    /* set_values_hook       */ NULL,
    /* set_values_almost     */ XtInheritSetValuesAlmost,
    /* get_values_hook       */ NULL,
    /* rect9                 */ NULL,
    /* version               */ XtVersion,
    /* callback_private      */ NULL,
    /* rect10                */ NULL,
    /* query_geometry        */ QueryGeometry,
    /* rect11                */ NULL,
    /* extension             */ NULL
   },
/* baseME */
   {
    /* get_internal_dimension  */ XtInheritGetInternalDimension,
    /* set_internal_dimension  */ XtInheritSetInternalDimension,
    /* enternotify	       */ XtInheritEnternotify,
    /* leavenotify	       */ XtInheritLeavenotify,
    /* activate		       */ XtInheritActivate,
   },
/* mwLabelME */
   {
   /* empty		       */ 0,
   },
};

WidgetClass mwLabelMEObjectClass = (WidgetClass) &mwLabelMEClassRec;

static void ClassInitialize(void)
{
    _InitializeWidgetSet();

    XtSetTypeConverter(XtRString, XtRNwsJustify, cvtStringToJustify,
                       NULL, 0, XtCacheNone, NULL);

    XtSetTypeConverter(XtRString, XtRIcon, cvtStringToIcon,
                       NULL, 0, XtCacheNone, NULL);
}

static void Initialize(Widget req_widget, Widget new_widget,
		ArgList args, Cardinal *num_args)
{
	MwLabelMEObject nw = (MwLabelMEObject) new_widget;
	Dimension width = 0 , height = 0;
	Display *dpy = XtDisplayOfObject(new_widget);
	XGCValues gc_res;
	XtGCMask  gc_mask;

	gc_res.foreground = nw->baseME.foreground;
	gc_res.font = nw->labelME.font->fid;

	gc_mask = GCForeground | GCFont;

	nw->labelME.gc = XCreateGC(dpy , DefaultRootWindow(dpy) , gc_mask, &gc_res);

	if (nw->labelME.label)
	{
		nw->labelME.label = MwStrdup(nw->labelME.label);

		if (nw->labelME.label_position > -1)
			width = XTextWidth(nw->labelME.font , nw->labelME.label , 
				strlen(nw->labelME.label)) + nw->labelME.label_position;
		else 
			width = XTextWidth(nw->labelME.font , nw->labelME.label , 
				strlen(nw->labelME.label)) + 2 * nw->labelME.spacing;

		height = nw->labelME.font->max_bounds.ascent +
                        nw->labelME.font->max_bounds.descent + 2 * nw->labelME.spacing;
	}
	if (nw->labelME.left_icon)
	{

		if (!(nw->labelME.label_position > -1))
			width += nw->labelME.left_icon->width + nw->labelME.spacing +
				nw->labelME.spacing * (nw->labelME.label == NULL);
		height = MW_MAX(height,2*nw->labelME.spacing+nw->labelME.left_icon->height);
	}
	if (nw->labelME.right_icon)
	{
		width += nw->labelME.right_icon->width + nw->labelME.spacing +
			nw->labelME.spacing * (nw->labelME.label == NULL && 
			nw->labelME.left_icon == NULL);
		height = MW_MAX(height,2*nw->labelME.spacing+nw->labelME.right_icon->height);		
	}
	mwLabelMEClassRec.baseME_class.set_internal_dimension(new_widget , width , height);
}

static void Destroy(Widget w)
{
	MwLabelMEObject cw = (MwLabelMEObject) w;

	if (cw->labelME.label)
		XtFree(cw->labelME.label);
}

static void Redisplay(Widget w, XEvent *event, Region region)
{
	MwLabelMEObject cw = (MwLabelMEObject) w;
	Display *dpy = XtDisplayOfObject(w);
	Window win = XtWindowOfObject (w);
	Position x,y,textx;
	Dimension width,height,textw;

	if (!XtIsRealized(w)) return;
		
	mwBaseMEClassRec.rect_class.expose(w , event , region);
	mwLabelMEClassRec.baseME_class.get_internal_dimension(w,&x,&y,&width,&height);

	if (cw->labelME.left_icon)
		X_DrawIcon(dpy , win , cw->labelME.left_icon , x + cw->rectangle.x +
			cw->labelME.spacing , y + cw->rectangle.y +
			(height - cw->labelME.left_icon->height) / 2);

	if (cw->labelME.right_icon)
		X_DrawIcon(dpy , win , cw->labelME.right_icon , x + cw->rectangle.x +
			width - cw->labelME.right_icon->width - cw->labelME.spacing ,
			y + cw->rectangle.y +
			(height - cw->labelME.right_icon->height) / 2);

	if (cw->labelME.label)
	{
	
		textw = XTextWidth(cw->labelME.font , cw->labelME.label ,
			strlen(cw->labelME.label));

	    switch (cw->labelME.label_justify)
	    {
		case XtCleft:
			textx = x + cw->rectangle.x + cw->labelME.spacing; 
			if (cw->labelME.left_icon) 
				textx += cw->labelME.spacing + 
					cw->labelME.left_icon->width;
			break;
		case XtCright:
			textx = x + cw->rectangle.x + width - textw -
				cw->labelME.spacing;
			if (cw->labelME.right_icon) 
				textx -= cw->labelME.spacing + 
					cw->labelME.right_icon->width;
			break;
		case XtCcenter:
		default:
			textx = x +  cw->rectangle.x + (width - textw) / 2;
	    }

 	    if (cw->labelME.label_position > -1) textx = cw->labelME.label_position;

	    if (cw->baseME.entered)
		XSetForeground(dpy , cw->labelME.gc , cw->baseME.activ_fg);
	    else
		XSetForeground(dpy , cw->labelME.gc , cw->baseME.foreground);

	    XDrawString(dpy , win , cw->labelME.gc ,
		textx , y + cw->rectangle.y + 
		(height - (cw->labelME.font->max_bounds.ascent +
		cw->labelME.font->max_bounds.descent)) / 2 +
		cw->labelME.font->max_bounds.ascent ,
		cw->labelME.label , strlen(cw->labelME.label));
	}

	if (!XtIsSensitive(w)) Xt_SetInsensitive(w);
}

#define WidgetValuesDiffer(w1,w2,component) (w1 -> labelME.component != \
                                             w2 -> labelME.component)

static Boolean SetValues(Widget current, Widget request, Widget new_widget,
		ArgList args, Cardinal *num_args)
{
	MwLabelMEObject cw = (MwLabelMEObject) current;
	MwLabelMEObject nw = (MwLabelMEObject) new_widget;
	Boolean redraw=False;

	if WidgetValuesDiffer( nw , cw , left_icon)
	{
		redraw = True;
	}

	if WidgetValuesDiffer( nw , cw , right_icon)
	{
		redraw = True;
	}

	if WidgetValuesDiffer( nw , cw , label_justify)
	{
		redraw = True;
	}

	if WidgetValuesDiffer(nw , cw , label)
	{
		if (cw->labelME.label)
		{
			XtFree(cw->labelME.label);
			cw->labelME.label = NULL;
		}
		if (nw->labelME.label) nw->labelME.label = MwStrdup(nw->labelME.label);
		redraw = True;
	}

	return redraw;	
}

static XtGeometryResult QueryGeometry(Widget w,
		XtWidgetGeometry *intended , XtWidgetGeometry *preferred)
{
	MwLabelMEObject cw = (MwLabelMEObject) w;
	Dimension pwidth = 0 , pheight = 0;
	Dimension width , height;
	Position x,y;

	mwLabelMEClassRec.baseME_class.get_internal_dimension(w , &x , &y ,
		&width , &height);

	if (cw->labelME.label)
	{
		if (cw->labelME.label_position > -1)
			pwidth = XTextWidth(cw->labelME.font , cw->labelME.label , 
				strlen(cw->labelME.label)) + cw->labelME.label_position;
		else 
			pwidth = XTextWidth(cw->labelME.font , cw->labelME.label , 
				strlen(cw->labelME.label)) + 2 * cw->labelME.spacing;

		pheight = cw->labelME.font->max_bounds.ascent +
                        cw->labelME.font->max_bounds.descent + 2 * cw->labelME.spacing;
	}
	if (cw->labelME.left_icon)
	{

		if (!(cw->labelME.label_position > -1))
			pwidth += cw->labelME.left_icon->width + cw->labelME.spacing +
				cw->labelME.spacing * (cw->labelME.label == NULL);
		pheight =MW_MAX(height,2*cw->labelME.spacing+cw->labelME.left_icon->height);
	}
	if (cw->labelME.right_icon)
	{
		pwidth += cw->labelME.right_icon->width + cw->labelME.spacing +
			cw->labelME.spacing * (cw->labelME.label == NULL && 
			cw->labelME.left_icon == NULL);
		pheight =MW_MAX(height,2*cw->labelME.spacing+cw->labelME.right_icon->height);		
	}
	
	preferred->request_mode = CWWidth | CWHeight;
	preferred->width = (Dimension)(pwidth + (cw->rectangle.width - width));
	preferred->height = (Dimension)(pheight + (cw->rectangle.height - height));


	if (((intended->request_mode & (CWWidth | CWHeight))
		== (CWWidth | CWHeight)) &&
		intended->width == preferred->width &&
		intended->height == preferred->height)
		return XtGeometryYes;

	else if (preferred->width == cw->rectangle.width &&
		preferred->height == cw->rectangle.height)
		return XtGeometryNo;

	else return XtGeometryAlmost;
}
