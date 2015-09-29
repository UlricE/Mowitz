/*
 * MwTabs.c - Index Tabs composite widget
 *
 * Author: Edward A. Falk
 *	   falk@falconer.vip.best.com
 *
 * Date: July 29, 1997
 *
 *
 * Overall layout of this widget is as follows:
 *
 *   ________ ,---------. _________
 *  |  label ||  Label  ||  Label  |  \ tabs
 *  |________||         ||_________|  /
 *  |+----------------------------+|  \
 *  ||                            ||  |
 *  ||  child widget window       ||   > frame
 *  |+----------------------------+|  |
 *  +------------------------------+  /
 *
 * The height of the tabs includes the shadow width, top and bottom
 * margins, and the height of the text.
 *
 * The height of the frame includes the top and bottom shadow width and the
 * size of the child widget window.
 *
 * The tabs overlap the frame and each other vertically by the shadow
 * width, so that when the topmost tab is drawn, it obliterates part of
 * the frame.
 *
 * Revision 1.13  1998/10/12 16:49:52  falk
 * GC functions seperated out into new file
 *
 * Revision 1.12  1998/08/07 01:08:37  falk
 * got rid of dead code
 *
 * Revision 1.11  1998/07/28 16:39:09  falk
 * now uses XtClass(w) instead of w->core.widget_class
 *
 * Revision 1.10  1998/06/26 16:27:42  falk
 * allocShadowColor now handles failure from XAllocColor
 *
 * Revision 1.9  1998/06/25 08:09:03  falk
 * simplified: got rid of WidgetCmap() and WidgetDepth()
 *
 * Revision 1.8  1998/06/18 03:12:42  falk
 * Added local definition of XtAllocateGC() for X11r4 compatibility
 *
 * Revision 1.7  1998/06/17 23:49:24  falk
 * small changes to make lint happier
 *
 * Revision 1.6  1998/06/17 23:39:40  falk
 * STDC declarations added, prototypes updated accordingly.
 *
 * Revision 1.5  1998/06/17 23:05:14  falk
 * removed last reference to Xaw
 *
 * Revision 1.4  1998/06/16 16:22:03  falk
 * Simplified 3-d look.  Removed references to Xaw3d.  Added bitmaps
 *
 * Revision 1.3  1998/06/11 17:12:57  falk
 * major style change; too many diffs to list.
 *
 * Revision 1.2  1998/02/14 07:24:45  falk
 * wider tab decorations, re-wrote geometry manager.
 *
 * Revision 1.1  1997/08/28 05:36:23  falk
 * Initial revision
 *
 */

/* TODO: min child height = tab height
 *
 */

#include	<stdio.h>

#include	<X11/Xlib.h>
#include	<X11/IntrinsicP.h>
#include	<X11/StringDefs.h>
#include	<X11/Xmu/Misc.h>

#include	"MwTabsP.h"
#include	<Mowitz/MwGcs.h>

#define	MIN_WID		10
#define	MIN_HGT		10
#define	INDENT		3	/* tabs indented from edge by this much */
#define	SPACING		0	/* distance between tabs */
#define	SHADWID		1	/* default shadow width */
#define	TABDELTA	2	/* top tab grows this many pixels when on top */


/****************************************************************
 *
 * IndexTabs Resources
 *
 ****************************************************************/

static	char	defaultTranslations[] =
	"<BtnUp>:	select()" ;

#define	offset(field)	XtOffsetOf(MwTabsRec, tabs.field)
static XtResource resources[] = {

  {XtNselectInsensitive, XtCSelectInsensitive, XtRBoolean, sizeof(Boolean),
      offset(selectInsensitive), XtRImmediate, (XtPointer) True},
  { XtNfont, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
	offset(font), XtRString, XtDefaultFont},
  { XtNinternalWidth, XtCWidth, XtRDimension, sizeof(Dimension),
	offset(internalWidth), XtRImmediate, (XtPointer)4 },
  { XtNinternalHeight, XtCHeight, XtRDimension, sizeof(Dimension),
	offset(internalHeight), XtRImmediate, (XtPointer)4 },
  { XtNborderWidth, XtCBorderWidth, XtRDimension, sizeof(Dimension),
         XtOffsetOf(RectObjRec,rectangle.border_width), XtRImmediate,
	 (XtPointer)0},
  { XtNtopWidget, XtCTopWidget, XtRWidget, sizeof(Widget),
         offset(topWidget), XtRImmediate, NULL},
  { XtNcallback, XtCCallback, XtRCallback, sizeof(XtPointer),
         offset(callbacks), XtRCallback, NULL},
  {XtNbeNiceToColormap, XtCBeNiceToColormap, XtRBoolean, sizeof(Boolean),
      offset(be_nice_to_cmap), XtRImmediate, (XtPointer) False},
  {XtNtopShadowContrast, XtCTopShadowContrast, XtRInt, sizeof(int),
      offset(top_shadow_contrast), XtRImmediate, (XtPointer) 20},
  {XtNbottomShadowContrast, XtCBottomShadowContrast, XtRInt, sizeof(int),
      offset(bot_shadow_contrast), XtRImmediate, (XtPointer) 40},
  {XtNinsensitiveContrast, XtCInsensitiveContrast, XtRInt, sizeof(int),
      offset(insensitive_contrast), XtRImmediate, (XtPointer) 33},
};
#undef	offset



	/* constraint resources */

#define	offset(field)	XtOffsetOf(MwTabsConstraintsRec, tabs.field)
static XtResource mwTabsConstraintResources[] = {
  {XtNtabLabel, XtCLabel, XtRString, sizeof(String),
	offset(label), XtRString, NULL},
  {XtNtabLeftBitmap, XtCLeftBitmap, XtRBitmap, sizeof(Pixmap),
	offset(left_bitmap), XtRImmediate, None},
  {XtNtabForeground, XtCForeground, XtRPixel, sizeof(Pixel),
	offset(foreground), XtRString, XtDefaultForeground},
  {XtNresizable, XtCResizable, XtRBoolean, sizeof(Boolean),
	offset(resizable), XtRImmediate, (XtPointer) True},
} ;
#undef	offset



#ifndef	__STDC__

	/* member functions */

static	void	TabsClassInit();
static	void	TabsInit();
static	void	TabsResize();
static	void	TabsExpose();
static	void	TabsDestroy();
static	void	TabsRealize();
static	Boolean	TabsSetValues();
static	XtGeometryResult	TabsQueryGeometry();
static	XtGeometryResult	TabsGeometryManager();
static	void	TabsChangeManaged();
static	void	TabsConstraintInitialize() ;
static	Boolean	TabsConstraintSetValues() ;

	/* action procs */

static	void	TabsSelect() ;

	/* internal privates */

static	void	TabsAllocGCs() ;	/* get rendering GCs */
static	void	TabsFreeGCs() ;		/* return rendering GCs */
static	void	DrawMwTabs() ;		/* draw all tabs */
static	void	DrawTab() ;		/* draw one index tab */
static	void	DrawFrame() ;		/* draw frame around contents */
static	void	DrawTrim() ;		/* draw trim around a tab */
static	void	DrawBorder() ;		/* draw border */
static	void	TabWidth() ;		/* recompute tab size */
static	void	MaxChild() ;
static	void	PreferredSize() ;	/* compute preferred size */
static	void	PreferredSize2() ;	/* compute preferred size */
static	void	getBitmapInfo() ;
static	int	TabLayout() ;		/* lay out tabs */
static	void	TabsShuffleRows() ;	/* bring current tab to bottom row */

static	void	TabsAllocFgGC() ;
static	void	TabsAllocGreyGC() ;

#else

static	void	TabsClassInit() ;
static	void	TabsInit( Widget req, Widget new, ArgList, Cardinal *nargs) ;
static	void	TabsConstraintInitialize(Widget, Widget, ArgList, Cardinal *) ;
static	void	TabsRealize(Widget, Mask *, XSetWindowAttributes *) ;
static	void	TabsDestroy( Widget w) ;
static	void	TabsResize( Widget w) ;
static	void	TabsExpose( Widget w, XEvent *event, Region region) ;
static	Boolean	TabsSetValues(Widget, Widget, Widget, ArgList, Cardinal *) ;
static	Boolean	TabsConstraintSetValues(Widget, Widget, Widget,
			ArgList, Cardinal *) ;
static	XtGeometryResult TabsQueryGeometry(Widget,
				XtWidgetGeometry *, XtWidgetGeometry *) ;
static	XtGeometryResult TabsGeometryManager(Widget,
				XtWidgetGeometry *, XtWidgetGeometry *) ;
static	void	TabsChangeManaged( Widget w) ;

static	void	TabsSelect(Widget, XEvent *, String *, Cardinal *) ;

static	void	DrawTabs( MwTabsWidget tw, int labels) ;
static	void	DrawTab( MwTabsWidget tw, Widget child, int labels) ;
static	void	DrawFrame( MwTabsWidget tw) ;
static	void	DrawTrim( MwTabsWidget, int x, int y,
		  int wid, int hgt, int bottom, int undraw) ;
static	void	DrawBorder( MwTabsWidget tw, Widget child, int undraw) ;

static	void	TabWidth( Widget w) ;
static	int	TabLayout( MwTabsWidget, int wid, Dimension *r_hgt,
			int query_only) ;
static	void	MaxChild( MwTabsWidget, Dimension *, Dimension *, Widget) ;
static	void	TabsShuffleRows( MwTabsWidget tw) ;
static	void	PreferredSize( MwTabsWidget,
			Dimension *reply_width, Dimension *reply_height,
			Dimension *reply_cw, Dimension *reply_ch) ;
static	void	PreferredSize2( MwTabsWidget, int cw, int ch,
			Dimension *rw, Dimension *rh) ;

static	void	TabsAllocGCs(MwTabsWidget) ;
static	void	TabsFreeGCs(MwTabsWidget) ;
static	void	getBitmapInfo( MwTabsWidget tw, MwTabsConstraints tab) ;
static	void	TabsAllocFgGC( MwTabsWidget tw) ;
static	void	TabsAllocGreyGC( MwTabsWidget tw) ;

#endif

#if XtSpecificationRelease < 5
static	GC	XtAllocateGC() ;
#endif

#define	AddRect(i,xx,yy,w,h)	\
  do{rects[(i)].x=(xx); rects[i].y=(yy);	\
     rects[i].width=(w); rects[i].height=(h);}while(0)

static	XtActionsRec	actionsList[] =
  {
    {"select",	TabsSelect},
  } ;


/****************************************************************
*
* Full class record constant
*
****************************************************************/

MwTabsClassRec mwTabsClassRec = {
  {
/* core_class fields      */
    /* superclass         */    (WidgetClass) &constraintClassRec,
    /* class_name         */    "MwTabs",
    /* widget_size        */    sizeof(MwTabsRec),
    /* class_initialize   */    TabsClassInit,
    /* class_part_init    */	NULL,			/* TODO? */
    /* class_inited       */	FALSE,
    /* initialize         */    TabsInit,
    /* initialize_hook    */	NULL,
    /* realize            */    TabsRealize,
    /* actions            */    actionsList,
    /* num_actions	  */	XtNumber(actionsList),
    /* resources          */    resources,
    /* num_resources      */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion	  */	TRUE,
    /* compress_exposure  */	TRUE,
    /* compress_enterleave*/	TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    TabsDestroy,
    /* resize             */    TabsResize,
    /* expose             */    TabsExpose,
    /* set_values         */    TabsSetValues,
    /* set_values_hook    */	NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */	NULL,
    /* accept_focus       */    NULL,
    /* version            */	XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    defaultTranslations,
    /* query_geometry     */	TabsQueryGeometry,
    /* display_accelerator*/	XtInheritDisplayAccelerator,
    /* extension          */	NULL
  },
  {
/* composite_class fields */
    /* geometry_manager   */    TabsGeometryManager,
    /* change_managed     */    TabsChangeManaged,
    /* insert_child	  */	XtInheritInsertChild,	/* TODO? */
    /* delete_child	  */	XtInheritDeleteChild,	/* TODO? */
    /* extension          */	NULL
  },
  {
/* constraint_class fields */
    /* subresources	  */	mwTabsConstraintResources,
    /* subresource_count  */	XtNumber(mwTabsConstraintResources),
    /* constraint_size	  */	sizeof(MwTabsConstraintsRec),
    /* initialize	  */	TabsConstraintInitialize,
    /* destroy		  */	NULL,
    /* set_values	  */	TabsConstraintSetValues,
    /* extension	  */	NULL,
  },
  {
/* MwTabs class fields */
    /* extension	  */	NULL,
  }
};

WidgetClass mwTabsWidgetClass = (WidgetClass)&mwTabsClassRec;



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
TabsClassInit()
{
    /* TODO: register converter for labels? */
}



	/* Init a newly created tabs widget.  Compute height of tabs
	 * and optionally compute size of widget. */

/* ARGSUSED */

static void
TabsInit(request, new, args, num_args)
    Widget request, new;
    ArgList args;
    Cardinal *num_args;
{
    MwTabsWidget newTw = (MwTabsWidget)new;

    /* height is easy, it's the same for all tabs:
     *  TODO: font height + height of tallest bitmap.
     */
    newTw->tabs.tab_height = 2 * newTw->tabs.internalHeight + SHADWID ;

    if( newTw->tabs.font != NULL )
      newTw->tabs.tab_height += newTw->tabs.font->max_bounds.ascent +
				newTw->tabs.font->max_bounds.descent ;

    /* GC allocation is deferred until XtRealize() */

    /* if size not explicitly set, set it to our preferred size now. */

    if( request->core.width == 0 || request->core.height == 0 )
    {
      Dimension	w,h ;
      PreferredSize(newTw, &w, &h, NULL,NULL) ;
      if( request->core.width == 0 ) new->core.width = w ;
      if( request->core.height == 0 ) new->core.height = h ;
      XtClass(new)->core_class.resize(new) ;
    }

    /* defer GC allocation, etc., until Realize() time. */
    newTw->tabs.foregroundGC =
    newTw->tabs.backgroundGC =
    newTw->tabs.greyGC =
    newTw->tabs.topGC =
    newTw->tabs.botGC = None ;

    newTw->tabs.grey50 = None ;

    newTw->tabs.needs_layout = False ;
}


	/* Init the constraint part of a new tab child.  Compute the
	 * size of the tab.
	 */
/* ARGSUSED */
static	void
TabsConstraintInitialize(request, new, args, num_args)
	Widget	request, new ;
	ArgList	args ;
	Cardinal *num_args;
{
	MwTabsConstraints tab = (MwTabsConstraints) new->core.constraints ;
	tab->tabs.greyAlloc = False ;	/* defer allocation of pixel */

	getBitmapInfo((MwTabsWidget)XtParent(new), tab) ;
	TabWidth(new) ;
}



	/* Called when tabs widget first realized.  Create the window
	 * and allocate the GCs
	 */

static	void
TabsRealize(w, valueMask, attributes)
	Widget w;
	Mask *valueMask;
	XSetWindowAttributes *attributes;
{
	MwTabsWidget tw = (MwTabsWidget) w;

	attributes->bit_gravity = NorthWestGravity;
	*valueMask |= CWBitGravity;

	/* TODO: shouldn't this chain to the parent's realize instead? */
	XtCreateWindow( w, (unsigned)InputOutput, (Visual *)CopyFromParent,
			*valueMask, attributes);

	TabsAllocGCs(tw) ;
}



static	void
TabsDestroy(w)
	Widget	w ;
{
	TabsFreeGCs((MwTabsWidget)w) ;
}


	/* Parent has resized us.  This will require that the tabs be
	 * laid out again.
	 */

static void
TabsResize(w)
	Widget	w;
{
	MwTabsWidget	tw = (MwTabsWidget) w;
	int		i ;
	int		num_children = tw->composite.num_children ;
	Widget		*childP ;
	MwTabsConstraints tab ;
	Dimension	cw,ch,bw ;


	/* Our size has now been dictated by the parent.  Lay out the
	 * tabs, lay out the frame, lay out the children.  Remember
	 * that the tabs overlap each other and the frame by shadowWidth.
	 * Also, the top tab is larger than the others, so if there's only
	 * one row, the widget must be made taller to accomodate this.
	 *
	 * Once the tabs are laid out, if there is more than one
	 * row, we may need to shuffle the rows to bring the top tab
	 * to the bottom row.
	 */

	if( num_children > 0 && tw->composite.children != NULL )
	{
	  /* Loop through the tabs and assign rows & x positions */
	  (void) TabLayout(tw, tw->core.width, NULL, False) ;

	  /* assign a top widget, bring it to bottom row. */
	  TabsShuffleRows(tw) ;

	  /* now assign child positions & sizes.  Positions are all the
	   * same: just inside the frame.  Sizes are also all the same.
	   */

	  tw->tabs.child_width = cw = tw->core.width - 2 * SHADWID ;
	  tw->tabs.child_height = ch =
	  		tw->core.height - tw->tabs.tab_total - 2 * SHADWID ;


	  for(i=0, childP=tw->composite.children;
	  	i < num_children;
		++i, ++childP)
	  {
	    tab = (MwTabsConstraints) (*childP)->core.constraints ;
	    bw = tab->tabs.bwid ;
	    XtConfigureWidget(*childP, SHADWID,tw->tabs.tab_total+SHADWID,
	    		cw-bw*2,ch-bw*2, bw) ;
	  }
	}

	tw->tabs.needs_layout = False ;
} /* Resize */



	/* Redraw entire MwTabs widget */

/* ARGSUSED */
static	void
TabsExpose(w, event, region)
	Widget	w ;
	XEvent	*event ;
	Region	region ;
{
	MwTabsWidget	tw = (MwTabsWidget) w;

	if( tw->tabs.needs_layout )
	  XtClass(w)->core_class.resize(w) ;

	DrawTabs(tw, True) ;
}


	/* Called when any MwTabs widget resources are changed. */

/* ARGSUSED */
static	Boolean
TabsSetValues(current, request, new, args, num_args)
    Widget current, request, new;
    ArgList args;
    Cardinal *num_args;
{
	MwTabsWidget curtw = (MwTabsWidget) current ;
	MwTabsWidget tw = (MwTabsWidget) new ;
	Boolean	needRedraw = False ;
	Widget	*childP ;
	int	i ;


	if( tw->tabs.font != curtw->tabs.font  ||
	    tw->tabs.internalWidth != curtw->tabs.internalWidth ||
	    tw->tabs.internalHeight != curtw->tabs.internalHeight )
	{
	  tw->tabs.tab_height = 2 * tw->tabs.internalHeight + SHADWID ;

	  if( tw->tabs.font != NULL )
	    tw->tabs.tab_height += tw->tabs.font->max_bounds.ascent +
				   tw->tabs.font->max_bounds.descent ;

	  /* Tab size has changed.  Resize all tabs and request a new size */
	  for(i=0, childP=tw->composite.children;
		i < tw->composite.num_children;
		++i, ++childP)
	    TabWidth(*childP) ;
	  PreferredSize(tw, &tw->core.width, &tw->core.height, NULL,NULL) ;
	  needRedraw = True ;
	  tw->tabs.needs_layout = True ;
	}

	/* TODO: if any color changes, need to recompute GCs and redraw */

	if( tw->core.background_pixel != curtw->core.background_pixel ||
	    tw->core.background_pixmap != curtw->core.background_pixmap )
	{
	  TabsFreeGCs(tw) ;
	  TabsAllocGCs(tw) ;
	  needRedraw = True ;
	}

	if( tw->core.sensitive != curtw->core.sensitive )
	  needRedraw = True ;

	/* If top widget changes, need to change stacking order, redraw tabs.
	 * Window system will handle the redraws.
	 */

	if( tw->tabs.topWidget != curtw->tabs.topWidget )
	{
	  Widget		w = curtw->tabs.topWidget ;
	  MwTabsConstraints	tab = (MwTabsConstraints) w->core.constraints ;

	  XRaiseWindow(XtDisplay(w), XtWindow(w)) ;

	  if( tab->tabs.row != tw->tabs.numRows-1 )
	    TabsShuffleRows(tw) ;

	  needRedraw = True ;
	}

	return needRedraw ;
}


	/* Called when any child constraint resources change. */

/* ARGSUSED */
static	Boolean
TabsConstraintSetValues(current, request, new, args, num_args)
    Widget current, request, new;
    ArgList args;
    Cardinal *num_args;
{
	MwTabsWidget tw = (MwTabsWidget) XtParent(new) ;
	MwTabsConstraints ctab = (MwTabsConstraints) current->core.constraints ;
	MwTabsConstraints tab = (MwTabsConstraints) new->core.constraints ;


	/* if label changes, need to re-layout the entire widget */
	/* if foreground changes, need to redraw tab label */

	/* TODO: only need resize of new bitmap has different dimensions
	 * from old bitmap.
	 */

	if( tab->tabs.label != ctab->tabs.label ||  /* Tab size has changed. */
	    tab->tabs.left_bitmap != ctab->tabs.left_bitmap )
	{
	  Dimension	wid,hgt ;
	  TabWidth(new) ;
	  PreferredSize(tw, &wid, &hgt, NULL,NULL) ;
	  tw->tabs.needs_layout = True ;

	  if( tab->tabs.left_bitmap != ctab->tabs.left_bitmap )
	    getBitmapInfo(tw, tab) ;

	  /* If there are no subclass ConstraintSetValues procedures remaining
	   * to be invoked, and if the preferred size has changed, ask
	   * for a resize.
	   */
	  if( XtClass((Widget)tw) == mwTabsWidgetClass	&&
	      ( wid != tw->core.width  || hgt != tw->core.height ) )
	  {
	    XtWidgetGeometry	request, answer ;
	    XtGeometryResult result ;
	    request.width = wid ;
	    request.height = hgt ;
	    request.request_mode = CWWidth | CWHeight ;
	    result = XtMakeGeometryRequest((Widget)tw, &request, &answer) ;
	    if( result == XtGeometryAlmost ) {
	      request = answer ;
	      result = XtMakeGeometryRequest((Widget)tw, &request, &answer) ;
	    }
	  }
	  else
	    tw->tabs.needs_layout = True ;
	}


	/* The child widget itself never needs a redisplay, but the parent
	 * MwTabs widget might.
	 */

	if( tw->tabs.needs_layout ) {
	  XClearWindow(XtDisplay((Widget)tw), XtWindow((Widget)tw)) ;
	  XtClass(tw)->core_class.expose((Widget)tw,NULL,None) ;
	}

	else if( tab->tabs.foreground != ctab->tabs.foreground )
	{
	  DrawTab(tw, new, True) ;
	}

	return False ;
}



/*
 * Return preferred size.
 */

static XtGeometryResult
TabsQueryGeometry(w, intended, preferred)
	Widget		w;
	XtWidgetGeometry *intended, *preferred;
{
register MwTabsWidget tw = (MwTabsWidget)w ;

	preferred->request_mode = CWWidth | CWHeight ;
	PreferredSize(tw, &preferred->width, &preferred->height, NULL,NULL) ;

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
TabsGeometryManager(w, req, reply)
    Widget		w;
    XtWidgetGeometry	*req;
    XtWidgetGeometry	*reply;	/* RETURN */

{
	MwTabsWidget		tw = (MwTabsWidget) XtParent(w);
	Dimension		s = SHADWID ;
	MwTabsConstraints		tab = (MwTabsConstraints)w->core.constraints;
	XtGeometryResult	result ;

	/* Position request always denied */

	if( ((req->request_mode & CWX) && req->x != w->core.x) ||
	    ((req->request_mode & CWY) && req->y != w->core.y) ||
	    !tab->tabs.resizable )
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
	 * The MwTabs widget keeps all of its children the same
	 * size.  A request to shrink will be accepted only if the
	 * new size is still big enough for all other children.  A
	 * request to shrink that is not big enough for all children
	 * returns an "almost" response with the new proposed size.
	 * A request to grow will be accepted only if the MwTabs parent can
	 * grow to accomodate.
	 */

	if (req->request_mode & (CWWidth | CWHeight | CWBorderWidth))
	{
	  Dimension	rw,rh ;		/* requested size, including borders */
	  Dimension	cw,ch ;		/* other children's preferred size */
	  Dimension	mw,mh ;		/* max of above; this is our target */
	  Dimension	aw,ah ;		/* available size we can give child */
	  Dimension	th ;		/* space used by tabs */
	  Dimension	wid,hgt ;	/* MwTabs widget size */

	  rw = req->width + 2*req->border_width ;
	  rh = req->height + 2*req->border_width ;

	  /* find out what the resulting preferred size would be */

	  MaxChild(tw, &cw, &ch, w) ;
	  mw = Max(cw, rw) ;
	  mh = Max(ch, rh) ;
	  PreferredSize2(tw, mw,mh, &wid, &hgt) ;

	  /* Ask to be resized to accomodate. */

	  if( wid != tw->core.width || hgt != tw->core.height )
	  {
	    Dimension	oldWid = tw->core.width, oldHgt = tw->core.height ;
	    XtWidgetGeometry	myrequest, myreply ;

	    myrequest.width = wid ;
	    myrequest.height = hgt ;
	    myrequest.request_mode = CWWidth | CWHeight ;

	    /* If child is only querying, or if we're going to have to
	     * offer the child a compromise, then make this a query only.
	     */

	    if( (req->request_mode & XtCWQueryOnly)  || rw < mw || rh < mh )
	      myrequest.request_mode |= XtCWQueryOnly ;

	    result = XtMakeGeometryRequest((Widget)tw, &myrequest, &myreply) ;

	    /* !$@# Box widget changes the core size even if QueryOnly
	     * is set.  I'm convinced this is a bug.  At any rate, to work
	     * around the bug, we need to restore the core size after every
	     * query geometry request.  This is only partly effective,
	     * as there may be other boxes further up the tree.
	     */
	    if( myrequest.request_mode & XtCWQueryOnly ) {
	      tw->core.width = oldWid ;
	      tw->core.height = oldHgt ;
	    }

	    /* based on the parent's response, determine what the
	     * resulting MwTabs widget size would be.
	     */

	    switch( result ) {
	      case XtGeometryYes:
		break ;

	      case XtGeometryNo:
		wid = tw->core.width ;
		hgt = tw->core.height ;
		break ;

	      case XtGeometryAlmost:
		wid = myreply.width ;
		hgt = myreply.height ;
		break;
	      default:	/* XtGeometryDone */
		;
	    }
	  }

	  /* Within the constraints imposed by the parent, what is
	   * the max size we can give the child?
	   */
	  (void) TabLayout(tw, wid, &th, True) ;
	  aw = wid - 2*s ;
	  ah = hgt - th - 2*s ;

	  /* OK, make our decision.  If requested size is >= max sibling
	   * preferred size, AND requested size <= available size, then
	   * we accept.  Otherwise, we offer a compromise.
	   */

	  if( rw == aw && rh == ah )
	  {
	    /* Acceptable.  If this wasn't a query, change *all* children
	     * to this size.
	     */
	    if( req->request_mode & XtCWQueryOnly )
	      return XtGeometryYes ;
	    else
	    {
	      Widget	*childP ;
	      int	i,bw ;
	      w->core.border_width = req->border_width ;
	      for(i=tw->composite.num_children, childP=tw->composite.children;
		    --i >= 0; ++childP)
	      {
		bw = (*childP)->core.border_width ;
		XtConfigureWidget(*childP, s,tw->tabs.tab_total+s,
			  rw-2*bw,rh-2*bw, bw) ;
	      }
#ifdef	COMMENT
	      /* TODO: under what conditions will we need to redraw? */
	      XClearWindow(XtDisplay((Widget)tw), XtWindow((Widget)tw)) ;
	      XtClass(tw)->core_class.expose((Widget)tw,NULL,NULL) ;
#endif	/* COMMENT */
	      return XtGeometryDone ;
	    }
	  }

	  /* Cannot accede to child's request.  Describe what we *can* do
	   * and return counter-offer.
	   */
	  reply->width  = aw - 2*req->border_width ;
	  reply->height = ah - 2*req->border_width ;
	  reply->border_width = req->border_width ;
	  reply->request_mode = CWWidth | CWHeight | CWBorderWidth ;
	  return XtGeometryAlmost ;
	}

	return XtGeometryYes ;
}




	/* The number of children we manage has changed; recompute
	 * size from scratch.
	 */

static	void
TabsChangeManaged(w)
    Widget w;
{
	MwTabsWidget		tw = (MwTabsWidget)w ;
	XtWidgetGeometry	request, reply ;

	request.request_mode = CWWidth | CWHeight ;
	PreferredSize(tw, &request.width, &request.height, NULL, NULL) ;
	(void) XtMakeGeometryRequest(w, &request, &reply) ;

	/* TODO: is this needed? */
	XtClass(w)->core_class.resize(w) ;
}




/****************************************************************
 *
 * Action Procedures
 *
 ****************************************************************/

	/* User clicks on a tab, figure out which one it was. */

/* ARGSUSED */
static	void
TabsSelect(w, event, params, num_params)
	Widget	w ;
	XEvent	*event ;
	String	*params ;
	Cardinal *num_params ;
{
	MwTabsWidget	tw = (MwTabsWidget) w ;
	Widget	*childP ;
	Position x,y ;
	Dimension h = tw->tabs.tab_height ;
	int	i ;

	/* TODO: is there an Xmu function or something to do this instead? */
	switch( event->type ) {
	  case ButtonPress:
	  case ButtonRelease:
	    x = event->xbutton.x ; y = event->xbutton.y ; break ;
	  case KeyPress:
	  case KeyRelease:
	    x = event->xkey.x ; y = event->xkey.y ; break ;
	  default:
	    return ;
	}

	/* TODO: determine which tab was clicked, if any.  Set that
	 * widget to be top of stacking order with XawTabsSetTop().
	 */
	for(i=0, childP=tw->composite.children;
	      i < tw->composite.num_children;
	      ++i, ++childP)
	{
	  MwTabsConstraints tab = (MwTabsConstraints)(*childP)->core.constraints;
	  if( x > tab->tabs.x  &&  x < tab->tabs.x + tab->tabs.width  &&
	      y > tab->tabs.y  &&  y < tab->tabs.y + h )
	  {
	    if( *childP != tw->tabs.topWidget &&
		(XtIsSensitive(*childP) || tw->tabs.selectInsensitive) )
	      XawTabsSetTop(*childP, True) ;
	    break ;
	  }
	}
}





/****************************************************************
 *
 * Public Procedures
 *
 ****************************************************************/


	/* Set the top tab, optionally call all callbacks. */
void
XawTabsSetTop(w, callCallbacks)
	Widget	w ;
	int	callCallbacks ;
{
	MwTabsWidget	tw = (MwTabsWidget)w->core.parent ;
	MwTabsConstraints tab ;

	if( !XtIsSubclass(w->core.parent, mwTabsWidgetClass) )
	{
	  char line[1024] ;
	  sprintf(line, "XawTabsSetTop: widget \"%s\" is not the child of a tabs widget.", XtName(w)) ;
	  XtAppWarning(XtWidgetToApplicationContext(w), line) ;
	  return ;
	}

	XRaiseWindow(XtDisplay(w), XtWindow(w)) ;

	tab = (MwTabsConstraints) w->core.constraints ;
	if( tab->tabs.row == 0 )
	{
	  /* Easy case; undraw current top, undraw new top, assign new
	   * top, redraw all borders.
	   * We *could* just erase and execute a full redraw, but I like to
	   * reduce screen flicker.
	   */
	  DrawBorder(tw, tw->tabs.topWidget, True) ;
	  DrawBorder(tw, w, True) ;
	  tw->tabs.topWidget = w ;
	  DrawTabs(tw, False) ;
	}
	else
	{
	  tw->tabs.topWidget = w ;
	  TabsShuffleRows(tw) ;
	  XClearWindow(XtDisplay((Widget)tw), XtWindow((Widget)tw)) ;
	  XtClass(tw)->core_class.expose((Widget)tw,NULL,None) ;
	}

	if( callCallbacks )
	  XtCallCallbackList(w, tw->tabs.callbacks, (XtPointer)w) ;
}




/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/


static	void
TabsAllocGCs(tw)
	MwTabsWidget tw ;
{
	TabsAllocFgGC(tw) ;
	TabsAllocGreyGC(tw) ;
	tw->tabs.backgroundGC = AllocBackgroundGC((Widget)tw, None) ;
	tw->tabs.topGC = AllocTopShadowGC((Widget)tw,
		tw->tabs.top_shadow_contrast, tw->tabs.be_nice_to_cmap) ;
	tw->tabs.botGC = AllocBotShadowGC((Widget)tw,
		tw->tabs.bot_shadow_contrast, tw->tabs.be_nice_to_cmap) ;
}


static	void
TabsFreeGCs(tw)
	MwTabsWidget	tw ;
{
	Widget w = (Widget) tw;

	XtReleaseGC(w, tw->tabs.foregroundGC) ;
	XtReleaseGC(w, tw->tabs.greyGC) ;
	XtReleaseGC(w, tw->tabs.backgroundGC) ;
	XtReleaseGC(w, tw->tabs.topGC) ;
	XtReleaseGC(w, tw->tabs.botGC) ;
}





	/* Redraw entire MwTabs widget */

static	void
DrawTabs(tw, labels)
	MwTabsWidget	tw ;
	int		labels ;	/* draw labels? */
{
	Widget		*childP ;
	int		i,j ;
	Dimension	s = SHADWID ;
	Dimension	th = tw->tabs.tab_height ;
	Position	y ;
	MwTabsConstraints	tab ;

	/* draw tabs and frames by row except for the top tab, which
	 * is drawn last.  (This is inefficiently written, but should not
	 * be too slow as long as there are not a lot of rows.)
	 */

	y = tw->tabs.numRows == 1 ? TABDELTA : 0 ;
	for(i=0; i<tw->tabs.numRows; ++i, y += th)
	{
	  for(j=0, childP=tw->composite.children;
		j < tw->composite.num_children;
		++j, ++childP)
	  {
	    tab = (MwTabsConstraints)(*childP)->core.constraints;
	    if( tab->tabs.row == i && *childP != tw->tabs.topWidget )
	      DrawTab(tw, *childP, labels) ;
	  }
	  if( i != tw->tabs.numRows -1 )
	    DrawTrim(tw, 0,y+th, tw->core.width, th+s, False,False) ;
	}

	DrawFrame(tw) ;

	/* and now the top tab */
	if( tw->tabs.topWidget != NULL )
	  DrawTab(tw, tw->tabs.topWidget, labels) ;
}



/* Draw one tab.  Corners are rounded very slightly. */

static	void
DrawTab(tw, child, labels)
	MwTabsWidget	tw ;
	Widget	child ;
	int	labels ;	/* draw label too? */
{
	GC	gc ;

	DrawBorder(tw, child, False) ;

	if( labels )
	{
	  MwTabsConstraints tab = (MwTabsConstraints)child->core.constraints;
	  Display	*dpy = XtDisplay((Widget)tw) ;
	  Window	win = XtWindow((Widget)tw) ;
	  String	lbl = tab->tabs.label != NULL ?
			      tab->tabs.label : XtName(child) ;

	  if( XtIsSensitive(child) )
	  {
	    gc = tw->tabs.foregroundGC ;
	    XSetForeground(dpy, gc, tab->tabs.foreground) ;
	  }
	  else
	  {
	    /* grey pixel allocation deferred until now */
	    if( !tab->tabs.greyAlloc )
	    {
	      if( tw->tabs.be_nice_to_cmap || tw->core.depth == 1 )
		tab->tabs.grey = tab->tabs.foreground ;
	      else
		tab->tabs.grey = AllocGreyPixel((Widget)tw,
					tab->tabs.foreground,
					tw->core.background_pixel,
					tw->tabs.insensitive_contrast ) ;
	      tab->tabs.greyAlloc = True ;
	    }
	    gc = tw->tabs.greyGC ;
	    XSetForeground(dpy, gc, tab->tabs.grey) ;
	  }

	  if( tab->tabs.left_bitmap != None && tab->tabs.lbm_width > 0 )
	  {
	    if( tab->tabs.lbm_depth == 1 )
	      XCopyPlane(dpy, tab->tabs.left_bitmap, win,gc,
	      	0,0, tab->tabs.lbm_width, tab->tabs.lbm_height,
		tab->tabs.x+tab->tabs.lbm_x, tab->tabs.y+tab->tabs.lbm_y, 1L) ;
	    else
	      XCopyArea(dpy, tab->tabs.left_bitmap, win,gc,
	      	0,0, tab->tabs.lbm_width, tab->tabs.lbm_height,
		tab->tabs.x+tab->tabs.lbm_x, tab->tabs.y+tab->tabs.lbm_y) ;
	  }

	  if( lbl != NULL && tw->tabs.font != NULL )
	    XDrawString(dpy,win,gc,
	      tab->tabs.x+tab->tabs.l_x, tab->tabs.y+tab->tabs.l_y,
	      lbl,strlen(lbl)) ;
	}
}


	/* draw frame all the way around the child windows. */

static	void
DrawFrame(tw)
	MwTabsWidget	tw ;
{
	GC		topgc = tw->tabs.topGC ;
	GC		botgc = tw->tabs.botGC ;
	Dimension	s = SHADWID ;
	Dimension	ch = tw->tabs.child_height ;

	Draw3dBox((Widget)tw, 0,tw->tabs.tab_total,
		tw->core.width, ch+2*s, s, topgc, botgc, topgc, botgc) ;
}


	/* draw trim around a tab or underneath a row of tabs */

static	void
DrawTrim(tw, x,y,wid,hgt, bottom, undraw)
	MwTabsWidget	tw ;			/* widget */
	Position	x,y ;			/* upper-left corner */
	int		wid,hgt ;		/* total size */
	int		bottom ;		/* draw bottom? */
	int		undraw ;		/* undraw all */
{
	Display		*dpy = XtDisplay((Widget)tw) ;
	Window		win = XtWindow((Widget)tw) ;
	GC		bggc = tw->tabs.backgroundGC ;
	GC		topgc = undraw ? bggc : tw->tabs.topGC ;
	GC		botgc = undraw ? bggc : tw->tabs.botGC ;

	if( bottom )
	  XDrawLine(dpy,win,bggc, x,y+hgt-1, x+wid-1,y+hgt-1) ;	/* bottom */
	XDrawLine(dpy,win,topgc, x,y+2, x,y+hgt-2) ;		/* left */
	XDrawPoint(dpy,win,topgc, x+1,y+1) ;			/* corner */
	XDrawLine(dpy,win,topgc, x+2,y, x+wid-3,y) ;		/* top */
	XDrawLine(dpy,win,botgc, x+wid-2,y+1, x+wid-2,y+hgt-2) ; /* right */
	XDrawLine(dpy,win,botgc, x+wid-1,y+2, x+wid-1,y+hgt-2) ; /* right */
}


/* Draw one tab border. */

static	void
DrawBorder(tw, child, undraw)
	MwTabsWidget	tw ;
	Widget		child ;
	int		undraw ;
{
	MwTabsConstraints tab = (MwTabsConstraints)child->core.constraints;
	Position	x = tab->tabs.x ;
	Position	y = tab->tabs.y ;
	Dimension	twid = tab->tabs.width ;
	Dimension	thgt = tw->tabs.tab_height ;

	/* top tab requires a little special attention; it overlaps
	 * neighboring tabs slightly, so the background must be cleared
	 * in the region of the overlap to partially erase those neighbors.
	 * TODO: is this worth doing with regions instead?
	 */
	if( child == tw->tabs.topWidget )
	{
	  Display	*dpy = XtDisplay((Widget)tw) ;
	  Window	win = XtWindow((Widget)tw) ;
	  GC		bggc = tw->tabs.backgroundGC ;
	  XRectangle	rects[3] ;
	  x -= TABDELTA ;
	  y -= TABDELTA ;
	  twid += TABDELTA*2 ;
	  thgt += TABDELTA ;
	  AddRect(0, x,y+1,twid,TABDELTA) ;
	  AddRect(1, x+1,y,TABDELTA,thgt) ;
	  AddRect(2, x+twid-TABDELTA-1,y,TABDELTA,thgt) ;
	  XFillRectangles(dpy,win,bggc, rects, 3) ;
	}

	DrawTrim(tw, x,y,twid,thgt+1, child == tw->tabs.topWidget, undraw) ;
}





	/* GEOMETRY UTILITIES */


	/* Compute the size of a child's tab.  Positions will be computed
	 * elsewhere.
	 *
	 *	height: font height + vertical_space*2 + shadowWid*2
	 *	width:	string width + horizontal_space*2 + shadowWid*2
	 *
	 * All tabs are the same height, so this is computed elsewhere.
	 */

static	void
TabWidth(w)
	Widget	w ;
{
	MwTabsConstraints tab = (MwTabsConstraints) w->core.constraints ;
	MwTabsWidget	tw = (MwTabsWidget)XtParent(w) ;
	String		lbl = tab->tabs.label != NULL ?
				tab->tabs.label : XtName(w) ;
	XFontStruct	*font = tw->tabs.font ;
	int		iw = tw->tabs.internalWidth ;

	tab->tabs.width = iw + SHADWID*2 ;
	tab->tabs.l_x = tab->tabs.lbm_x = SHADWID + iw ;

	if( tab->tabs.left_bitmap != None )
	{
	  tab->tabs.width += tab->tabs.lbm_width + iw ;
	  tab->tabs.l_x += tab->tabs.lbm_width + iw ;
	  tab->tabs.lbm_y = (tw->tabs.tab_height - tab->tabs.lbm_height)/2 ;
	}

	if( lbl != NULL && font != NULL )
	{
	  tab->tabs.width += XTextWidth(font, lbl, strlen(lbl)) + iw ;
	  tab->tabs.l_y = (tw->tabs.tab_height +
	 	 tw->tabs.font->max_bounds.ascent -
		 tw->tabs.font->max_bounds.descent)/2 ;
	}
}



	/* Lay out tabs to fit in given width.  Compute x,y position and
	 * row number for each tab.  Return number of rows and total height
	 * required by all tabs.  If there is only one row, add TABDELTA
	 * height to the total.  Rows are assigned bottom to top.
	 *
	 * MwTabs are indented from the edges by INDENT.
	 *
	 * TODO: if they require more than two rows and the total height:width
	 * ratio is more than 2:1, then try something else.
	 */

static	int
TabLayout(tw, wid, reply_height, query_only)
	MwTabsWidget	tw;
	int		wid ;		/* amount of width to work with. */
	Dimension	*reply_height;	/* total height of tabs, may be NULL */
	int		query_only ;	/* just want #rows & total height */
{
	int		i, row ;
	int		num_children = tw->composite.num_children ;
	Widget		*childP ;
	Dimension	w ;
	Position	x,y ;
	MwTabsConstraints	tab ;

	/* Algorithm: loop through children, assign X positions.  If a tab
	 * would extend beyond the right edge, start a new row.  After all
	 * rows are assigned, make a second pass and assign Y positions.
	 */

	if( num_children > 0 )
	{
	  /* Loop through the tabs and see how much space they need. */

	  row = 0 ;
	  x = INDENT ;
	  y = 0 ;
	  wid -= INDENT ;
	  for(i=num_children, childP=tw->composite.children; --i >= 0; ++childP)
	  {
	    tab = (MwTabsConstraints) (*childP)->core.constraints ;
	    w = tab->tabs.width ;
	    if( x + w > wid ) {			/* new row */
	      ++row ;
	      x = INDENT ;
	      y += tw->tabs.tab_height ;
	    }
	    if( !query_only ) {
	      tab->tabs.x = x ;
	      tab->tabs.y = y ;
	      tab->tabs.row = row ;
	    }
	    x += w + SPACING ;
	  }
	  /* If there was only one row, increse the height by TABDELTA */
	  if( ++row == 1 )
	  {
	    y = TABDELTA ;
	    if( !query_only )
	      for(i=num_children, childP=tw->composite.children;
		    --i >= 0 ; ++childP)
	      {
		tab = (MwTabsConstraints) (*childP)->core.constraints ;
		tab->tabs.y = y ;
	      }
	  }
	  y += tw->tabs.tab_height ;
	}
	else
	  row = y = 0 ;

	if( !query_only ) {
	  tw->tabs.tab_total = y ;
	  tw->tabs.numRows = row ;
	}

	if( reply_height != NULL )
	  *reply_height = y ;

	return row ;
}



	/* Find max preferred child size.  Returned sizes include child
	 * border widths. */

static	void
MaxChild(tw, reply_cw, reply_ch, except)
	MwTabsWidget	tw;
	Dimension	*reply_cw, *reply_ch;	/* child widget size */
	Widget		except ;		/* ignore this child */
{
	XtWidgetGeometry	preferred ;
	Dimension	cw,ch ;	/* child width, height */
	int		i ;
	int		num_children = tw->composite.num_children ;
	Widget		*childP ;
	MwTabsConstraints	tab ;


	cw = ch = 0 ;

	for(i=0, childP=tw->composite.children; i < num_children; ++i, ++childP)
	{
	  tab = (MwTabsConstraints) (*childP)->core.constraints ;
	  if( *childP != except )
	  {
	    (void) XtQueryGeometry(*childP, NULL, &preferred) ;
	    tab->tabs.bwid = preferred.border_width ;

	    cw = Max(cw, preferred.width + 2*preferred.border_width) ;
	    ch = Max(ch, preferred.height + 2*preferred.border_width) ;
	  }
	}

	*reply_cw = cw ;
	*reply_ch = ch ;
}



	/* rotate row numbers to bring current widget to bottom row,
	 * compute y positions for all tabs
	 */

static	void
TabsShuffleRows(tw)
	MwTabsWidget	tw;
{
	MwTabsConstraints	tab ;
	int		move ;
	int		nrows ;
	Widget		*childP ;
	Dimension	th = tw->tabs.tab_height ;
	Position	bottom ;
	int		i ;

	/* There must be a top widget.  If not, assign one. */
	if( tw->tabs.topWidget == NULL && tw->composite.children != NULL )
	  tw->tabs.topWidget = *tw->composite.children ;

	if( tw->tabs.topWidget != NULL )
	{
	  nrows = tw->tabs.numRows ;
	  assert( nrows > 0 ) ;

	  if( nrows > 1 )
	  {
	    tab = (MwTabsConstraints) tw->tabs.topWidget->core.constraints ;
	    assert( tab != NULL ) ;

	    /* how far to move top row */
	    move = nrows - tab->tabs.row ;
	    bottom = tw->tabs.tab_total - th ;

	    for(i=tw->composite.num_children, childP=tw->composite.children;
		  --i >= 0;
		  ++childP)
	    {
	      tab = (MwTabsConstraints) (*childP)->core.constraints ;
	      tab->tabs.row = (tab->tabs.row + move) % nrows ;
	      tab->tabs.y = bottom - tab->tabs.row * th ;
	    }
	  }
	}
}


	/* find preferred size.  Ask children, find size of largest,
	 * add room for tabs & return.  This can get a little involved,
	 * as we don't want to have too many rows of tabs; we may widen
	 * the widget to reduce # of rows.
	 */

static	void
PreferredSize(tw, reply_width, reply_height, reply_cw, reply_ch)
	MwTabsWidget	tw;
	Dimension	*reply_width, *reply_height;	/* total widget size */
	Dimension	*reply_cw, *reply_ch;		/* child widget size */
{
	Dimension	cw,ch ;		/* child width, height */


	/* find max desired child height */
	MaxChild(tw, &cw, &ch, NULL) ;

	PreferredSize2(tw, cw,ch, reply_width, reply_height) ;

	if( reply_cw != NULL ) *reply_cw = cw ;
	if( reply_ch != NULL ) *reply_ch = ch ;
}


	/* Find preferred size, given size of children. */

static	void
PreferredSize2(tw, cw,ch, reply_width, reply_height)
	MwTabsWidget	tw;
	int		cw,ch ;		/* child width, height */
	Dimension	*reply_width, *reply_height;	/* total widget size */
{
	Dimension	th ;		/* space used by tabs */
	Dimension	wid,hgt ;	/* total size */
	Dimension	s = SHADWID ;
	int		nrows ;


	/* make room for shadow frame */
	cw += s*2 ;
	ch += s*2 ;

	if( tw->composite.num_children > 0 )
	  nrows = TabLayout(tw, cw, &th, True) ;
	else
	  th = 0 ;

	/* TODO: check for absurd results (more than 2 rows, high aspect
	 * ratio).  Try wider size if needed.
	 */

	wid = Max(cw, MIN_WID) ;
	hgt = Max(th+ch, MIN_HGT) ;

	*reply_width = wid ;
	*reply_height = hgt ;
}


static	void
getBitmapInfo(tw, tab)
	MwTabsWidget	tw ;
	MwTabsConstraints	tab ;
{
	Window root ;
	int	x,y ;
	unsigned int bw ;

	if( tab->tabs.left_bitmap == None  ||
	    !XGetGeometry(XtDisplay(tw), tab->tabs.left_bitmap, &root, &x, &y,
	    	&tab->tabs.lbm_width, &tab->tabs.lbm_height,
		&bw, &tab->tabs.lbm_depth) )
	  tab->tabs.lbm_width = tab->tabs.lbm_height = 0 ;
}




	/* Code copied & modified from Gcs.c.  This version has dynamic
	 * foreground.
	 */

static	void
TabsAllocFgGC(tw)
	MwTabsWidget	tw;
{
	Widget		w = (Widget) tw;
	XGCValues	values ;

	values.background = tw->core.background_pixel ;
	values.font = tw->tabs.font->fid ;

	tw->tabs.foregroundGC =
	  XtAllocateGC(w, w->core.depth,
	    GCBackground|GCFont, &values,
	    GCForeground,
	    GCSubwindowMode|GCGraphicsExposures|GCDashOffset|
		GCDashList|GCArcMode) ;
}

static	void
TabsAllocGreyGC(tw)
	MwTabsWidget	tw;
{
	Widget		w = (Widget) tw;
	XGCValues	values ;

	values.background = tw->core.background_pixel ;
	values.font = tw->tabs.font->fid ;

	if( tw->tabs.be_nice_to_cmap || w->core.depth == 1)
	{
	  values.fill_style = FillStippled ;
	  values.stipple = GetGrey50((Widget)tw, &tw->tabs.grey50) ;

	  tw->tabs.greyGC =
	    XtAllocateGC(w, w->core.depth,
	      GCBackground|GCFont|GCStipple|GCFillStyle, &values,
	      GCForeground,
	      GCSubwindowMode|GCGraphicsExposures|GCDashOffset|
		  GCDashList|GCArcMode) ;
	}
	else
	{
	  tw->tabs.greyGC =
	    XtAllocateGC(w, w->core.depth,
	      GCFont, &values,
	      GCForeground,
	      GCBackground|GCSubwindowMode|GCGraphicsExposures|GCDashOffset|
		  GCDashList|GCArcMode) ;
	}
}

#if XtSpecificationRelease < 5

static	GC
XtAllocateGC(w, depth, mask, values, dynamic, dontcare)
	Widget		w ;
	int		depth ;
	unsigned long	mask, dynamic, dontcare ;
	XGCValues	*values ;
{
	return XtGetGC(w, mask, values) ;
}

#endif
