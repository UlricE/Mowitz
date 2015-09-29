/* $Id: MwRuler.c,v 1.1.1.1 2003/01/16 20:31:16 ulric Exp $
 *
 * Ruler.c - Ruler widget
 *
 * Author: Edward A. Falk
 *	   falk@falconer.vip.best.com
 *
 * Date: July, 1999
 *
 * $Log: MwRuler.c,v $
 * Revision 1.1.1.1  2003/01/16 20:31:16  ulric
 * Mowitz
 *
 * Revision 1.2  1999/08/31 19:44:36  falk
 * fixed bug during initial sizing
 * no longer uses regions
 * now uses GraphicsExposure
 *
 * Revision 1.1  1999/08/27 18:33:57  falk
 * Initial revision
 *
 *
 */

/* TODO:
 *  RulerSetMin(rw, -5.) screws up.
 *  Graphics exposure?
 */

/* General notes:
 *
 * The ruler dimensions, etc. are represented as floating-point
 * numbers.  These must be converted to integers to pass to Xlib.
 *
 * The obvious way to do this is to compute
 *
 *	x = x0 + (v - vmin) * scale
 *
 * where x0 is the pixel coordinate of the left edge of the ruler,
 * vmin is the ruler value at x0, and scale is the number of pixels
 * per ruler unit.
 *
 * Unfortunately, this formula contains the potential for round-off
 * error.  When vmin changes, there is a chance for the x value
 * to be off by one pixel.  In practice, this means that if you
 * slide the ruler back and forth long enough, eventually you will
 * see a label or tic mark drawn double because of a one-pixel offset.
 *
 * Here is a concrete example:
 *	vmin = -1.5
 *	scale = 100
 *	x0 = 0
 *	find: label for v=-1.:
 *
 *	x = (-1 - -1.5) * 100 = 50
 *
 *	scroll right 1 pixel:
 *	new vmin = -1.5099999
 *	x = (-1 - -1.50999) * 100 = 50	<= round off error!
 *
 *
 * One solution is to compute:
 *
 *	x = l0 + v * scale
 *
 * where l0 is the computed pixel location of ruler value 0.:
 *
 *	l0 = x0 + (0. - vmin) * scale
 *
 * As long as l0 is not recomputed, or is recomputed using only
 * integer arithmatic, or a complete redraw is executed after
 * computing l0, then round-off error will no longer be a problem.
 *
 * One problem remains:  The domain of float numbers is much larger
 * than for integers.  It is possible that for very large magnitude
 * values of vmin, l0 may overflow.  We can reduce, but not solve the
 * problem by representing l0 with the largest integers available.
 *
 * A better solution is for l0 to represent not the pixel value of 0.,
 * but of some other value, such as an arbitrary point within a million
 * pixels of the ruler.  This would require recomputing l0 every time
 * its magnitude became too great.  The formula would now look like:
 *
 *	x = l0 + (v - v0) * scale
 *
 * where v0 is the ruler value which lies at l0.  If we use vmin for the
 * initial value of v0, then the initial value of l0 is simply x0.
 *
 * As long as changes to l0 and v0 are made with integer math, or a
 * complete redraw is executed after recomputing l0 and v0, there
 * should be no problem with round-off error.
 */

#include	<stdio.h>
#include	<ctype.h>
#include	<string.h>
#include	<stdlib.h>
#include	<limits.h>

#include	<X11/cursorfont.h>
#include	<X11/IntrinsicP.h>
#include	<X11/StringDefs.h>
#include	<X11/Xmu/Misc.h>
#include	<X11/Xmu/CharSet.h>
#include	<X11/Xaw/XawInit.h>
#include	"MwRulerP.h"
#include	"MwGcs.h"

#define	RULER_MIN	4	/* min space between tic marks */
#define	POINTER_SIZE	7
#define	PS2		(POINTER_SIZE*2+1)
#define	TIC_LEN		12	/* longest tic mark */
#define	IMARGIN		6	/* internal margin between tics & text */

#define	MAX_L0		(LONG_MAX/1024)	/* arbitrary, really */



/****************************************************************
 *
 * Ruler Resources
 *
 ****************************************************************/

static	char	defaultTranslations[] =
	"<BtnDown>:	StartScroll()\n\
	 <BtnMotion>:	Drag()\n\
	 <BtnUp>:	EndScroll()\n\
	 <KeyDown>Page_Up:	Scroll(-page)\n\
	 <KeyDown>Page_Down:	Scroll(page)\n\
	 Shift<KeyDown>Left:	Scroll(-page)\n\
	 Shift<KeyDown>KP_Left:	Scroll(-page)\n\
	 Shift<KeyDown>Up:	Scroll(-page)\n\
	 Shift<KeyDown>KP_Up:	Scroll(-page)\n\
	 Shift<KeyDown>Right:	Scroll(page)\n\
	 Shift<KeyDown>KP_Right: Scroll(page)\n\
	 Shift<KeyDown>Down:	Scroll(page)\n\
	 Shift<KeyDown>KP_Down:	Scroll(page)\n\
	 Ctrl<KeyDown>Left:	Scroll(-1)\n\
	 Ctrl<KeyDown>KP_Left:	Scroll(-1)\n\
	 Ctrl<KeyDown>Up:	Scroll(-1)\n\
	 Ctrl<KeyDown>KP_Up:	Scroll(-1)\n\
	 Ctrl<KeyDown>Right:	Scroll(1)\n\
	 Ctrl<KeyDown>KP_Right:	Scroll(1)\n\
	 Ctrl<KeyDown>Down:	Scroll(1)\n\
	 Ctrl<KeyDown>KP_Down:	Scroll(1)\n\
	 <KeyDown>Left:		Scroll(-tic)\n\
	 <KeyDown>KP_Left:	Scroll(-tic)\n\
	 <KeyDown>Up:		Scroll(-tic)\n\
	 <KeyDown>KP_Up:	Scroll(-tic)\n\
	 <KeyDown>Right:	Scroll(tic)\n\
	 <KeyDown>KP_Right:	Scroll(tic)\n\
	 <KeyDown>Down:		Scroll(tic)\n\
	 <KeyDown>KP_Down:	Scroll(tic)\n\
";


static	float	float0 = 0.0 ;
static	float	float100 = 100.0 ;

#define	offset(field)	XtOffsetOf(MwRulerRec, ruler.field)
static XtResource resources[] = {

  {XtNmarkStep, XtCStep, XtRInt, sizeof(int),
      offset(markStep), XtRImmediate, (XtPointer) 1},
  {XtNmarkDiv, XtCDivisions, XtRInt, sizeof(int),
      offset(markDiv), XtRImmediate, (XtPointer) 32},
  {XtNlabelStep, XtCStep, XtRInt, sizeof(int),
      offset(labelStep), XtRImmediate, (XtPointer) 1},
  {XtNlabelDiv, XtCDivisions, XtRInt, sizeof(int),
      offset(labelDiv), XtRImmediate, (XtPointer) 32},
  {XtNlabelStyle, XtCLabelStyle, XtRLabelStyle, sizeof(XtLabelStyle),
      offset(labelStyle), XtRImmediate, (XtPointer) DECIMAL},
  {XtNorientation, XtCOrientation, XtRGravity, sizeof(int),
      offset(orientation), XtRImmediate, (XtPointer) NorthGravity},
  {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
       offset(foreground), XtRString, XtDefaultForeground},
  {XtNpointerColor, XtCForeground, XtRPixel, sizeof(Pixel),
       offset(pointerColor), XtRString, XtDefaultForeground},
  {XtNshowPointer, XtCShowPointer, XtRBoolean, sizeof(Boolean),
       offset(showPointer), XtRImmediate, (XtPointer)False},
  {XtNminValue, XtCMinValue, XtRFloat, sizeof(float),
       offset(minValue), XtRFloat, (XtPointer)&float0},
  {XtNvalue, XtCValue, XtRFloat, sizeof(float),
       offset(value), XtRFloat, (XtPointer)&float0},
  {XtNscale, XtCScale, XtRFloat, sizeof(float),
       offset(scale), XtRFloat, (XtPointer)&float100},
  {XtNiValue, XtCIValue, XtRInt, sizeof(int),
      offset(iValue), XtRImmediate, (XtPointer) 0},
  { XtNfont, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
	offset(font), XtRString, XtDefaultFont},
  { XtNfracFont, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
	offset(fracFont), XtRString, NULL},
  { XtNcursor, XtCCursor, XtRCursor, sizeof(Cursor),
	offset(cursor), XtRImmediate, NULL},
  { XtNcallback, XtCCallback, XtRCallback, sizeof(XtPointer),
         offset(callbacks), XtRCallback, NULL},
  {XtNuserData, XtCUserData, XtRPointer, sizeof(XtPointer),
      offset(userData), XtRImmediate, NULL},
};
#undef	offset



#ifdef	__STDC__

	/* member functions */
static	void	RulerClassInit() ;
static	void	RulerInit( Widget req, Widget new, ArgList, Cardinal *) ;
static	void	RulerRealize( Widget, Mask *, XSetWindowAttributes *) ;
static	void	RulerDestroy( Widget ) ;
static	void	RulerResize( Widget ) ;
static	void	RulerExpose( Widget, XEvent *event, Region region) ;
static	Boolean	RulerSetValues( Widget, Widget, Widget, ArgList, Cardinal *) ;
static	XtGeometryResult RulerQueryGeometry( Widget,
		    XtWidgetGeometry *intended, XtWidgetGeometry *preferred) ;

	/* actions */
static	void	StartScroll( Widget, XEvent *, String *, Cardinal *) ;
static	void	Drag( Widget, XEvent *, String *, Cardinal *) ;
static	void	EndScroll( Widget, XEvent *, String *, Cardinal *) ;
static	void	Scroll( Widget, XEvent *, String *, Cardinal *) ;
static	void	ScrollTo( Widget, XEvent *, String *, Cardinal *) ;


	/* internal privates */

static	void	PreferredSize( MwRulerWidget, Dimension *wid, Dimension *hgt) ;
static	void	RulerStepDiv(MwRulerWidget) ;
static	void	RulerDraw( MwRulerWidget, int x1, int x2) ;
static	void	drawTics( MwRulerWidget, int, int, int, int, int, int, int) ;
static	void	drawTic( MwRulerWidget, int x, int, int, int x1, int x2) ;
static	void	drawLabel( MwRulerWidget, int, int, String, GC, XFontStruct *) ;
static	void	indexPmPos(MwRulerWidget, Position *, Position *,
				Dimension *, Dimension *) ;
static	void	drawPointer( MwRulerWidget ) ;
static	void	undrawPointer( MwRulerWidget ) ;
static	void	iScroll( MwRulerWidget, int dp) ;
static	void	ExtractPosition ( XEvent *event, Position *x, Position *y) ;
static	void	RulerAllocGCs( MwRulerWidget ) ;
static	void	RulerFreeGCs( MwRulerWidget ) ;
static	void	RulerAllocFgGC( MwRulerWidget ) ;
static	void	RulerAllocPointerGC( MwRulerWidget ) ;
static	Boolean	CvtStringToLabelStyle( Display *, XrmValuePtr, Cardinal *,
			XrmValuePtr from, XrmValuePtr to, XtPointer *) ;

static	int	ifloor( double a) ;
static	int	iceil( double a) ;
static	void	fracStr( MwRulerWidget, char line[], int v, int n, int d, int s) ;
static	int	fracWid(MwRulerWidget, int v, int divisions) ;
static	int	rulerWid(MwRulerWidget, int v, int divisions) ;
static	int	minStep( int step, int sm, double scale) ;
static	int	maxDiv( MwRulerWidget, int d, int wid, int (*func)(), int a) ;

#else	/* K&R C */


	/* member functions */

static	void	RulerClassInit();
static	void	RulerInit();
static	void	RulerResize();
static	void	RulerExpose();
static	void	RulerDestroy();
static	void	RulerRealize();
static	Boolean	RulerSetValues();

	/* action procs */

static	void	StartScroll(), Drag(), EndScroll() ;
static	void	Scroll() ;
static	void	ScrollTo() ;

	/* internal privates */

static	void	RulerStepDiv() ;
static	void	RulerDraw() ;
static	void	drawTics() ;
static	void	drawTic() ;
static	void	drawLabel() ;
static	void	indexPmPos() ;
static	void	drawPointer() ;
static	void	undrawPointer() ;
static	void	RulerAllocGCs() ;	/* get rendering GCs */
static	void	RulerFreeGCs() ;	/* return rendering GCs */
static	void	PreferredSize() ;	/* compute preferred size */
static	int	RulerLayout() ;		/* lay out ruler */

static	void	RulerDraw3dBox() ;

static	void	RulerAllocFgGC() ;
static	void	RulerAllocPointerGC() ;
static	void	assignTicHgt() ;
static	int	minStep() ;
static	int	fracWid() ;
static	int	rulerWid() ;
static	int	maxDiv() ;
static	void	iScroll() ;
static	void	ExtractPosition() ;
static	Boolean	CvtStringToLabelStyle() ;
static	int	ifloor() ;
static	int	iceil() ;
static	void	fracStr() ;

#endif


#ifndef	max
#define	max(a,b) (((a) > (b)) ? (a) : (b))
#define	min(a,b) (((a) < (b)) ? (a) : (b))
#define	abs(a)	(((a) < 0) ? -(a) : (a))
#endif


#define	fontAscent(f)	((f != NULL) ? (f)->ascent : 0)



static	XtActionsRec	actionsList[] =
  {
    {"StartScroll",	StartScroll},
    {"Drag",		Drag},
    {"EndScroll",	EndScroll},
    {"Scroll",		Scroll},
    {"ScrollTo",	ScrollTo},
  } ;


/****************************************************************
*
* Full class record constant
*
****************************************************************/

#define	SuperClass	(&coreClassRec)

MwRulerClassRec mwRulerClassRec = {
  {
/* core_class fields      */
    /* superclass         */    (WidgetClass) SuperClass,
    /* class_name         */    "MwRuler",
    /* widget_size        */    sizeof(MwRulerRec),
    /* class_initialize   */    RulerClassInit,
    /* class_part_init    */	NULL,			/* TODO? */
    /* class_inited       */	FALSE,
    /* initialize         */    RulerInit,
    /* initialize_hook    */	NULL,
    /* realize            */    RulerRealize,
    /* actions            */    actionsList,
    /* num_actions	  */	XtNumber(actionsList),
    /* resources          */    resources,
    /* num_resources      */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion	  */	TRUE,
    /* compress_exposure  */	XtExposeCompressSeries|XtExposeGraphicsExposeMerged|XtExposeNoRegion,
    /* compress_enterleave*/	TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    RulerDestroy,
    /* resize             */    RulerResize,
    /* expose             */    RulerExpose,
    /* set_values         */    RulerSetValues,
    /* set_values_hook    */	NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */	NULL,
    /* accept_focus       */    NULL,
    /* version            */	XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    defaultTranslations,
    /* query_geometry     */	RulerQueryGeometry,
    /* display_accelerator*/	XtInheritDisplayAccelerator,
    /* extension          */	NULL
  },
  {
/* Ruler class fields */
    /* extension	  */	NULL,
  }
};

WidgetClass mwRulerWidgetClass = (WidgetClass)&mwRulerClassRec;


#ifdef	DEBUG
#ifdef	__STDC__
#define	assert(e) \
	  if(!(e)) fprintf(stderr,"yak! %s at %s:%d\n",#e,__FILE__,__LINE__)
#else
#define	assert(e) \
	  if(!(e)) fprintf(stderr,"yak! e at %s:%d\n",__FILE__,__LINE__)
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
RulerClassInit()
{
    XtAddConverter( XtRString, XtRGravity, XmuCvtStringToGravity,
	NULL, (Cardinal)0) ;
    XtSetTypeConverter( XtRString, XtRLabelStyle, CvtStringToLabelStyle,
	NULL, (Cardinal)0, XtCacheNone, NULL) ;
}



	/* Init a newly created ruler widget.  Compute height of ruler
	 * and optionally compute size of widget. */


static void
RulerInit(request, new, args, num_args)
    Widget request, new;
    ArgList args;
    Cardinal *num_args;
{
    MwRulerWidget newRw = (MwRulerWidget)new;

    /* GC allocation is deferred until XtRealize() */

    newRw->ruler.foregroundGC =
    newRw->ruler.fracGC =
    newRw->ruler.pointerGC = None ;

    newRw->ruler.needs_layout = True ;

    if( newRw->ruler.fracFont == NULL )
      newRw->ruler.fracFont = newRw->ruler.font ;

    switch(newRw->ruler.orientation) {
      case NorthWestGravity:
      case NorthEastGravity:
        newRw->ruler.orientation = NorthGravity ;
	break ;
      case SouthWestGravity:
      case SouthEastGravity:
        newRw->ruler.orientation = SouthGravity ;
	break ;
    }

    /* if size not explicitly set, set it to our preferred size now. */

    if( request->core.width == 0 || request->core.height == 0 )
    {
      Dimension	w=100,h=100 ;
      PreferredSize(newRw, &w, &h) ;
      if( request->core.width == 0 ) new->core.width = w ;
      if( request->core.height == 0 ) new->core.height = h ;
      XtClass(new)->core_class.resize(new) ;
    }
}




	/* Called when ruler widget first realized.  Create the window
	 * and allocate the GCs
	 */

static	void
RulerRealize(w, valueMask, attributes)
	Widget w;
	Mask *valueMask;
	XSetWindowAttributes *attributes;
{
	MwRulerWidget rw = (MwRulerWidget) w;
	Display	*dpy = XtDisplay(w) ;

	(*SuperClass->core_class.realize)(w, valueMask, attributes) ;

	RulerAllocGCs(rw) ;

	if( rw->ruler.cursor == None )
	  switch( rw->ruler.orientation ) {
	    case NorthGravity:
	    case SouthGravity:
	      rw->ruler.cursor = XCreateFontCursor(dpy, XC_sb_h_double_arrow);
	      break ;
	    case WestGravity:
	    case EastGravity:
	      rw->ruler.cursor = XCreateFontCursor(dpy, XC_sb_v_double_arrow);
	      break ;
	  }

	if( XtIsSensitive(w) )
	  XDefineCursor(dpy, XtWindow(w), rw->ruler.cursor) ;
	rw->ruler.indexPm = XCreatePixmap( dpy, XtWindow(w),
		PS2, PS2, w->core.depth ) ;
	rw->ruler.indexSaved = False ;
}



static	void
RulerDestroy(w)
	Widget	w ;
{
	MwRulerWidget rw = (MwRulerWidget)w ;
	RulerFreeGCs(rw) ;
	XFreePixmap(XtDisplay(w), rw->ruler.indexPm) ;
}


	/* Parent has resized us.  This will require that the ruler be
	 * laid out again.
	 */

static void
RulerResize(w)
	Widget	w;
{
	MwRulerWidget	rw = (MwRulerWidget) w;
	double		v0 = rw->ruler.minValue ;
	XRectangle	rect ;

	RulerStepDiv(rw) ;

	rect.x = rect.y = 0 ;
	rect.width = rw->core.width ;
	rect.height = rw->core.height ;

	rw->ruler.v0 = v0 ;
	rw->ruler.l0 = 0 ;

	rw->ruler.needs_layout = False ;
}



	/* utility: generate a label.  Value is v + n/d, expressed
	 * as either a fraction or a decimal.  V might be negative;
	 * numerator,denominator are not.
	 *
	 * Three cases:  Whole number, decimal output, fraction output
	 */

static	void
fracStr(rw, line, v, numerator,denominator, sign)
	MwRulerWidget	rw ;
	char		line[128] ;
	int		v, numerator, denominator ;
	int		sign ;
{
	if( numerator >= denominator ) {
	  v += numerator/denominator ;
	  numerator %= denominator ;
	}
	if( rw->ruler.labelStyle == FRACTION && v*sign < 0 && numerator > 0 )
	  numerator = denominator - numerator ;

	if( numerator == 0 || denominator <= 0 )
	  sprintf(line, "%d", v) ;
	else if( rw->ruler.labelStyle == DECIMAL )
	  sprintf(line, "%g", v+(double)sign*numerator/denominator) ;
	else
	{
	  int i, j, k ;
	  i = numerator; j=denominator ;
	  while( (k=i%j) > 0 ) {i=j; j=k;}	/* GCD */
	  sprintf(line, "%d/%d", numerator/j,denominator/j) ;
	}
}


	/* utility: return the width that a fractional label will require */

static	int
fracWid(rw, v, divisions)
	MwRulerWidget	rw ;
	int		v, divisions ;
{
	char	line[128] ;

	fracStr(rw, line, v, divisions-1,divisions, 1) ;
	return 2 * XTextWidth(rw->ruler.fracFont, line, strlen(line)) ;
}


static	int
rulerWid(rw, v, divisions)
	MwRulerWidget	rw ;
	int		v, divisions ;
{
	return v ;
}


	/* Utility: increase step until it's more than sm */

static	int
minStep(step, sm, scale)
	int	step, sm ;
	double	scale ;
{
	while( step*scale < sm ) {
	  step *= 2 ;
	  if( step*scale < sm )
	    step = step*5/2 ;
	  if( step*scale < sm )
	    step *= 2 ;
	}
	return step ;
}

	/* Utility: divide divisions until func()*divisions <= scale
	 * where func() returns the space required by a single
	 * tic mark or label.
	 */

static	int
maxDiv(rw, divisions, width, func, arg)
	MwRulerWidget rw ;
	int	divisions ;	/* initial # of divisions */
	int	width ;		/* ruler scale factor, pixels */
	int	(*func)() ;	/* returns width of one division */
	int	arg ;		/* passed to func */
{
	int	i ;

	/* This is more complicated than you'd think it should
	 * be.  We need to reduce the number of divisions to
	 * make room between the tic marks.  Try dividing divisions
	 * by small reasonable numbers (where reasonable is defined
	 * to mean that it divides evenly) until we have enough
	 * room.  If that doesn't work, divide by *something* and
	 * try again.
	 */


	for(;;)
	{
	  for(i=2; i<=10; ++i)
	    if( divisions % i == 0 &&
		func(rw, arg, divisions/i)*divisions/i <= width )
	      return divisions/i ;

	  /* can't keep trying small integers forever, try scaling
	   * it down.
	   */

	  if( divisions % 2 == 0 )
	    divisions /= 2 ;
	  else if( divisions % 3 == 0 )
	    divisions /= 3 ;
	  else if( divisions % 5 == 0 )
	    divisions /= 5 ;
	  else if( divisions % 7 == 0 )
	    divisions /= 7 ;
	  else if( divisions % 11 == 0 )
	    divisions /= 11 ;
	  else
	    divisions /= 2 ;	/* just force it */
	}
}



	/* Redraw entire Ruler widget */

static	void
RulerExpose(w, event, region)
	Widget	w ;
	XEvent	*event ;
	Region	region ;
{
	MwRulerWidget	rw = (MwRulerWidget) w;
	Position	px,py ;
	Dimension	wid, hgt ;

	if( rw->ruler.needs_layout )
	  XtClass(w)->core_class.resize(w) ;

	/* Make sure we erase the index mark first */

	if( rw->ruler.indexSaved ) {
	  indexPmPos(rw, &px, &py, &wid,&hgt) ;
	  XClearArea(XtDisplay(w), XtWindow(w), px,py, wid,hgt, False) ;
	}

	RulerDraw(rw, 0,rw->ruler.length) ;
	drawPointer(rw) ;
}



static	void
RulerDraw(rw, x1,x2)
	MwRulerWidget	rw ;
	int		x1,x2 ;
{
	char	line[128] ;
	int	lstep, mstep ;
	long	x ;
	int	ty = 0,lx = 0,ly = 0;
	int	j ;
	int	x0, x3 ;
	int	ps ;				/* pixel step */
	double	scale = rw->ruler.scale ;
	double	ascale = abs(scale) ;
	int	sign = scale >= 0 ? 1 : -1 ;
	double	v0 = rw->ruler.v0 ;
	double	minv = rw->ruler.minValue ;
	int	tstep ;

	if( !XtIsRealized((Widget)rw) )
	  return ;


	/* find our maximum range */

	x0 = 0 ;
	x3 = x0 + rw->ruler.length - 1 ;

	/* figure out where labels & tic marks go */

	switch( rw->ruler.orientation ) {
	  case NorthGravity:
	    ly = 2 + fontAscent(rw->ruler.font) ;
	    ty = rw->core.height - 1 ;
	    break ;
	  case SouthGravity:
	    ly = rw->core.height - 2 ;
	    ty = 0 ;
	    break ;
	  case WestGravity:
	    ly = 2 ;
	    ty = rw->core.width - 1 ;
	    break ;
	  case EastGravity:
	    ly = rw->core.width - 2 ;
	    ty = 0 ;
	    break ;
	}

	x1 = max(x1,x0) ;
	x2 = min(x2,x3) ;

	if( x2 < x1 ) return ;


	if( (mstep = rw->ruler.mStep) > 0 )
	{
	  int	mdiv = rw->ruler.mDiv ;		/* minor tic marks/major tic */
	  double t ;
	  t = sign > 0 ? ifloor(minv/mstep)*mstep : iceil(minv/mstep)*mstep ;
	  ps = mstep * ascale ;
	  tstep = sign * mstep ;
	  do {
	    x = (t-v0) * scale ;
	    x += rw->ruler.l0 ;
	    drawTic(rw, (int)x,ty, TIC_LEN, x1,x2) ;
	    if( mdiv > 1  &&  x <= x2  &&  x+ps >= x1 )
	      drawTics(rw, (int)x,ty, ps, mdiv, TIC_LEN-2, x1,x2) ;
	    t += tstep ;
	  } while( x <= x2 ) ;
	}


	if( rw->ruler.font != NULL && (lstep = rw->ruler.lStep) > 0 )
	{
	  GC gc = rw->ruler.foregroundGC ;
	  GC fgc = rw->ruler.fracGC ;
	  int	ldiv = rw->ruler.lDiv ;		/* minor labels/major label */
	  double t ;
	  t = sign > 0 ? ifloor(minv/lstep)*lstep : iceil(minv/lstep)*lstep ;
	  ps = lstep * ascale ;
	  tstep = sign * lstep ;
	  do {
	    x = (t-v0) * scale ;
	    x += rw->ruler.l0 ;
	    sprintf(line, "%d", (int)t) ;
	    drawLabel(rw, (int)x,ly, line, gc, rw->ruler.font) ;
	    for(j=1; j<ldiv; ++j) {
	      lx = x + (ps*j)/ldiv ;
	      if( lx-rw->ruler.txtWid <= x2  &&  lx+rw->ruler.txtWid >= x1 ) {
		fracStr(rw, line, (int)t, j*lstep,ldiv, sign) ;
		drawLabel(rw, lx,ly, line, fgc, rw->ruler.fracFont) ;
	      }
	    }
	    t += tstep ;
	  } while( x <= x2 ) ;
	}
}




	/* Draw n tic marks, distributed over 'dx' pixels */

static	void
drawTics(rw, x,y,dx,n, tic_len, x1,x2)
	MwRulerWidget rw ;
	int	x, dx ;
	int	y ;
	int	n, tic_len ;
	int	x1,x2 ;
{
	int	i ;
	int	x0 ;
	int	ps, pn, pd, cnt ;	/* pixel step = ps+pn/pd */

	tic_len = max(tic_len,2) ;

	/* into how many parts do we divide this region? */

	if( n % 2 == 0 ) pd = 2 ;
	else if( n % 3 == 0 ) pd = 3 ;
	else if( n % 5 == 0 ) pd = 5 ;
	else pd = n ;

	ps = dx/pd ;
	pn = dx%pd ;

	x0 = x ;
	cnt = -pd/2 ;
	for(i=0; i<pd; ++i)
	{
	  if( i > 0 )
	    drawTic(rw, x0,y, tic_len, x1,x2) ;
	  if( pd < n )
	    drawTics(rw, x0,y,ps,n/pd, tic_len-2, x1,x2) ;
	  x0 += ps ;
	  cnt += pn ; if(cnt>=0) { cnt -= pd ; ++x0 ; }
	}
}


static	void
drawTic(rw, x,y, tic_len, x1,x2)
    MwRulerWidget	rw ;
    int		x,y, tic_len;
    int		x1,x2 ;
{
    Display	*dpy = XtDisplay((Widget)rw) ;
    Window	win = XtWindow((Widget)rw) ;
    GC		gc = rw->ruler.foregroundGC ;

    if( x < x1 || x > x2 ) return ;

    switch( rw->ruler.orientation ) {
      case NorthGravity: XDrawLine(dpy,win,gc, x,y, x,y-tic_len) ; break ;
      case SouthGravity: XDrawLine(dpy,win,gc, x,y, x,y+tic_len) ; break ;
      case EastGravity:  XDrawLine(dpy,win,gc, y,x, y+tic_len,x) ; break ;
      case WestGravity:  XDrawLine(dpy,win,gc, y,x, y-tic_len,x) ; break ;
    }
}

static	void
drawLabel(rw, x,y, str, gc, font)
    MwRulerWidget	rw ;
    int		x,y ;
    String	str ;
    GC		gc ;
    XFontStruct	*font ;
{
	Display	*dpy = XtDisplay((Widget)rw) ;
	Window	win = XtWindow((Widget)rw) ;
	int	len = strlen(str) ;
	int	wid ;

	switch( rw->ruler.orientation ) {
	  case NorthGravity:
	  case SouthGravity:
	    wid = XTextWidth(font, str, len) ;
	    XDrawString(dpy,win,gc, x-wid/2,y, str,len) ;
	    break ;
	  case WestGravity:
	    XDrawString(dpy,win,gc, y, x+fontAscent(font)/2, str,len) ;
	    break ;
	  case EastGravity:
	    wid = XTextWidth(font, str, len) ;
	    XDrawString(dpy,win,gc, y-wid, x+fontAscent(font)/2,
		str,len) ;
	    break ;
	}
}


	/* Utility: return proper base position of indexPm for ruler */

static	void
indexPmPos(rw, x, y, wid,hgt)
    MwRulerWidget	rw ;
    Position	*x, *y ;
    Dimension	*wid, *hgt ;
{
    *x = *y = rw->ruler.iValue - POINTER_SIZE ;

    switch( rw->ruler.orientation ) {
      case NorthGravity:
        *y = rw->core.height - POINTER_SIZE - 3 ;
	*wid = PS2 ;
	*hgt = POINTER_SIZE+1 ;
	break ;
      case SouthGravity:
        *y = 3 ;
	*wid = PS2 ;
	*hgt = POINTER_SIZE+1 ;
	break ;
      case WestGravity:
        *x = rw->core.width - POINTER_SIZE - 3 ;
	*wid = POINTER_SIZE+1 ;
	*hgt = PS2 ;
	break ;
      case EastGravity:
        *x = 3 ;
	*wid = POINTER_SIZE ;
	*hgt = PS2+1 ;
	break ;
    }
}


static	void
drawPointer(rw)
	MwRulerWidget	rw ;
{
    Display	*dpy = XtDisplay((Widget)rw) ;
    Window	win = XtWindow((Widget)rw) ;
    GC		gc = rw->ruler.pointerGC ;
    XPoint	pts[4] ;
    Position	x,y,d = 0;
    Dimension	wid,hgt ;

    if( rw->ruler.iValue < -POINTER_SIZE  ||  !rw->ruler.showPointer )
      return ;

    indexPmPos(rw, &x, &y, &wid,&hgt) ;

    XCopyArea(dpy,win, rw->ruler.indexPm, gc, x,y, wid, hgt, 0,0) ;
    rw->ruler.indexSaved = True ;

    x = y = rw->ruler.iValue ;

    switch( rw->ruler.orientation ) {
      case NorthGravity:
	y = rw->core.height - 3 ;
	d = POINTER_SIZE ;
	break ;
      case SouthGravity:
	y = 3 ;
	d = -POINTER_SIZE ;
	break ;
      case WestGravity:
	x = rw->core.width - 3 ;
	d = POINTER_SIZE ;
	break ;
      case EastGravity:
	x = 3 ;
	d = -POINTER_SIZE ;
	break ;
    }

    switch( rw->ruler.orientation ) {
      case NorthGravity:
      case SouthGravity:
	pts[0].x = x ;		pts[0].y = y ;
	pts[1].x = x-d ;	pts[1].y = y-d ;
	pts[2].x = x+d ;	pts[2].y = y-d ;
	pts[3].x = x ;		pts[3].y = y ;
	break ;
      case EastGravity:
      case WestGravity:
	pts[0].y = y ;		pts[0].x = x ;
	pts[1].y = y-d ;	pts[1].x = x-d ;
	pts[2].y = y+d ;	pts[2].x = x-d ;
	pts[3].y = y ;		pts[3].x = x ;
	break ;
    }

    XDrawLines(dpy, win, gc, pts, 4, CoordModeOrigin) ;
}


static	void
undrawPointer(rw)
    MwRulerWidget	rw ;
{
    Display	*dpy = XtDisplay((Widget)rw) ;
    Window	win = XtWindow((Widget)rw) ;
    GC		gc = rw->ruler.pointerGC ;
    Position	x,y ;
    Dimension	wid,hgt ;

    if( !XtIsRealized((Widget)rw) || !rw->ruler.indexSaved)
      return ;

    indexPmPos(rw, &x, &y, &wid,&hgt) ;

    XCopyArea(dpy, rw->ruler.indexPm, win, gc, 0,0, wid,hgt, x,y) ;

    rw->ruler.indexSaved = False ;
}



	/* Called when any Ruler widget resources are changed. */

static Boolean
RulerSetValues(old, request, new, args, num_args)
	Widget	old, request, new;
	ArgList	args;
	Cardinal *num_args;
{
	MwRulerWidget oldRw = (MwRulerWidget) old ;
	MwRulerWidget rw = (MwRulerWidget) new ;
	Boolean	needRedraw = False ;


	/* Changing font, scale or minValue may cause a size change */
	if( rw->ruler.font != oldRw->ruler.font  ||
	    rw->ruler.minValue != oldRw->ruler.minValue ||
	    rw->ruler.scale != oldRw->ruler.scale )
	{
	  PreferredSize(rw, &rw->core.width, &rw->core.height) ;
	  rw->ruler.needs_layout = True ;
	  needRedraw = True ;
	}

	if( rw->ruler.labelStep != oldRw->ruler.labelStep ||
	    rw->ruler.labelDiv != oldRw->ruler.labelDiv ||
	    rw->ruler.labelStyle != oldRw->ruler.labelStyle ||
	    rw->ruler.markStep != oldRw->ruler.markStep ||
	    rw->ruler.markDiv != oldRw->ruler.markDiv )
	  rw->ruler.needs_layout = needRedraw = True ;

	/* TODO: if any color changes, need to recompute GCs and redraw */

	if( rw->ruler.foreground != oldRw->ruler.foreground )
	{
	  if( XtIsRealized(new) ) {
	    XtReleaseGC(new, rw->ruler.foregroundGC) ;
	    XtReleaseGC(new, rw->ruler.fracGC) ;
	    RulerAllocFgGC(rw) ;
	  }
	  needRedraw = True ;
	}

	if( rw->ruler.pointerColor != oldRw->ruler.pointerColor )
	{
	  if( XtIsRealized(new) ) {
	    XtReleaseGC(new, rw->ruler.pointerGC) ;
	    RulerAllocPointerGC(rw) ;
	  }
	  needRedraw = True ;
	}

	if( rw->ruler.value != oldRw->ruler.value  &&
	    rw->ruler.iValue == oldRw->ruler.iValue )
	{
	  rw->ruler.iValue = (rw->ruler.value - rw->ruler.minValue) *
		rw->ruler.scale ;
	  needRedraw = True ;
	}

	if( rw->ruler.iValue != oldRw->ruler.iValue  &&
	    rw->ruler.value == oldRw->ruler.value &&
	    rw->ruler.scale > 0 )
	{
	  if( rw->ruler.scale == 0. ) rw->ruler.scale = 1.0 ;
	  rw->ruler.value = rw->ruler.iValue / rw->ruler.scale +
	  	rw->ruler.minValue ;
	  needRedraw = True ;
	}

	if( rw->core.background_pixel != oldRw->core.background_pixel ||
	    rw->core.background_pixmap != oldRw->core.background_pixmap ||
	    rw->ruler.showPointer != oldRw->ruler.showPointer )
	{
	  needRedraw = True ;
	}

	/* not allowed to change orientation */
	rw->ruler.orientation = oldRw->ruler.orientation ;

	if( rw->ruler.cursor != oldRw->ruler.cursor  ||
	    XtIsSensitive(old) != XtIsSensitive(new) )
	{
	  if( XtIsSensitive(new) )
	    XDefineCursor(XtDisplay(new), XtWindow(new), rw->ruler.cursor) ;
	  else
	    XDefineCursor(XtDisplay(new), XtWindow(new), None) ;
	}

	return needRedraw ;
}




/*
 * Return preferred size.  We only care about one dimension.
 */

static XtGeometryResult
RulerQueryGeometry(w, intended, preferred)
	Widget		w;
	XtWidgetGeometry *intended, *preferred;
{
register MwRulerWidget rw = (MwRulerWidget)w ;

	if( intended->width == w->core.width  &&
	    intended->height == w->core.height )
	  return XtGeometryNo ;

	PreferredSize(rw, &preferred->width, &preferred->height) ;

	switch( rw->ruler.orientation ) {
	  case NorthGravity:
	  case SouthGravity:
	    preferred->request_mode = CWHeight ;
	    if( !(intended->request_mode & CWHeight) ||
		intended->height >= preferred->height )
	      return XtGeometryYes;
	    break ;
	  default:
	    preferred->request_mode = CWWidth ;
	    if( !(intended->request_mode & CWWidth) ||
		intended->width >= preferred->width )
	      return XtGeometryYes;
	    break ;
	}

	return XtGeometryAlmost;
}






/****************************************************************
 *
 * Action Procedures
 *
 ****************************************************************/

	/* User clicks on ruler */

static	void
StartScroll(w, event, params, num_params)
	Widget	w ;
	XEvent	*event ;
	String	*params ;
	Cardinal *num_params ;
{
	MwRulerWidget	rw = (MwRulerWidget) w ;
	MwRulerReport	rpt ;
	Position	x,y ;
	ExtractPosition(event, &x,&y) ;
	switch( rw->ruler.orientation ) {
	  case NorthGravity:
	  case SouthGravity: rw->ruler.drag = x ; break ;
	  default: rw->ruler.drag = y ; break ;
	}

	rpt.what = START ;
	rpt.minValue = rw->ruler.minValue ;
	XtCallCallbackList(w, rw->ruler.callbacks, (XtPointer)&rpt) ;
}


	/* User drags ruler. */

static	void
Drag(w, event, params, num_params)
	Widget	w ;
	XEvent	*event ;
	String	*params ;
	Cardinal *num_params ;
{
	MwRulerWidget	rw = (MwRulerWidget) w ;
	MwRulerReport	rpt ;
	int		dp ;
	Position	x,y ;

	ExtractPosition(event, &x,&y) ;
	switch( rw->ruler.orientation ) {
	  case NorthGravity:
	  case SouthGravity:
	    rpt.dx = dp = x - rw->ruler.drag ;
	    rpt.dy = 0 ;
	    rw->ruler.drag = x ;
	    break ;
	  default:
	    rpt.dx = 0 ;
	    rpt.dy = dp = y - rw->ruler.drag ;
	    rw->ruler.drag = y ;
	    break ;
	}
	iScroll(rw,dp) ;

	rpt.what = SCROLL ;
	rpt.minValue = rw->ruler.minValue ;
	XtCallCallbackList(w, rw->ruler.callbacks, (XtPointer)&rpt) ;
}


	/* User releases ruler. */

static	void
EndScroll(w, event, params, num_params)
	Widget	w ;
	XEvent	*event ;
	String	*params ;
	Cardinal *num_params ;
{
	MwRulerWidget	rw = (MwRulerWidget) w ;
	MwRulerReport	rpt ;

	rpt.what = STOP ;
	rpt.dx = rpt.dy = 0 ;
	rpt.minValue = rw->ruler.minValue ;
	XtCallCallbackList(w, rw->ruler.callbacks, (XtPointer)&rpt) ;
}


	/* User presses a key */

static	void
Scroll(w, event, params, num_params)
	Widget	w ;
	XEvent	*event ;
	String	*params ;
	Cardinal *num_params ;
{
	MwRulerWidget	rw = (MwRulerWidget)w ;
	MwRulerReport	rpt ;
	String	amt ;
	int	delta = 1 ;	/* pixels to scroll */
	double	scale = rw->ruler.scale ;
	Boolean	doIscroll = True ;

	if( *num_params < 1 || scale == 0. ) return ;

	amt = params[0] ;

	if( *amt == '-' ) {
	  delta = -1 ;
	  ++amt ;
	}

	if( isdigit(*amt) )
	  delta *= atoi(amt) ;
	else
	  switch(*amt) {
	    case 'p': delta *= rw->ruler.length/2 ; break ;
	    case 't':
	      /* special case: we're not interested in an integer
	       * number of pixels, but rather one tic mark.  If
	       * the scale factor is an integer, then fine.  Otherwise,
	       * we'd better do a redisplay.
	       */
	      if( scale < 0 ) delta = -delta ;
	      if( scale != (int)scale ) {
		MwRulerSetMin(w, rw->ruler.minValue - delta) ;
		doIscroll = False ;
	      }
	      delta *= scale ;
	      break ;
	    default: return ;
	  }

	if( doIscroll )
	  iScroll(rw, delta) ;

	rpt.what = STOP ;
	switch( rw->ruler.orientation ) {
	  case NorthGravity:
	  case SouthGravity: rpt.dx = delta ; rpt.dy = 0 ; break ;
	  default: rpt.dx = 0 ; rpt.dy = delta ;
	}
	rpt.minValue = rw->ruler.minValue ;
	XtCallCallbackList(w, rw->ruler.callbacks, (XtPointer)&rpt) ;
}


	/* Not normally used, available to users.
	 * 1 arg: arg is minvalue, place at left/top edge
	 * 2 args: arg 2 is pixel location for minvalue
	 *   negative values are taken relative to other edge.
	 */

static	void
ScrollTo(w, event, params, num_params)
	Widget	w ;
	XEvent	*event ;
	String	*params ;
	Cardinal *num_params ;
{
	MwRulerWidget	rw = (MwRulerWidget)w ;
	MwRulerReport	rpt ;
	double		scale = rw->ruler.scale ;
	double		minValue, oldMin ;
	int		pixValue, delta ;

	if( *num_params < 1 || scale == 0. ) return ;

	minValue = atof(params[0]) ;

	if( *num_params >= 2 ) {
	  pixValue = atoi(params[1]) ;
	  if( params[1][0] == '-' )
	    pixValue += rw->ruler.length ;
	}
	else
	  pixValue = 0 ;

	oldMin = rw->ruler.minValue ;

	MwRulerSetMinPosition(w, minValue, pixValue) ;

	oldMin -= rw->ruler.minValue ;
	delta = scale * oldMin ;

	rpt.what = STOP ;
	switch( rw->ruler.orientation ) {
	  case NorthGravity:
	  case SouthGravity: rpt.dx = delta ; rpt.dy = 0 ; break ;
	  default: rpt.dx = 0 ; rpt.dy = delta ;
	}
	rpt.minValue = rw->ruler.minValue ;
	XtCallCallbackList(w, rw->ruler.callbacks, (XtPointer)&rpt) ;
}



	/* Utility: scroll ruler by an integer number of pixels */

static	void
iScroll(rw, dp)
	MwRulerWidget	rw ;
	int		dp ;
{
    Widget w = (Widget)rw ;
    double scale = rw->ruler.scale ;
    int	x1,x2 ;

    if( dp == 0 ) return ;

    if( scale != 0. )
      rw->ruler.minValue -= (double)dp/scale ;

    /* make sure our base point stays in range. */

    if( dp > MAX_L0 || dp < -MAX_L0  ||
	(rw->ruler.l0 += dp) > MAX_L0 || rw->ruler.l0 < -MAX_L0 )
    {
      /* better do a full redraw */
      MwRulerSetMin(w, rw->ruler.minValue) ;
      return ;
    }

    if( XtIsRealized((Widget)rw) )
    {
	Display	*dpy = XtDisplay(w) ;
	Window	win = XtWindow(w) ;
	GC	gc = rw->ruler.foregroundGC ;
	int	l = rw->ruler.length ;
	int	sx,sy,w,h,dx,dy ;
	int	cx,cy,cw,ch ;

	undrawPointer(rw) ;
	switch( rw->ruler.orientation ) {
	  case NorthGravity:
	  case SouthGravity:
	    dy = sy = cy = 0 ;
	    h = ch = rw->core.height ;
	    if( dp > 0 ) { cx = sx = 0 ; dx = dp ; w = l-dp ; cw = dp ; }
	    else { sx = -dp ; dx = 0 ; w = l+dp ; cw = -dp ; cx = l+dp ; }
	    x1 = cx ;
	    x2 = cx + abs(dp) ;
	    break ;
	  default:
	    dx = sx = cx = 0 ;
	    w = cw = rw->core.width ;
	    if( dp > 0 ) { cy = sy = 0 ; dy = dp ; h = l-dp ; ch = dp ; }
	    else { sy = -dp ; dy = 0 ; h = l+dp ; ch = -dp ; cy = l+dp ; }
	    x1 = cy ;
	    x2 = cy + abs(dp) ;
	    break ;
	}
	XCopyArea(dpy,win,win,gc, sx,sy, w,h, dx,dy) ;
	XClearArea(dpy,win, cx,cy,cw,ch, False) ;

	RulerDraw(rw, x1,x2) ;
	drawPointer(rw) ;
    }
}



	/* Utility: get x,y coordinates from an event */

static void
ExtractPosition (event, x, y)
    XEvent *event;
    Position *x, *y;		/* RETURN */
{
    switch( event->type ) {
    case MotionNotify:
	*x = event->xmotion.x;
	*y = event->xmotion.y;
	break;
    case ButtonPress:
    case ButtonRelease:
	*x = event->xbutton.x;
	*y = event->xbutton.y;
	break;
    case KeyPress:
    case KeyRelease:
	*x = event->xkey.x;
	*y = event->xkey.y;
	break;
    case EnterNotify:
    case LeaveNotify:
	*x = event->xcrossing.x;
	*y = event->xcrossing.y;
	break;
    default:
	*x = 0; *y = 0;
    }
}





/****************************************************************
 *
 * Public Procedures
 *
 ****************************************************************/


void
MwRulerSetMinScale(w, minValue, scale)
	Widget	w ;
	double	minValue ;
	double	scale ;
{
	MwRulerWidget rw = (MwRulerWidget)w ;

	if( !XtIsSubclass(w, mwRulerWidgetClass) ) return ;

	rw->ruler.minValue = minValue ;
	rw->ruler.scale = scale ;
	rw->ruler.needs_layout = True ;
	rw->ruler.iValue = (rw->ruler.value - minValue) * scale ;
	XClearWindow(XtDisplay(w), XtWindow(w)) ;
	XtClass(w)->core_class.expose(w,NULL,None) ;
}


void
MwRulerSetMin(w, minValue)
	Widget	w ;
	double	minValue ;
{
	MwRulerWidget rw = (MwRulerWidget)w ;

	if( !XtIsSubclass(w, mwRulerWidgetClass) ) return ;

	MwRulerSetMinScale(w, minValue, rw->ruler.scale) ;
}


void
MwRulerSetScale(w, scale)
	Widget	w ;
	double	scale ;
{
	MwRulerWidget rw = (MwRulerWidget)w ;

	if( !XtIsSubclass(w, mwRulerWidgetClass) ) return ;

	MwRulerSetMinScale(w, rw->ruler.minValue, scale) ;
}


void
MwRulerSetMinPosition(w, minValue, position)
	Widget	w ;
	double	minValue ;
	int	position ;
{
	MwRulerWidget rw = (MwRulerWidget)w ;

	if( !XtIsSubclass(w, mwRulerWidgetClass) ) return ;

	if( position != 0 && rw->ruler.scale != 0. )
	  minValue -= (double)position/rw->ruler.scale ;

	MwRulerSetMin(w, minValue) ;
}


double
MwRulerGetMin(w)
	Widget	w ;
{
	MwRulerWidget rw = (MwRulerWidget)w ;

	/* TODO: error message? */
	if( !XtIsSubclass(w, mwRulerWidgetClass) ) return 0. ;

	return rw->ruler.minValue ;
}


double
MwRulerGetScale(w)
	Widget	w ;
{
	MwRulerWidget rw = (MwRulerWidget)w ;

	/* TODO: error message? */
	if( !XtIsSubclass(w, mwRulerWidgetClass) ) return 0. ;

	return rw->ruler.scale ;
}


void
MwRulerSetValue(w, value)
	Widget	w ;
	double	value ;
{
	MwRulerWidget rw = (MwRulerWidget)w ;

	if( !XtIsSubclass(w, mwRulerWidgetClass) ) return ;

	undrawPointer(rw) ;
	rw->ruler.value = value ;
	rw->ruler.iValue = MwRulerValue2Position(w, value) ;
	drawPointer(rw) ;
}

void
MwRulerSetIValue(w, value)
	Widget	w ;
	int	value ;
{
	MwRulerWidget rw = (MwRulerWidget)w ;

	if( !XtIsSubclass(w, mwRulerWidgetClass) ) return ;

	undrawPointer(rw) ;
	rw->ruler.iValue = value ;
	if( rw->ruler.scale > 0. )
	  rw->ruler.value = MwRulerPosition2Value(w, value) ;
	drawPointer(rw) ;
}


void
MwRulerShowPointer(w, show)
	Widget	w ;
	Bool	show ;
{
	MwRulerWidget rw = (MwRulerWidget)w ;

	if( !XtIsSubclass(w, mwRulerWidgetClass) ) return ;

	if( rw->ruler.showPointer )
	  undrawPointer(rw) ;

	rw->ruler.showPointer = show ;

	if( show )
	  drawPointer(rw) ;
}


double
MwRulerGetValue(w)
	Widget	w ;
{
	MwRulerWidget rw = (MwRulerWidget)w ;

	if( !XtIsSubclass(w, mwRulerWidgetClass) ) return 0. ;

	return rw->ruler.value ;
}

int
MwRulerGetIValue(w)
	Widget	w ;
{
	MwRulerWidget rw = (MwRulerWidget)w ;

	if( !XtIsSubclass(w, mwRulerWidgetClass) ) return 0 ;

	return rw->ruler.iValue ;
}



double
MwRulerPosition2Value(w, position)
	Widget	w ;
	int	position ;
{
	MwRulerWidget rw = (MwRulerWidget)w ;

	if( !XtIsSubclass(w, mwRulerWidgetClass) || rw->ruler.scale ==0. )
	  return 0. ;

	return rw->ruler.minValue + position / rw->ruler.scale ;

}


int
MwRulerValue2Position(w, value)
	Widget	w ;
	double	value ;
{
	MwRulerWidget rw = (MwRulerWidget)w ;
	int	x ;

	if( !XtIsSubclass(w, mwRulerWidgetClass) ) return 0 ;

	x = (value - rw->ruler.v0) * rw->ruler.scale ;
	return x + rw->ruler.l0 ;
}





/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/


static	void
RulerAllocGCs(rw)
	MwRulerWidget rw ;
{
	RulerAllocFgGC(rw) ;
	RulerAllocPointerGC(rw) ;
}


static	void
RulerFreeGCs(rw)
	MwRulerWidget	rw ;
{
	Widget w = (Widget) rw;

	XtReleaseGC(w, rw->ruler.foregroundGC) ;
	XtReleaseGC(w, rw->ruler.fracGC) ;
	XtReleaseGC(w, rw->ruler.pointerGC) ;
}







	/* GEOMETRY UTILITIES */


	/* Utility: compute the stepsize & number of divisions based
	 * on the user's preferences and the available space.
	 */

static void
RulerStepDiv(rw)
	MwRulerWidget	rw;
{
	double		scale = rw->ruler.scale ;
	double		v0 = rw->ruler.minValue ;
	int		step ;		/* # of units to step, normally 1. */
	int		divisions ;
	char		line[128] ;
	int		i ;


	if( scale < 0. ) {
	  scale = -scale ;
	  v0 = -v0 ;
	}

	switch(rw->ruler.orientation) {
	  case NorthGravity:
	  case SouthGravity:
	    rw->ruler.length = rw->core.width ;
	    break ;
	  default:
	    rw->ruler.length = rw->core.height ;
	    break ;
	}
	/* if length <= 0, assume that we haven't been sized yet,
	 * so wing it.
	 */
	if( rw->ruler.length <= 0 )
	  rw->ruler.length = scale ;


	/* Find out how finely we can pack major tic marks.
	 * We want at least 3 clear pixels per tic mark.
	 */

	step = rw->ruler.markStep ;
	divisions = rw->ruler.markDiv ;
	if( step > 0 && step*scale < RULER_MIN ) {
	  divisions = 1 ;
	  step = minStep(step, RULER_MIN, scale) ;
	}
	else if( divisions > 1 && divisions * RULER_MIN > scale )
	  divisions = maxDiv(rw, divisions, (int)scale, rulerWid, RULER_MIN) ;

	rw->ruler.mStep = step ;
	rw->ruler.mDiv = divisions ;

	assert(rw->ruler.mDiv >= 1 ) ;

	/* Find out how closely we can pack labels.  Assume either
	 * first or last label is the biggest.  Fitting minor labels
	 * gets pretty tricky, especially if the ruler is horizontal;
	 * the number of subdivisions affects the size of the labels,
	 * which affects how many subdivisions we can have.
	 */

	if( rw->ruler.font != NULL && scale > 0. && rw->ruler.labelStep > 0 )
	{
	  double	v1 = 0.0;
	  int		wid ;

	  int iv0 = ifloor(v0) ;
	  int iv1 ;

	  switch( rw->ruler.orientation ) {
	    case NorthGravity:
	    case SouthGravity:
	      v1 = v0 + (double) rw->ruler.length / scale ;
	      iv1 = iceil(v1) ;
	      sprintf(line, "%d", iv0) ;
	      wid = XTextWidth(rw->ruler.font, line, strlen(line)) ;
	      sprintf(line, "%d", iv1) ;
	      i = XTextWidth(rw->ruler.font, line, strlen(line)) ;
	      if( i > wid ) { wid = i ; iv0 = iv1 ; }
	      wid *= 2 ;
	      break ;
	    default:
	      wid = fontAscent(rw->ruler.font) * 2 ;
	  }
	  /* wid is now the amount of space per label, plus an equal
	   * amount of blank space between.
	   */
	  rw->ruler.txtWid = wid/2 ;

	  step = rw->ruler.labelStep ;
	  divisions = rw->ruler.labelDiv ;
	  if( step > 0 && step*scale < wid ) {
	    divisions = 1 ;
	    step = minStep(step, wid, scale) ;
	  }

	  else if( divisions > 1 )
	  {
	    /* now compute the minor labels, for horizontal rulers,
	     * this loop mimics the maxDiv() function, except that
	     * the label width varies with the number of divisions.
	     */
	    switch( rw->ruler.orientation ) {
	      case NorthGravity:
	      case SouthGravity:
		iv1 = -max(abs(v0),abs(v1)) ;
		if( fracWid(rw, iv1, divisions) * divisions > scale )
		  divisions = maxDiv(rw, divisions, (int)scale, fracWid, iv1) ;
		wid = fracWid(rw, iv1, divisions) ;
		break ;
	      default:
		wid = fontAscent(rw->ruler.fracFont) * 2 ;
		divisions = maxDiv(rw, divisions, (int)scale, rulerWid, wid) ;
		break ;
	    }
	    if( (wid/=2) > rw->ruler.txtWid )
	      rw->ruler.txtWid = wid ;
	  }

	  rw->ruler.lStep = step ;
	  rw->ruler.lDiv = divisions ;
	  assert(rw->ruler.lDiv >= 1 ) ;
	}
}


static	void
PreferredSize(rw, reply_width, reply_height)
	MwRulerWidget	rw;
	Dimension	*reply_width, *reply_height;	/* total widget size */
{
	int	wid ;

	switch( rw->ruler.orientation ) {
	  case NorthGravity:
	  case SouthGravity:
	    *reply_height = TIC_LEN ;
	    if( rw->ruler.font != NULL )
	      *reply_height += fontAscent(rw->ruler.font) + IMARGIN ;
	    break ;
	  default:
	    /* vertical rulers are a bit tricky.  We need an estimate
	     * of what the label divisions are going to be before
	     * we will know how wide they will be.
	     */
	    RulerStepDiv(rw) ;
	    wid = fracWid(rw, (int)rw->ruler.minValue, rw->ruler.lDiv) / 2 ;
	    *reply_width = TIC_LEN + IMARGIN + wid ;
	}
}



static	int
ifloor(a)
  double a ;
{
  int rval = a ;

  if( a >= 0 || rval == a ) return rval ;
  else return rval-1 ;
}


static	int
iceil(a)
  double a ;
{
  int rval = a ;

  if( a <= 0 || rval == a ) return rval ;
  else return rval+1 ;
}





	/* Color & GC allocation.
	 *
	 * Ruler widgets use the following graphics contexts:
	 *
	 *  Foreground		ruler label text drawn this way
	 *  Insensitive Fg	foreground color greyed out.
	 *  Background		ruler background color
	 *
	 * Background GC is used for "undrawing" parts of the ruler.
	 * There is no "armed" color.
	 *
	 *
	 * GC's are defined as follows, depending on attributes and
	 * window depth:
	 *
	 * Monochrome:
	 *	Foreground = foreground color attribute or BlackPixel()
	 *	Pointer = Foreground color
	 *	Background = background color attribute or WhitePixel()
	 *
	 * Color, beNiceToColormap=true:
	 *	Foreground = foreground color attribute or BlackPixel()
	 *	Pointer = Foreground color
	 *	Background = background color attribute or WhitePixel()
	 *
	 * Color, beNiceToColormap=false:
	 *	Foreground = foreground color attribute or BlackPixel()
	 *	Pointer = foreground color
	 *	Background = background color attribute or WhitePixel()
	 *
	 * Special cases:
	 *	If background is white,   ??
	 *	if background is black,   ??
	 *
	 *
	 * If the widget's background is solid white or solid black,
	 * this code just picks some numbers.  (The choice is designed
	 * to be compatibile with ThreeD interface.)
	 */

static	void
RulerAllocFgGC(rw)
	MwRulerWidget	rw ;
{
	Widget		w = (Widget) rw;
	XGCValues	values ;
	u_long		mask = GCForeground|GCGraphicsExposures ;

	values.foreground = rw->ruler.foreground ;
	values.graphics_exposures = True ;
	if( rw->ruler.font != NULL ) {
	  values.font = rw->ruler.font->fid ;
	  mask = GCForeground|GCFont|GCGraphicsExposures ;
	}

	rw->ruler.foregroundGC =
	  XCreateGC( XtDisplay(w), XtWindow(w), mask, &values) ;

	values.font = rw->ruler.fracFont->fid ;
	rw->ruler.fracGC =
	  XCreateGC( XtDisplay(w), XtWindow(w), mask, &values) ;
}

static	void
RulerAllocPointerGC(rw)
	MwRulerWidget	rw;
{
	Widget		w = (Widget) rw;
	XGCValues	values ;

	values.foreground = rw->ruler.pointerColor ;
	values.graphics_exposures = False ;

	rw->ruler.pointerGC = XCreateGC( XtDisplay(w), XtWindow(w),
	    GCForeground|GCGraphicsExposures, &values) ;
}



static	Boolean
CvtStringToLabelStyle(dpy, args, nargs, from, to, data)
	Display		*dpy ;
	XrmValuePtr	args ;
	Cardinal	*nargs ;
	XrmValuePtr	from ;
	XrmValuePtr	to ;
	XtPointer	*data ;
{
	String	str = (String) from->addr ;

	to->size = sizeof(XtLabelStyle) ;

	if( XmuCompareISOLatin1(str, "fraction") == 0 )
	  *(XtLabelStyle *)to->addr = FRACTION ;
	else if( XmuCompareISOLatin1(str, "decimal") == 0 )
	  *(XtLabelStyle *)to->addr = DECIMAL ;
	else {
	  XtStringConversionWarning(from->addr, "labelStyle");
	  return False ;
	}
	return True ;
}
