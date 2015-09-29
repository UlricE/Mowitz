/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include	"MwLineMEP.h"
#include	"MwNws.h"

static void ClassInitialize(void);
static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Redisplay (Widget, XEvent *, Region);

static XtResource resources[] = {
        {
         XtNheight ,
         XtCHeight ,
         XtRDimension ,
         sizeof(Dimension) ,
         XtOffsetOf(RectObjRec,rectangle.height) ,
         XtRImmediate ,
         (XtPointer) 2
        },
};

MwLineMEClassRec mwLineMEClassRec = {
/* core */
   {
    /* superclass            */ (WidgetClass) &mwBaseMEClassRec,
    /* class_name            */ "MwLineME",
    /* widget_size           */ sizeof(MwLineMERec),
    /* class_initialize      */ ClassInitialize,
    /* class_part_initialize */ NULL,
    /* class_inited          */ FALSE,
    /* initialize            */ Initialize,
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
    /* destroy               */ NULL,
    /* resize                */ XtInheritResize,
    /* expose                */ Redisplay,
    /* set_values            */ NULL,
    /* set_values_hook       */ NULL,
    /* set_values_almost     */ XtInheritSetValuesAlmost,
    /* get_values_hook       */ NULL,
    /* rect9                 */ NULL,
    /* version               */ XtVersion,
    /* callback_private      */ NULL,
    /* rect10                */ NULL,
    /* query_geometry        */ XtInheritQueryGeometry,
    /* rect11                */ NULL,
    /* extension             */ NULL
   },
/* baseME */
   {
    /* get_internal_dimension  */ XtInheritGetInternalDimension,
    /* set_internal_dimension  */ XtInheritSetInternalDimension,
    /* enternotify	       */ NULL,
    /* leavenotify	       */ NULL,
    /* activate		       */ NULL,
   },
/* lineME */
   {
   /* empty		       */ 0
   }
};

WidgetClass mwLineMEObjectClass = (WidgetClass) &mwLineMEClassRec;

static void ClassInitialize(void)
{
	_InitializeWidgetSet();
}

static void Initialize(Widget req_widget, Widget new_widget,
		ArgList args, Cardinal *num_args)
{
	MwLineMEObject nw = (MwLineMEObject) new_widget;
	Display * dpy = XtDisplayOfObject(new_widget);
	XGCValues gc_res;
	XtGCMask  gc_mask;

	gc_res.foreground = nw->baseME.foreground;
	gc_mask = GCForeground ;

	nw->lineME.gc = XCreateGC(dpy,DefaultRootWindow(dpy),
		gc_mask, &gc_res);

}

static void Redisplay(Widget w , XEvent *event , Region region)
{
	MwLineMEObject cw = (MwLineMEObject) w;
	Display * dpy = XtDisplayOfObject(w);
	Window win = XtWindowOfObject(w);

	XSetForeground(dpy , cw->lineME.gc , cw->baseME.dark);
	XFillRectangle(dpy , win , cw->lineME.gc , cw->rectangle.x , cw->rectangle.y ,
		cw->rectangle.width , cw->rectangle.height / 2);

	XSetForeground(dpy , cw->lineME.gc , cw->baseME.light);
	XFillRectangle(dpy , win , cw->lineME.gc , cw->rectangle.x , cw->rectangle.y +
		cw->rectangle.height / 2  , cw->rectangle.width , cw->rectangle.height);

}
