/* MwFrame.c - Put a decorative frame around any other widget.
 *
 * Author: Edward A. Falk
 *	   falk@falconer.vip.best.com
 *
 * Date: Sept 29, 1998
 *
 *
 * Revision 1.3  1998/12/16 03:10:59  falk
 * fixed shadow_etched_out, shadow_etched_in
 *
 * Revision 1.2  1998/12/15 06:35:16  falk
 * label is now another child widget.  Some names
 * changed for compatibility with Motif.
 *
 * Revision 1.1  1998/10/12 01:33:20  falk
 * Initial revision
 *
 * Revision 1.1  1998/10/09 17:11:12  falk
 * Initial revision
 *
 *
 */

#include	<stdio.h>

#include	<X11/Xlib.h>
#include	<X11/IntrinsicP.h>
#include	<X11/StringDefs.h>
#include	<X11/Xmu/Converters.h>
#include	<X11/Xmu/CharSet.h>
#include	<X11/Xaw/Label.h>


#include	"MwFrameP.h"
#include	"MwGcs.h"

#define	MIN_SIZE	2	/* make sure we don't have zero-size widget */
#define	TTL_MARGIN	5	/* minimum margin for title */

#ifndef	max
#define	max(a,b)	((a)<(b)?(b):(a))
#define	min(a,b)	((a)>(b)?(b):(a))
#endif


/****************************************************************
 *
 * MwFrame Resources
 *
 ****************************************************************/

#define	offset(field)	XtOffsetOf(MwFrameRec, frame.field)
static XtResource resources[] = {

  { XtNshadowType, XtCShadowType, XtRShadowType, sizeof(XtShadowType),
	offset(type), XtRImmediate, (XtPointer)Lowered},
  { XtNshadowWidth, XtCShadowWidth, XtRDimension, sizeof(Dimension),
	offset(shadowWidth), XtRImmediate, (XtPointer)2 },
  { XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
	offset(foreground), XtRString, XtDefaultForeground},
  { XtNtitle, XtCTitle, XtRWidget, sizeof(Widget),
	offset(title), XtRWidget, NULL},
  { XtNjustify, XtCJustify, XtRJustify, sizeof(XtJustify),
	offset(justify), XtRString, XtJustifyLeft},
  { XtNmarginWidth, XtCMargin, XtRDimension, sizeof(Dimension),
	offset(marginWidth), XtRImmediate, (XtPointer)0 },
  { XtNmarginHeight, XtCMargin, XtRDimension, sizeof(Dimension),
	offset(marginHeight), XtRImmediate, (XtPointer)0 },
  { XtNallowResize, XtCAllowResize, XtRBoolean, sizeof(Boolean),
	offset(allowResize), XtRImmediate, (XtPointer)True },
  {XtNbeNiceToColormap, XtCBeNiceToColormap, XtRBoolean, sizeof(Boolean),
      offset(be_nice_to_cmap), XtRImmediate, (XtPointer) False},
  {XtNtopShadowContrast, XtCTopShadowContrast, XtRInt, sizeof(int),
      offset(top_shadow_contrast), XtRImmediate, (XtPointer) 20},
  {XtNbottomShadowContrast, XtCBottomShadowContrast, XtRInt, sizeof(int),
      offset(bot_shadow_contrast), XtRImmediate, (XtPointer) 40},

  { XtNborderWidth, XtCBorderWidth, XtRDimension, sizeof(Dimension),
         XtOffsetOf(RectObjRec,rectangle.border_width), XtRImmediate,
	 (XtPointer)0},
};
#undef	offset




#ifndef	__STDC__

	/* member functions */

static	void	MwFrameClassInit();
static	void	MwFrameInit();
static	void	MwFrameResize();
static	void	MwFrameExpose();
static	void	MwFrameDestroy();
static	void	MwFrameRealize();
static	Boolean	MwFrameSetValues();
static	XtGeometryResult	MwFrameQueryGeometry();
static	XtGeometryResult	MwFrameGeometryManager();
static	void	MwFrameChangeManaged();
static	void	_CvtStringToShadowType() ;

	/* internal privates */

static	void	MwFrameAllocGCs() ;	/* get rendering GCs */
static	void	MwFrameFreeGCs() ;	/* return rendering GCs */
static	void	DrawMwFrame() ;		/* draw all frame */
static	void	DrawMwFrame() ;		/* draw frame around contents */
static	void	DrawTrim() ;		/* draw trim around a tab */
static	void	DrawBorder() ;		/* draw border */
static	void	PreferredSize() ;	/* compute preferred size */
static	void	PreferredSize3() ;	/* compute preferred size */
static	Widget	MwFrameChild() ;		/* find primary child widget */

static	Pixel	AllocShadowPixel() ;

#else

static void MwFrameClassInit() ;
static void MwFrameInit( Widget req, Widget new, ArgList args, Cardinal *) ;
static void MwFrameRealize( Widget w, Mask *, XSetWindowAttributes *) ;
static void MwFrameDestroy( Widget w ) ;
static void MwFrameResize( Widget w) ;
static void MwFrameExpose( Widget w , XEvent *event , Region region ) ;
static Boolean MwFrameSetValues( Widget, Widget, Widget, ArgList, Cardinal *) ;

static XtGeometryResult MwFrameQueryGeometry( Widget,
				XtWidgetGeometry *, XtWidgetGeometry *) ;
static XtGeometryResult MwFrameGeometryManager( Widget,
				XtWidgetGeometry *, XtWidgetGeometry *) ;
static void MwFrameChangeManaged( Widget w) ;

static	void	_CvtStringToShadowType( XrmValuePtr, Cardinal *,
		  XrmValuePtr, XrmValuePtr) ;



static	void	MwFrameAllocGCs( MwFrameWidget fw ) ;
static	void	MwFrameFreeGCs( MwFrameWidget fw ) ;
static	void	PreferredSize( MwFrameWidget fw, Dimension *w, Dimension *h) ;
static	void	PreferredSize3(MwFrameWidget, int cw, int ch, int tw, int th,
			Dimension *, Dimension *) ;
static	Widget	MwFrameChild(MwFrameWidget) ;

#endif

#if XtSpecificationRelease < 5
static	GC	XtAllocateGC() ;
#endif

#define	AddRect(i,xx,yy,w,h)	\
  do{rects[(i)].x=(xx); rects[i].y=(yy);	\
     rects[i].width=(w); rects[i].height=(h);}while(0)


/****************************************************************
*
* Full class record constant
*
****************************************************************/

MwFrameClassRec mwFrameClassRec = {
  {
/* core_class fields      */
    /* superclass         */    (WidgetClass) &compositeClassRec,
    /* class_name         */    "MwFrame",
    /* widget_size        */    sizeof(MwFrameRec),
    /* class_initialize   */    MwFrameClassInit,
    /* class_part_init    */	NULL,			/* TODO? */
    /* class_inited       */	FALSE,
    /* initialize         */    MwFrameInit,
    /* initialize_hook    */	NULL,
    /* realize            */    MwFrameRealize,
    /* actions            */    NULL,
    /* num_actions	  */	0,
    /* resources          */    resources,
    /* num_resources      */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion	  */	TRUE,
    /* compress_exposure  */	TRUE,
    /* compress_enterleave*/	TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    MwFrameDestroy,
    /* resize             */    MwFrameResize,
    /* expose             */    MwFrameExpose,
    /* set_values         */    MwFrameSetValues,
    /* set_values_hook    */	NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */	NULL,
    /* accept_focus       */    NULL,
    /* version            */	XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    XtInheritTranslations,
    /* query_geometry     */	MwFrameQueryGeometry,
    /* display_accelerator*/	XtInheritDisplayAccelerator,
    /* extension          */	NULL
  },
  {
/* composite_class fields */
    /* geometry_manager   */    MwFrameGeometryManager,
    /* change_managed     */    MwFrameChangeManaged,
    /* insert_child	  */	XtInheritInsertChild,	/* TODO? */
    /* delete_child	  */	XtInheritDeleteChild,	/* TODO? */
    /* extension          */	NULL
  },
  {
/* MwFrame class fields */
    /* extension	  */	NULL,
  }
};

WidgetClass mwFrameWidgetClass = (WidgetClass)&mwFrameClassRec;



#ifdef	DEBUG
#ifdef	__STDC__
#define	assert(e) \
	  ((e) || fprintf(stderr,"yak! %s at %s:%d\n",#e,__FILE__,__LINE__))
#else
#define	assert(e) \
	  ((e) || fprintf(stderr,"yak! e at %s:%d\n",__FILE__,__LINE__))
#endif
#else
#define	assert(e)
#endif




/****************************************************************
 *
 * Member Procedures
 *
 ****************************************************************/

static void
MwFrameClassInit()
{
    static XtConvertArgRec parentCvtArgs[] = {
	{XtBaseOffset, (XtPointer)XtOffsetOf(WidgetRec, core.parent),
	     sizeof(Widget)}
    };
    XtAddConverter( XtRString, XtRShadowType, _CvtStringToShadowType,
    	NULL, 0) ;
    XtAddConverter( XtRString, XtRJustify, XmuCvtStringToJustify, NULL, 0) ;
    XtSetTypeConverter (XtRString, XtRWidget, XmuNewCvtStringToWidget,
			parentCvtArgs, XtNumber(parentCvtArgs), XtCacheNone,
			(XtDestructor)NULL);
}



	/* Init a newly created frame widget.  Compute height of frame
	 * and optionally compute size of widget.
	 */

/* ARGSUSED */

static void
MwFrameInit(request, new, args, num_args)
    Widget request, new;
    ArgList args;
    Cardinal *num_args;
{
    MwFrameWidget newFw = (MwFrameWidget)new;

    /* if size not explicitly set, set it to our preferred size now. */

    if( request->core.width == 0 || request->core.height == 0 )
    {
      Dimension	w,h ;
      PreferredSize(newFw, &w, &h) ;
      if( request->core.width == 0 ) new->core.width = w ;
      if( request->core.height == 0 ) new->core.height = h ;
      XtClass(new)->core_class.resize(new) ;
    }

    /* defer GC allocation, etc., until Realize() time. */
    newFw->frame.foregroundGC =
    newFw->frame.topGC =
    newFw->frame.botGC = None ;

    newFw->frame.needs_layout = False ;
}




	/* Called when frame widget first realized.  Create the window
	 * and allocate the GCs
	 */

#define superclass (&compositeClassRec)

static	void
MwFrameRealize(w, valueMask, attributes)
	Widget w;
	Mask *valueMask;
	XSetWindowAttributes *attributes;
{
	MwFrameWidget fw = (MwFrameWidget) w;

	(*superclass->core_class.realize)(w, valueMask, attributes);
	MwFrameAllocGCs(fw) ;
}



static	void
MwFrameDestroy(w)
	Widget	w ;
{
	MwFrameFreeGCs((MwFrameWidget)w) ;
}


	/* Parent has resized us.  This will require that the frame be
	 * laid out again.
	 */

static void
MwFrameResize(w)
	Widget	w;
{
	MwFrameWidget	fw = (MwFrameWidget) w;
	Widget		child ;
	Dimension	cw,ch,bw ;
	Dimension	sw = fw->frame.shadowWidth ;
	Widget		ttl = fw->frame.title ;
	int		tw,th ;		/* title width, height */
	int		cy ;		/* child y */

	if( ttl != NULL )
	{
	  tw = ttl->core.width ;
	  th = ttl->core.height ;
	  switch( fw->frame.justify ) {
	    case XtJustifyLeft:
	      fw->frame.tx = sw + max(fw->frame.marginWidth, TTL_MARGIN) ;
	      break ;
	    case XtJustifyCenter:
	      fw->frame.tx = (w->core.width - tw) / 2 ;
	      break ;
	    case XtJustifyRight:
	      fw->frame.tx = w->core.width - tw - sw -
	      	max(fw->frame.marginWidth, TTL_MARGIN ) ;
	      break ;
	  }
	  fw->frame.ty = 0 ;
	  cy = max(th,sw) + fw->frame.marginHeight ;
	  fw->frame.sy = th > sw ? (th - sw)/2 : 0 ;
	  fw->frame.sh = fw->core.height - fw->frame.sy ;

	  XtConfigureWidget(ttl, fw->frame.tx, fw->frame.ty, tw,th, 0) ;
	}
	else {
	  cy = sw + fw->frame.marginHeight ;
	  fw->frame.sy = 0 ;
	  fw->frame.sh = fw->core.height ;
	}

	if( (child = MwFrameChild(fw)) != NULL )
	{
	  /* assign child position & size.  */

	  bw = child->core.border_width ;
	  cw = fw->core.width - 2*(bw+sw+fw->frame.marginWidth) ;
	  ch = fw->core.height - cy - sw - fw->frame.marginHeight - 2*bw ;

	  XtConfigureWidget(child, sw+fw->frame.marginWidth, cy, cw,ch, bw) ;
	}

	fw->frame.needs_layout = False ;
} /* Resize */



	/* Redraw entire MwFrame widget */

/* ARGSUSED */
static	void
MwFrameExpose(w, event, region)
	Widget	w ;
	XEvent	*event ;
	Region	region ;
{
	MwFrameWidget	fw = (MwFrameWidget) w;
	int		s = fw->frame.shadowWidth ;
	Position	sy = fw->frame.sy ;
	Dimension	sw = fw->core.width ;
	Dimension	sh = fw->frame.sh ;
	GC		topGC = fw->frame.topGC ;
	GC		botGC = fw->frame.botGC ;
	GC		tophalf = fw->frame.top_half_GC;
	GC		bothalf = fw->frame.bot_half_GC;

	if( fw->frame.needs_layout )
	  XtClass(w)->core_class.resize(w) ;

	switch( fw->frame.type ) {
	  case Blank: break ;
	  case Solid:
	    Draw3dBox(w, 0,sy, sw,sh,
		s, fw->frame.foregroundGC, fw->frame.foregroundGC,
		fw->frame.foregroundGC, fw->frame.foregroundGC) ;
	    break ;

	  case Raised:
	    Draw3dBox(w, 0,sy, sw,sh, s, topGC, botGC, tophalf, bothalf) ;
	    break ;
	  case Lowered:
	    Draw3dBox(w, 0,sy, sw,sh, s, botGC, topGC, bothalf, tophalf) ;
	    break ;

	  case Ridge:
	    Draw3dBox(w, s/2, sy+s/2, sw-s, sh-s, s-s/2, botGC, topGC, bothalf, tophalf) ;
	    Draw3dBox(w, 0,sy,sw,sh, s/2, topGC, botGC, tophalf, bothalf) ;
	    break ;
	  case Groove:
	    Draw3dBox(w, s/2, sy+s/2, sw-s, sh-s, s-s/2, topGC, botGC, tophalf, bothalf) ;
	    Draw3dBox(w, 0,sy, sw,sh, s/2, botGC, topGC, bothalf, tophalf) ;
	    break ;

	  case Plateau:
	    Draw3dBox(w, 0,sy+0, sw, sh, 2, topGC, botGC, tophalf, bothalf) ;
	    Draw3dBox(w, s-2, sy+s-2, sw-s*2+4, sh-s*2+4, 2, botGC, topGC, bothalf, tophalf) ;
	    break ;
	  case Trough:
	    Draw3dBox(w, 0,sy+0, sw,sh, 2, botGC, topGC, bothalf, tophalf) ;
	    Draw3dBox(w, s-2, sy+s-2, sw-s*2+4, sh-s*2+4, 2, topGC, botGC, tophalf, bothalf) ;
	    break ;
	}
}


	/* Called when any MwFrame widget resources are changed. */

/* ARGSUSED */
static	Boolean
MwFrameSetValues(current, request, new, args, num_args)
    Widget current, request, new;
    ArgList args;
    Cardinal *num_args;
{
	MwFrameWidget curfw = (MwFrameWidget) current ;
	MwFrameWidget fw = (MwFrameWidget) new ;
	Boolean	needRedraw = False ;

	if( fw->frame.title != curfw->frame.title ||
	    fw->frame.shadowWidth != curfw->frame.shadowWidth  ||
	    fw->frame.marginWidth != curfw->frame.marginWidth ||
	    fw->frame.marginHeight != curfw->frame.marginHeight )
	{
	  needRedraw = True ;
	  fw->frame.needs_layout = True ;
	}

	/* TODO: if any color changes, need to recompute GCs and redraw */

	if( fw->core.background_pixel != curfw->core.background_pixel ||
	    fw->core.background_pixmap != curfw->core.background_pixmap ||
	    fw->frame.foreground != curfw->frame.foreground  ||
	    fw->frame.be_nice_to_cmap != curfw->frame.be_nice_to_cmap  ||
	    fw->frame.top_shadow_contrast != curfw->frame.top_shadow_contrast ||
	    fw->frame.bot_shadow_contrast != curfw->frame.bot_shadow_contrast )
	{
	  MwFrameFreeGCs(fw) ;
	  MwFrameAllocGCs(fw) ;
	  needRedraw = True ;
	}

	else if( fw->core.sensitive != curfw->core.sensitive  ||
		 fw->frame.type != curfw->frame.type  ||
		 fw->frame.justify != curfw->frame.justify )
	  needRedraw = True ;

	return needRedraw ;
}




/*
 * Return preferred size.
 */

static XtGeometryResult
MwFrameQueryGeometry(w, intended, preferred)
	Widget		w;
	XtWidgetGeometry *intended, *preferred;
{
register MwFrameWidget fw = (MwFrameWidget)w ;
	Dimension	cw,ch ;		/* child width, height */
	Dimension	tw,th ;		/* title width, height */
	XtWidgetGeometry c_intended, c_preferred ;
	Widget		child = MwFrameChild(fw) ;
	Widget		ttl = fw->frame.title ;
	Dimension	sw = fw->frame.shadowWidth ;
	Dimension	mw = fw->frame.marginWidth ;
	Dimension	mh = fw->frame.marginHeight ;


	/* Take intended size; subtract shadow width, margin, child border
	 * size and forward the offer to the child.  Take child response,
	 * add border size, margin, shadow width and return that result
	 * to parent.
	 *
	 * Somewhat more complicated if there's a title widget.
	 */

	/* First, get title size.  */

	if( ttl != NULL && XtIsManaged(ttl) ) {
	  XtQueryGeometry(ttl, NULL, &c_preferred) ;
	  tw = c_preferred.width + 2*c_preferred.border_width + 
	  	2*max(mw, TTL_MARGIN) ;
	  th = c_preferred.height + 2*c_preferred.border_width ;
	}
	else
	  tw = th = 0 ;

	/* title height will be used in place of top shadow width */
	th = max(sw,th) ;

	if( child != NULL && XtIsManaged(child) ) {
	  if( intended != NULL ) {
	    c_intended = *intended ;
	    c_intended.request_mode &= CWWidth | CWHeight ;
	    c_intended.width -= 2*sw + 2*mw + 2*child->core.border_width ;
	    c_intended.width = max(c_intended.width,1) ;
	    c_intended.height -= th+sw + 2*mh + 2*child->core.border_width ;
	    c_intended.height = max(c_intended.height,1) ;
	  }
	  else
	    c_intended.request_mode = 0 ;

	  XtQueryGeometry(child, &c_intended, &c_preferred) ;
	  cw = c_preferred.width + 2*c_preferred.border_width + 2*sw + 2*mw ;
	  ch = c_preferred.height + 2*c_preferred.border_width ;
	}
	else
	  cw = ch = MIN_SIZE ;

	preferred->request_mode = CWWidth | CWHeight ;
	preferred->width = max(cw,tw) + 2*sw ;
	preferred->height = th+sw + 2*mh + ch ;

	if( intended == NULL )
	  return XtGeometryYes ;

	if( intended->width == w->core.width  &&
	    intended->height == w->core.height )
	  return XtGeometryNo ;

	if( (!(intended->request_mode & CWWidth) ||
	      intended->width >= preferred->width)  &&
	    (!(intended->request_mode & CWHeight) ||
	      intended->height >= preferred->height) )
	  return XtGeometryYes;
	else
	  return XtGeometryAlmost;
}



/*
 * Geometry Manager; called when a child wants to be resized.
 */

static XtGeometryResult
MwFrameGeometryManager(w, req, reply)
    Widget		w;
    XtWidgetGeometry	*req;
    XtWidgetGeometry	*reply;	/* RETURN */

{
	MwFrameWidget		fw = (MwFrameWidget) XtParent(w);
	XtGeometryResult	result ;

	/* Position request always denied */

	if( !fw->frame.allowResize  ||
	    ((req->request_mode & CWX) && req->x != w->core.x) ||
	    ((req->request_mode & CWY) && req->y != w->core.y) )
	  return XtGeometryNo ;

	/* Make all three fields in the request valid */
	if( !(req->request_mode & CWWidth) )
	    req->width = w->core.width;
	if( !(req->request_mode & CWHeight) )
	    req->height = w->core.height;
	if( !(req->request_mode & CWBorderWidth) )
	    req->border_width = w->core.border_width;

	if( req->width == w->core.width &&
	    req->height == w->core.height &&
	    req->border_width == w->core.border_width )
	  return XtGeometryNo ;

	/* Size changes must see if the new size can be accomodated.
	 * A request to resize will be accepted only if the MwFrame can be
	 * resized to accomodate.
	 */

	if (req->request_mode & (CWWidth | CWHeight | CWBorderWidth))
	{
	  XtWidgetGeometry	myrequest, myreply ;
	  Dimension	cw,ch ;		/* child size, including borders */
	  Dimension	tw,th ;		/* title size, including borders */
	  Dimension	wid,hgt ;	/* MwFrame widget size */
	  Dimension	oldWid = fw->core.width, oldHgt = fw->core.height ;
	  Widget	child = MwFrameChild(fw) ;
	  Widget	ttl = fw->frame.title ;
	  Dimension	sw = fw->frame.shadowWidth ;
	  Dimension	mw = fw->frame.marginWidth ;
	  Dimension	mh = fw->frame.marginHeight ;
	  Dimension	margin = sw + req->border_width ;

	  if( w == child ) {
	    cw = req->width + req->border_width*2 ;
	    ch = req->height + req->border_width*2 ;
	  }
	  else if( child != NULL ) {
	    cw = child->core.width + child->core.border_width*2 ;
	    ch = child->core.height + child->core.border_width*2 ;
	  }
	  else
	    cw = ch = 0 ;

	  if( w == ttl ) {
	    tw = req->width + req->border_width*2 ;
	    th = req->height + req->border_width*2 ;
	  }
	  else if( ttl != NULL ) {
	    tw = ttl->core.width + ttl->core.border_width*2 ;
	    th = ttl->core.height + ttl->core.border_width*2 ;
	  }
	  else
	    tw = th = 0 ;

	  PreferredSize3(fw, cw,ch, tw,th, &wid, &hgt) ;

	  /* Ask to be resized to accomodate. */

	  myrequest.width = wid ;
	  myrequest.height = hgt ;
	  myrequest.request_mode = CWWidth | CWHeight ;

	  myrequest.request_mode |= req->request_mode & XtCWQueryOnly ;

	  result = XtMakeGeometryRequest((Widget)fw, &myrequest, &myreply) ;

	  /* !$@# Box widget changes the core size even if QueryOnly
	   * is set.  I'm convinced this is a bug.  At any rate, to work
	   * around the bug, we need to restore the core size after every
	   * query geometry request.  This is only partly effective,
	   * as there may be other boxes further up the tree.
	   */
	  if( myrequest.request_mode & XtCWQueryOnly ) {
	    fw->core.width = oldWid ;
	    fw->core.height = oldHgt ;
	  }

	  /* If parent offers a compromise, we do the same. */
	  if( result == XtGeometryAlmost ) {
	    reply->width  = myreply.width - (margin + mw) * 2 ;
	    reply->width  = min(reply->width, req->width) ;
	    reply->height = myreply.height- (margin+mh)*2;
	    reply->height = min(reply->height, req->height) ;
	    reply->border_width = req->border_width ;
	    if( reply->width < 1 || reply->height < 1 )
	      result = XtGeometryNo ;
	  }

	  else if( result == XtGeometryYes &&
		   !(req->request_mode & XtCWQueryOnly) )
	  {
	    w->core.width = req->width ;
	    w->core.height = req->height ;
	    w->core.border_width = req->border_width ;
	  }

	  return result ;
	}

	return XtGeometryNo ;
}




	/* The number of children we manage has changed; recompute
	 * size from scratch.
	 */

static	void
MwFrameChangeManaged(w)
    Widget w;
{
	MwFrameWidget		fw = (MwFrameWidget)w ;
	XtWidgetGeometry	request, reply ;

	request.request_mode = CWWidth | CWHeight ;
	PreferredSize(fw, &request.width, &request.height) ;
	(void) XtMakeGeometryRequest(w, &request, &reply) ;

	/* TODO: is this needed? */
	XtClass(w)->core_class.resize(w) ;
}







/****************************************************************
 *
 * Private Procedures
 *
 * TODO:  Somehow make these shared.
 *
 ****************************************************************/


static	void
MwFrameAllocGCs(fw)
	MwFrameWidget fw ;
{
	Widget	w = (Widget)fw ;
	Screen *scn = XtScreen(w);
	XGCValues values;

	fw->frame.foregroundGC =
		AllocFgGC(w, fw->frame.foreground, None ) ;
	fw->frame.topGC = AllocTopShadowGC(w, fw->frame.top_shadow_contrast,
		fw->frame.be_nice_to_cmap) ;
	fw->frame.botGC = AllocBotShadowGC(w, fw->frame.bot_shadow_contrast,
		fw->frame.be_nice_to_cmap) ;
	values.foreground = WhitePixelOfScreen(scn);
	fw->frame.top_half_GC = XtAllocateGC(w, w->core.depth,
		GCForeground, &values,
		0L,
		GCBackground|GCFont|GCSubwindowMode|GCGraphicsExposures|
			GCDashOffset|GCDashList|GCArcMode);
	values.foreground = BlackPixelOfScreen(scn);
	fw->frame.bot_half_GC = XtAllocateGC(w, w->core.depth,
		GCForeground, &values,
		0L,
		GCBackground|GCFont|GCSubwindowMode|GCGraphicsExposures|
			GCDashOffset|GCDashList|GCArcMode);
}


static	void
MwFrameFreeGCs(fw)
	MwFrameWidget	fw ;
{
	Widget w = (Widget) fw;

	XtReleaseGC(w, fw->frame.foregroundGC) ;
	XtReleaseGC(w, fw->frame.topGC) ;
	XtReleaseGC(w, fw->frame.botGC) ;
}







	/* GEOMETRY UTILITIES */


	/* find preferred size.  Ask child, add room for frame & return.  */

static	void
PreferredSize(fw, reply_width, reply_height)
	MwFrameWidget	fw;
	Dimension	*reply_width, *reply_height;	/* total widget size */
{
	XtWidgetGeometry preferred ;

	(void) MwFrameQueryGeometry((Widget)fw, NULL, &preferred) ;
	*reply_width = preferred.width ;
	*reply_height = preferred.height ;
}


	/* Find preferred size, given child and title's preferred sizes. */

static	void
PreferredSize3(fw, cw,ch, tw,th, reply_width, reply_height)
	MwFrameWidget	fw;
	int		cw,ch ;		/* child width, height */
	int		tw,th ;		/* title width, height */
	Dimension	*reply_width, *reply_height;	/* total widget size */
{
	Dimension	sw = fw->frame.shadowWidth ;

	cw += 2*fw->frame.marginWidth ;
	tw += 2*max(fw->frame.marginWidth, TTL_MARGIN) ;

	*reply_width = max(cw,tw) + 2*sw ;
	*reply_height = max(sw,th) + ch + 2*fw->frame.marginHeight + sw ;
}


static	Widget
MwFrameChild(fw)
    MwFrameWidget	fw ;
{
    Widget	*childP = NULL ;
    Widget	ttl = fw->frame.title ;
    int		i ;

    if( (childP = fw->composite.children) != NULL )
      for(i=fw->composite.num_children; --i >= 0; ++childP )
	if( *childP != ttl )
	  return *childP ;

    return NULL ;
}






	/* RESOURCES */



/* ARGSUSED */
static	void
_CvtStringToShadowType(args, num_args, fromVal, toVal)
    XrmValuePtr args;		/* unused */
    Cardinal    *num_args;	/* unused */
    XrmValuePtr fromVal ;
    XrmValuePtr toVal ;
{
    String	str = (String)fromVal->addr ;
    int		i ;
    static XtShadowType	type;
    static struct {char *name; XtShadowType type;} types[] = {
      {"blank", Blank}, {"none", Blank}, {"solid", Solid},
      {"raised", Raised}, {"shadow_out", Raised},
      {"lowered", Lowered}, {"shadow_in", Lowered},
      {"ridge", Ridge}, {"shadow_etched_out", Ridge},
      {"groove", Groove}, {"shadow_etched_in", Groove},
      {"plateau", Plateau}, {"trough", Trough},
    } ;

    for(i=0; i<XtNumber(types); ++i)
    if( XmuCompareISOLatin1(str, types[i].name) == 0 ) {
      type = types[i].type ;
      break ;
    }

    if( i > XtNumber(types) ) {
      XtStringConversionWarning(fromVal->addr, XtRShadowType);
      toVal->size = 0 ;
      toVal->addr = NULL ;
      return ;
    }

    toVal->size = sizeof(type) ;
    toVal->addr = (XPointer) &type;
}
