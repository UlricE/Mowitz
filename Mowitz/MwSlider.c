/* MwSlider.c -- Slider widget
 *
 * Author: Edward A. Falk
 *	   falk@falconer.vip.best.com
 *
 * Date: 2 Nov 1999
 *
 * Copyright 1999 by Edward A. Falk.  This widget may be used freely in any
 * software.  Source code is freely distributable provided that my name
 * is not removed from it.
 *
 * The X Consortium, and any party obtaining a copy of these files
 * from the X Consortium, directly or indirectly, is granted, free of
 * charge, a full and unrestricted irrevocable, world-wide, paid up,
 * royalty-free, nonexclusive right and license to deal in this
 * software and documentation files (the "Software"), including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and sell copies of the Software, and to
 * permit persons who receive copies from any such party to do so.
 *
 *
 *
 * This widget presents the user with a graphical slider which the
 * user may manipulate to set a numeric value.
 *
 * $Log: MwSlider.c,v $
 * Revision 1.1.1.1  2003/01/16 20:31:17  ulric
 * Mowitz
 *
 * Revision 1.7  2000/01/19 01:46:09  falk
 * Fixed resources: input scale is short, not int.
 *
 * Revision 1.6  2000/01/18 20:39:24  falk
 * _SliderThumb() utility added.  Other code simplified accordingly.
 *
 * Revision 1.5  2000/01/17 20:54:21  falk
 * default now horizontal.  PreferredSize now a class method.  DrawThumb
 * now centers properly.
 *
 * Revision 1.4  2000/01/14 19:04:39  falk
 * fixed polarity of scrollwheel mouse interactions
 *
 * Revision 1.3  2000/01/11 19:39:36  falk
 * Added foreground resource.  Added keyboard focus.
 *
 * Revision 1.2  2000/01/11 18:26:28  falk
 * Recomputes slider position after resize.  Added "newvalue" argument
 * to XawSliderSetValue
 *
 * Revision 1.1  1999/11/04 17:22:10  falk
 * Initial revision
 *
 *
 */

#include	<stdio.h>
#include	<stdlib.h>
#include	<ctype.h>

#include	<X11/Xlib.h>
#include	<X11/IntrinsicP.h>
#include	<X11/StringDefs.h>
#include	<X11/Xmu/Drawing.h>
#include	<X11/Xmu/Misc.h>
#include	<X11/Xmu/CharSet.h>

#include	"MwSliderP.h"
#include	"MwGcs.h"

#define	DEF_WID		100
#define	DEF_HGT		10
#define	THUMB_WID	20	/* default thumb width */
#define	THUMB_HGT	10	/* default thumb height */


/****************************************************************
 *
 * MwSlider Resources
 *
 ****************************************************************/

#define	offset(field)	XtOffsetOf(MwSliderRec, slider.field)
static XtResource resources[] = {
  {XtNfont, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
	offset(font), XtRString, XtDefaultFont},
  {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
	offset(foreground), XtRString, XtDefaultForeground},
  {XtNminimum, XtCMinimum, XtRInt, sizeof(int),
	offset(minimum), XtRImmediate, (XtPointer)0},
  {XtNmaximum, XtCMaximum, XtRInt, sizeof(int),
	offset(maximum), XtRImmediate, (XtPointer)100},
  {XtNvalue, XtCValue, XtRInt, sizeof(int),
	offset(value), XtRImmediate, (XtPointer)0},
  {XtNstep, XtCStep, XtRInt, sizeof(int),
	offset(step), XtRImmediate, (XtPointer)-1},
  {XtNstep2, XtCStep2, XtRInt, sizeof(int),
	offset(step2), XtRImmediate, (XtPointer)1},
  {XtNorientation, XtCOrientation, XtROrientation, sizeof(XtOrientation),
	offset(orientation), XtRImmediate, (XtPointer) XtorientHorizontal},
  {XtNshowValue, XtCShowValue, XtRBoolean, sizeof(Boolean),
	offset(showValue), XtRImmediate, (XtPointer) False},
  {XtNdecimalPoints, XtCDecimalPoints, XtRShort, sizeof(short),
	offset(decimalPoints), XtRImmediate, (XtPointer)0},
  {XtNinputScale, XtCInputScale, XtRShort, sizeof(short),
	offset(inputScale), XtRImmediate, (XtPointer)100},
  {XtNautoScale, XtCAutoScale, XtRBoolean, sizeof(Boolean),
	offset(autoScale), XtRImmediate, (XtPointer) True},
  {XtNthumbLength, XtCThumbLength, XtRShort, sizeof(short),
	offset(thumbLength), XtRImmediate, (XtPointer) THUMB_WID},
  {XtNdragCallback, XtCDragCallback, XtRCallback, sizeof(XtPointer),
	offset(dragCallback), XtRCallback, NULL},
  {XtNvalueChangedCallback, XtCValueChangedCallback,
  	XtRCallback, sizeof(XtPointer),
	offset(valueChangedCallback), XtRCallback, NULL},

  {XtNbeNiceToColormap, XtCBeNiceToColormap, XtRBoolean, sizeof(Boolean),
	offset(be_nice_to_cmap), XtRImmediate, (XtPointer) False},
  {XtNshadowWidth, XtCShadowWidth, XtRShort, sizeof(short),
	offset(shadowWidth), XtRImmediate, (XtPointer) 1},
  {XtNtopShadowContrast, XtCTopShadowContrast, XtRShort, sizeof(short),
	offset(top_contrast), XtRImmediate, (XtPointer) 20},
  {XtNbottomShadowContrast, XtCBottomShadowContrast, XtRShort, sizeof(short),
	offset(bot_contrast), XtRImmediate, (XtPointer) 40},
  {XtNarmShadowContrast, XtCArmShadowContrast, XtRShort, sizeof(short),
	offset(arm_contrast), XtRImmediate, (XtPointer) 20},
  {XtNinsensitiveContrast, XtCInsensitiveContrast, XtRShort, sizeof(short),
	offset(insensitive_contrast), XtRImmediate, (XtPointer) 33},
};
#undef	offset






	/* FORWARD REFERENCES: */

#ifdef	NeedFunctionPrototypes

	/* member functions */

static	void	SliderClassInit() ;
static	void	SliderClassPartInit(WidgetClass wc) ;
static	void	SliderInit( Widget req, Widget new, ArgList, Cardinal *nargs) ;
static	void	SliderRealize(Widget, Mask *, XSetWindowAttributes *) ;
static	void	SliderDestroy( Widget w) ;
static	void	SliderResize( Widget w) ;
static	void	SliderExpose( Widget w, XEvent *event, Region region) ;
static	Boolean	SliderSetValues(Widget, Widget, Widget, ArgList, Cardinal *) ;
static	XtGeometryResult
	  SliderQueryGeometry( Widget, XtWidgetGeometry *, XtWidgetGeometry *) ;
static	Boolean	SliderAcceptFocus(Widget, Time *) ;
static	void	SliderDrawBorder(MwSliderWidget) ;
static	void	SliderDrawBackground(MwSliderWidget, int, int, int, int) ;
static	void	SliderDrawThumb(MwSliderWidget) ;
static	void	SliderMoveThumb(MwSliderWidget sw, int newpos) ;
static	void	SliderFocusProc(MwSliderWidget sw) ;

	/* action procs */

static	void	StartAdjust(Widget, XEvent *, String *, Cardinal *) ;
static	void	HandleThumb(Widget, XEvent *, String *, Cardinal *) ;
static	void	EndAdjust(Widget, XEvent *, String *, Cardinal *) ;
static	void	Adjust(Widget, XEvent *, String *, Cardinal *) ;
static	void	Notify(Widget, XEvent *, String *, Cardinal *) ;
static	void	Focus(Widget, XEvent *, String *, Cardinal *) ;


	/* internal privates */

static	void	ChangeSliderValue(MwSliderWidget sw, int value) ;

static	void	SliderPreferredSize( MwSliderWidget,
			Dimension *wid, Dimension *hgt,
			Dimension *min_wid, Dimension *min_hgt) ;

static	void	SliderAllocGCs(MwSliderWidget) ;
static	void	SliderFreeGCs(MwSliderWidget) ;

#else

static	void	SliderClassInit() ;
static	void	SliderClassPartInit() ;
static	void	SliderInit() ;
static	void	SliderRealize() ;
static	void	SliderDestroy() ;
static	void	SliderResize() ;
static	void	SliderExpose() ;
static	Boolean	SliderSetValues() ;
static	XtGeometryResult SliderQueryGeometry() ;
static	Boolean	SliderAcceptFocus() ;
static	void	SliderDrawBorder() ;
static	void	SliderDrawBackground() ;
static	void	SliderDrawThumb() ;
static	void	SliderMoveThumb() ;
static	void	SliderFocusProc() ;
static	void	StartAdjust() ;
static	void	HandleThumb() ;
static	void	EndAdjust() ;
static	void	Adjust() ;
static	void	Notify() ;
static	void	Focus() ;
static	void	ChangeSliderValue() ;
static	void	SliderPreferredSize() ;
static	void	SliderAllocGCs() ;
static	void	SliderFreeGCs() ;

#endif


static	char	defaultTranslations[] = "\
	<FocusIn>:		Focus(1)		\n\
	<FocusOut>:		Focus(0)		\n\
	<Btn1Down>:		StartAdjust()		\n\
	<Btn1Motion>:		HandleThumb()		\n\
	<Btn1Up>:		EndAdjust() Notify()	\n\
	<Btn4Down>:		Adjust(step2) Notify()	\n\
	<Btn5Down>:		Adjust(-step2) Notify()	\n\
	<Key>Left:		Adjust(-step2) Notify()	\n\
	<Key>KP_Left:		Adjust(-step2) Notify()	\n\
	<Key>Right:		Adjust(step2) Notify()	\n\
	<Key>KP_Right:		Adjust(step2) Notify()	\n\
	<Key>Down:		Adjust(-step2) Notify()	\n\
	<Key>KP_Down:		Adjust(-step2) Notify()	\n\
	<Key>Up:		Adjust(step2) Notify()	\n\
	<Key>KP_Up:		Adjust(step2) Notify()	\n\
	<Key>Page_Up:		Adjust(step) Notify()	\n\
	<Key>KP_Page_Up:	Adjust(step) Notify()	\n\
	<Key>Page_Down:		Adjust(-step) Notify()	\n\
	<Key>KP_Page_Down:	Adjust(-step) Notify()	\n\
	<Key>Home:		Adjust(home) Notify()	\n\
	<Key>KP_Home:		Adjust(home) Notify()	\n\
	<Key>End:		Adjust(end) Notify()	\n\
	<Key>KP_End:		Adjust(end) Notify()	\n\
	 " ;


static	XtActionsRec	actionsList[] =
  {
    {"StartAdjust",	StartAdjust},
    {"HandleThumb",	HandleThumb},
    {"EndAdjust",	EndAdjust},
    {"Adjust",		Adjust},
    {"Notify",		Notify},
    {"Focus",		Focus},
  } ;



/****************************************************************
*
* Full class record constant
*
****************************************************************/

#define	SuperClass	(&coreClassRec)

MwSliderClassRec mwSliderClassRec = {
  {
/* core_class fields      */
    /* superclass         */    (WidgetClass) SuperClass,
    /* class_name         */    "MwSlider",
    /* widget_size        */    sizeof(MwSliderRec),
    /* class_initialize   */    SliderClassInit,
    /* class_part_init    */	SliderClassPartInit,
    /* class_inited       */	FALSE,
    /* initialize         */    SliderInit,
    /* initialize_hook    */	NULL,
    /* realize            */    SliderRealize,
    /* actions            */    actionsList,
    /* num_actions	  */	XtNumber(actionsList),
    /* resources          */    resources,
    /* num_resources      */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion	  */	TRUE,
    /* compress_exposure  */	XtExposeCompressMaximal|
				XtExposeGraphicsExpose|
    				XtExposeGraphicsExposeMerged|
				XtExposeNoRegion,
    /* compress_enterleave*/	TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    SliderDestroy,
    /* resize             */    SliderResize,
    /* expose             */    SliderExpose,
    /* set_values         */    SliderSetValues,
    /* set_values_hook    */	NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */	NULL,
    /* accept_focus       */    SliderAcceptFocus,
    /* version            */	XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    defaultTranslations,
    /* query_geometry     */	SliderQueryGeometry,
    /* display_accelerator*/	XtInheritDisplayAccelerator,
    /* extension          */	NULL
  },
  {
/* Slider class fields */
    /* draw_border	*/	SliderDrawBorder,
    /* draw_background	*/	SliderDrawBackground,
    /* draw_thumb	*/	SliderDrawThumb,
    /* move_thumb	*/	SliderMoveThumb,
    /* focus_proc	*/	SliderFocusProc,
    /* preferred_size	*/	SliderPreferredSize,
    /* extension	*/	NULL,
  }
};

WidgetClass mwSliderWidgetClass = (WidgetClass)&mwSliderClassRec;



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
SliderClassInit(void)
{
    /* register converters */

    XtAddConverter( XtRString, XtROrientation, XmuCvtStringToOrientation,
    	NULL, 0) ;
}


static	void
SliderClassPartInit(WidgetClass wc)
{
  MwSliderWidgetClass sc = (MwSliderWidgetClass)wc ;
  MwSliderWidgetClass super = (MwSliderWidgetClass)wc->core_class.superclass;

  if( sc->slider_class.draw_border == XtInheritDrawBorder )
    sc->slider_class.draw_border = super->slider_class.draw_border ;

  if( sc->slider_class.draw_background == XtInheritDrawBackground )
    sc->slider_class.draw_background = super->slider_class.draw_background ;

  if( sc->slider_class.draw_thumb == XtInheritDrawThumb )
    sc->slider_class.draw_thumb = super->slider_class.draw_thumb ;

  if( sc->slider_class.move_thumb == XtInheritMoveThumb )
    sc->slider_class.move_thumb = super->slider_class.move_thumb ;

  if( sc->slider_class.focus_proc == XtInheritFocusProc )
    sc->slider_class.focus_proc = super->slider_class.focus_proc ;

  if( sc->slider_class.preferred_size == XtInheritPreferredSize )
    sc->slider_class.preferred_size = super->slider_class.preferred_size ;
}



	/* Init a newly created slider widget.  */


static void
SliderInit(Widget request, Widget new, ArgList args, Cardinal *num_args)
{
    MwSliderWidget sw = (MwSliderWidget)new;
    int	minimum = sw->slider.minimum ;
    int	maximum = sw->slider.maximum ;

    /* defer GC allocation, etc., until Realize() time. */
    sw->slider.foregroundGC =
    sw->slider.backgroundGC =
    sw->slider.greyGC =
    sw->slider.topGC =
    sw->slider.botGC = None ;
    sw->slider.grey50 = None ;
    if( maximum != minimum )
      sw->slider.thumbpos =
      	sw->slider.length*(sw->slider.value-minimum)/(maximum-minimum) ;
    else
      sw->slider.thumbpos = 0 ;

    if( sw->slider.step < 0 )
      sw->slider.step = (sw->slider.maximum - sw->slider.minimum)/10 ;

    sw->slider.adjusting = False ;


    /* if size not explicitly set, set it to our preferred size now. */

    if( request->core.width == 0 || request->core.height == 0 )
    {
      Dimension	w,h, dummy ;
      SliderClass(sw)->slider_class.preferred_size(sw, &w,&h, &dummy,&dummy);
      if( request->core.width == 0 ) new->core.width = w ;
      if( request->core.height == 0 ) new->core.height = h ;
      XtClass(new)->core_class.resize(new) ;
    }

    sw->slider.needs_layout = True ;
}





	/* Called when slider widget first realized.  Create the window
	 * and allocate the GCs
	 */

static	void
SliderRealize(Widget w, Mask *valueMask, XSetWindowAttributes *attributes)
{
	MwSliderWidget sw = (MwSliderWidget) w;

	/* chain upwards */
	SuperClass->core_class.realize(w, valueMask, attributes);

	SliderAllocGCs(sw) ;
}



static	void
SliderDestroy(Widget w)
{
	SliderFreeGCs((MwSliderWidget)w) ;
}


	/* Parent has resized us.  This will require that the slider be
	 * laid out again.
	 */

static void
SliderResize(Widget w)
{
	MwSliderWidget	sw = (MwSliderWidget) w;
	int		length ;
	int		s = sw->slider.shadowWidth ;
	int		minimum, maximum ;

	/* compute working length of slider */

	sw->slider.start = sw->slider.shadowWidth ;

	if( sw->slider.orientation == XtorientHorizontal ) {
	  length = sw->core.width ;
	  sw->slider.thumbWidth = sw->core.height - s*2 ;
	}
	else {
	  length = sw->core.height ;
	  sw->slider.thumbWidth = sw->core.width - s*2 ;
	}

	length -= + sw->slider.thumbLength + s*2 ;
	if( length < 0 ) length = 0 ;
	sw->slider.length = length ;

	/* TODO: shrink thumb if it will make things fit? */

	/* recompute thumb position */
	minimum = sw->slider.minimum ;
	maximum = sw->slider.maximum ;
	if( maximum != minimum )
	  sw->slider.thumbpos =
	    length*(sw->slider.value-minimum)/(maximum-minimum) ;
	else
	  sw->slider.thumbpos = 0 ;

	if( sw->slider.autoScale )
	  sw->slider.inputScale = length < 100 ? length : 100 ;

	sw->slider.needs_layout = False ;
}



	/* Redraw entire Slider widget */

static	void
SliderExpose(Widget w, XEvent *event, Region region)
{
	MwSliderWidget	sw = (MwSliderWidget) w;

	if( sw->slider.needs_layout )
	  XtClass(w)->core_class.resize(w) ;

	SliderClass(sw)->slider_class.draw_border(sw) ;
	SliderClass(sw)->slider_class.draw_background(sw,
		0,0, w->core.width, w->core.height) ;
	SliderClass(sw)->slider_class.draw_thumb(sw) ;
}


	/* Called when any Slider widget resources are changed. */

static	Boolean
SliderSetValues(Widget old, Widget request, Widget new,
	ArgList args, Cardinal *num_args)
{
	MwSliderWidget oldsw = (MwSliderWidget) old ;
	MwSliderWidget sw = (MwSliderWidget) new ;
	Boolean	needRedraw = False ;

	if( sw->slider.value != oldsw->slider.value  ||
	    sw->slider.minimum != oldsw->slider.minimum  ||
	    sw->slider.maximum != oldsw->slider.maximum )
	{
	  int	minimum = sw->slider.minimum ;
	  int	maximum = sw->slider.maximum ;

	  if( maximum != minimum )
	    sw->slider.thumbpos =
	    	sw->slider.length*(sw->slider.value-minimum)/(maximum-minimum) ;
	  else
	    sw->slider.thumbpos = 0 ;

	  needRedraw = True ;
	}

	if( sw->slider.orientation != oldsw->slider.orientation  ||
	    sw->slider.thumbLength != oldsw->slider.thumbLength  ||
	    sw->slider.shadowWidth != oldsw->slider.shadowWidth ||
	    sw->slider.font != oldsw->slider.font )
	{
	  /* do some resizing stuff? */
	  needRedraw = True ;
	  sw->slider.needs_layout = True ;
	}

	/* if any color changes, need to recompute GCs and redraw */

	if( sw->core.background_pixel != oldsw->core.background_pixel ||
	    sw->core.background_pixmap != oldsw->core.background_pixmap ||
	    sw->slider.top_contrast != oldsw->slider.top_contrast  ||
	    sw->slider.bot_contrast != oldsw->slider.bot_contrast  ||
	    sw->slider.arm_contrast != oldsw->slider.arm_contrast  ||
	    sw->slider.insensitive_contrast !=
	    			oldsw->slider.insensitive_contrast  ||
	    sw->slider.be_nice_to_cmap != oldsw->slider.be_nice_to_cmap )
	{
	  /* brute-force method, recompute everything.  It would be
	   * more efficient to only recompute the GC's that are
	   * actually affected.
	   */
	  SliderFreeGCs(sw) ;
	  SliderAllocGCs(sw) ;
	  needRedraw = True ;
	}

	if( sw->core.sensitive != oldsw->core.sensitive )
	  needRedraw = True ;

	return needRedraw ;
}



static	Boolean
SliderAcceptFocus( Widget w, Time *tm )
{
    if( !w->core.being_destroyed  && XtIsRealized(w) &&
	XtIsSensitive(w) && w->core.visible && XtIsManaged(w) )
    {
      Widget p ;
      for(p = XtParent(w); !XtIsShell(p); p = XtParent(p)) ;
      XtSetKeyboardFocus(p, w) ;
      return True ;
    }
    else
      return False ;
}





/*
 * Return preferred size.  Accept changes to our preferred size,
 * reject changes that are too small, return "almost" for sizes that
 * are too big.
 *
 * These return values are highly subjective.  For example, many
 * widgets (e.g. scrolled windows or text windows) would be perfectly
 * happy with any size greater than the minimum.
 */

static XtGeometryResult
SliderQueryGeometry(Widget w,
	XtWidgetGeometry *intended, XtWidgetGeometry *preferred)
{
    register MwSliderWidget sw = (MwSliderWidget)w ;
    XtGeometryMask mode = intended->request_mode ;
    int	iw = intended->width ;
    int	ih = intended->height ;
    Dimension min_wid, min_hgt ;

    /* provide preferred size to caller, don't care about position,
     * border_width, etc.
     */
    preferred->request_mode = CWWidth | CWHeight ;
    SliderClass(sw)->slider_class.preferred_size (sw,
    	&preferred->width, &preferred->height, &min_wid, &min_hgt) ;

    /* reject if caller proposing no change */
    /* TODO: is this really worth it? */
    if( (!(mode & CWX) || intended->x == w->core.x)  &&
	(!(mode & CWY) || intended->y == w->core.y)  &&
	(!(mode & CWBorderWidth) ||
		intended->border_width == w->core.border_width)  &&
	(!(mode & CWWidth) || iw == w->core.width)  &&
	(!(mode & CWHeight) || ih == w->core.height) )
      return XtGeometryNo ;

    /* reject if too small */
    if( ((mode & CWWidth) && iw < min_wid )  ||
	((mode & CWHeight) && ih < min_hgt ) )
      return XtGeometryNo ;

    /* Since we don't mind being too big, accept anything else. */
    return XtGeometryYes ;
}





/****************************************************************
 *
 * Action Procedures
 *
 ****************************************************************/

/*
 * Rules for converting between pointer motion and slider motion:
 *
 *   Definitions:
 *         spos = slider position
 *         ds = change in slider position
 *         length = domain of slider motion
 *         range = maximum-minimum
 *         dx = pointer motion
 *         dv = change in value
 *
 *
 *   Relationship between slider position and slider value are described
 *   by this equation:
 *
 *         spos/length = (value-minimum)/(maximum-minimum)
 *
 *     convert value => slider position:
 *
 *         spos = length*(value-minimum)/(maximum-minimum)
 *
 *     convert slider position => value:
 *
 *         value = (maximum-minimum)*spos/length + minimum
 *
 *
 *
 *   Changes in pointer position are related to changes in slider
 *   position:
 *
 *         ds = dx*inputScale/100
 *
 *
 *   It's better to take pointer motion and map it directly to value
 *   changes, especially with small sliders.
 *
 *         dv = ds*range/length
 *         dv = dx*inputScale/100*range/length
 *
 *   There are three special cases that are worth optimizing:
 *
 *	inputScale = 100:
 *
 *         dv = dx*range/length
 *
 *	inputScale = length:
 *	length = 0:
 *
 *         dv = dx*range/100
 *
 */





static	void
StartAdjust(Widget w, XEvent *ev, String *args, Cardinal *num_args)
{
	MwSliderWidget sw = (MwSliderWidget)w ;
	int	s = sw->slider.shadowWidth ;
	int	pt ;

	/* find out if this was a click on the thumb, or the
	 * background.
	 */
	if( sw->slider.orientation == XtorientHorizontal )
	  pt = s + ev->xbutton.x ;
	else
	  pt = sw->core.height - s - ev->xbutton.y ;

	if( pt < sw->slider.thumbpos + 2*s )
	  ChangeSliderValue(sw, sw->slider.value - sw->slider.step) ;
	else if( pt > sw->slider.thumbpos + 2*s + sw->slider.thumbLength )
	  ChangeSliderValue(sw, sw->slider.value + sw->slider.step) ;
	else {
	  sw->slider.oldvalue = sw->slider.value ;
	  sw->slider.adjustPt = pt ;
	  sw->slider.adjusting = True ;
	}
}


static	void
HandleThumb(Widget w, XEvent *ev, String *args, Cardinal *num_args)
{
	MwSliderWidget sw = (MwSliderWidget)w ;
	int	s = sw->slider.shadowWidth ;
	int	pt, dp, dv ;
	int	length = sw->slider.length ;
	int	range = sw->slider.maximum - sw->slider.minimum ;

	if( !sw->slider.adjusting ) return ;

	if( length <= 0 ) length = 1 ;

	if( sw->slider.orientation == XtorientHorizontal )
	  pt = s + ev->xbutton.x ;
	else
	  pt = sw->core.height - s - ev->xbutton.y ;

	dp = pt - sw->slider.adjustPt ;

	/* special cases */
	if( sw->slider.inputScale == 100 )
	  dv = dp*range/length ;
	else if( sw->slider.inputScale == length )
	  dv = dp*range/100 ;
	else
	  dv = dp*sw->slider.inputScale*range/length/100 ;

	ChangeSliderValue(sw, sw->slider.oldvalue + dv) ;
	XtCallCallbackList(w,
		sw->slider.dragCallback, (XtPointer)sw->slider.value) ;
}


static	void
EndAdjust(Widget w, XEvent *ev, String *args, Cardinal *num_args)
{
	MwSliderWidget sw = (MwSliderWidget)w ;

	if( !sw->slider.adjusting ) return ;

	sw->slider.adjusting = False ;
}


static	void
Adjust(Widget w, XEvent *ev, String *args, Cardinal *num_args)
{
	MwSliderWidget sw = (MwSliderWidget)w ;
	int newvalue = sw->slider.value ;

	if( *num_args == 0 ) return ;	/* TODO: warning? */

	if( *num_args >= 1 )
	{
	  Bool neg = False ;
	  int n = 0 ;
	  String str = *args ;
	  if( *str == '-' ) { neg = True ; ++str ; }
	  if( isdigit(*str) )
	    n = atoi(str) ;
	  else if( XmuCompareISOLatin1(str, "step2") == 0 )
	    n = sw->slider.step2 ;
	  else if( XmuCompareISOLatin1(str, "step") == 0 )
	    n = sw->slider.step ;
	  else if( XmuCompareISOLatin1(str, "home") == 0 )
	    newvalue = sw->slider.minimum ;
	  else if( XmuCompareISOLatin1(str, "end") == 0 )
	    newvalue = sw->slider.maximum ;

	  if( neg ) n = -n ;
	  newvalue += n ;

	  ChangeSliderValue(sw, newvalue) ;
	}
}

static	void
Notify(Widget w, XEvent *ev, String *args, Cardinal *num_args)
{
	MwSliderWidget sw = (MwSliderWidget)w ;
	XtCallCallbackList(w,
		sw->slider.valueChangedCallback, (XtPointer)sw->slider.value) ;
}


static	void
Focus(Widget w, XEvent *ev, String *args, Cardinal *num_args)
{
	MwSliderWidget sw = (MwSliderWidget)w ;

	if( *num_args == 0 ) return ;	/* TODO: warning? */

	sw->slider.has_focus = atoi(*args) ;

	SliderClass(sw)->slider_class.focus_proc(sw) ;
}






/****************************************************************
 *
 * Public Procedures
 *
 ****************************************************************/


	/* Set the top fc, optionally call all callbacks. */
void
XawSliderSetValue(Widget w, int newvalue, Bool callCallbacks)
{
	MwSliderWidget	sw = (MwSliderWidget) w;
	if( !XtIsSubclass(w, mwSliderWidgetClass) ) return ;
	ChangeSliderValue(sw, newvalue) ;
	if( callCallbacks)
	  XtCallCallbackList(w,
		sw->slider.valueChangedCallback, (XtPointer)sw->slider.value) ;
}




/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/


/*
 * Overview:
 *
 *   draw_border() draws the "static" part of the widget, that is, any
 *   part that will never be obscured by the thumb.  In the base class,
 *   this means the shadow border.  This function should also draw the
 *   keyboard focus feedback, possibly by calling focus_proc().
 *
 *   draw_background() draws whatever part of the slider is under the
 *   thumb.  It is used during expose() and to repair damage made by
 *   moving the thumb.  It accepts as arguments a region to be
 *   redrawn.  This may describe a region just vacated by the thumb,
 *   or it may describe the entire widget.  This function may also need
 *   to draw the keyboard focus feedback, if such feedback can be damaged
 *   by thumb motion.  This function will not be called with width or
 *   height equal to zero.
 *
 *   draw_thumb() draws the thumb at the current location.
 *
 *   move_thumb() moves the thumb to a new location.  In the base class,
 *   this function uses XCopyArea() to move the thumb, and draw_background()
 *   to repair the damage.  XCopyArea() may not be suitable if the thumb
 *   is non-rectangular.
 *
 *   focus_proc() is called in response to changes in keyboard focus;
 *   it draws the visual indication that the slider has gained
 *   or has lost keyboard focus.  This proc examines the has_focus
 *   variable.
 *
 *   preferred_size() returns the slider's preferred size, and the
 *   minimum acceptable size.
 *
 *   Variables:
 *
 *     start       the first pixel the thumb may ever occupy
 *
 *     length      the working length of the slider -- i.e. the total
 *                 adjusting range of the thumb.  The first pixel
 *                 occupied by the thumb may range from start to
 *                 start+length.
 *
 *     thumbpos    current location of the thumb; ranges from 0 to length
 *
 *     thumbLength length of the thumb.  The last pixel occupied by the
 *                 thumb may range from (start + thumbLength - 1) to
 *                 (start + length + thumbLength -1)
 *
 *     thumbWidth  width of the thumb; used by move_thumb() to determine
 *		   the size of the region to be copied.  Note that for
 *		   horizontal sliders, this is the vertical dimension.
 *
 */



static	void
SliderDrawBorder(MwSliderWidget sw)
{
	Draw3dBox((Widget)sw, 0,0, sw->core.width, sw->core.height,
		sw->slider.shadowWidth, sw->slider.botGC, sw->slider.topGC,
		sw->slider.botGC, sw->slider.topGC) ;
}


static	void
SliderDrawBackground(MwSliderWidget sw, int x, int y, int wid, int hgt)
{
	Widget	w = (Widget)sw ;
	int	s = sw->slider.shadowWidth ;
	int	x2 = x+wid ;
	int	y2 = y+hgt ;
	int	x3 = s ;
	int	y3 = s ;
	int	x4 = sw->core.width - s ;
	int	y4 = sw->core.height - s ;

	/* easy; just clip the requested region to within the shadow
	 * border and fill.
	 */

	if( x < x3 ) x = x3 ;
	if( y < y3 ) y = y3 ;
	if( x2 > x4 ) x2 = x4 ;
	if( y2 > y4 ) y2 = y4 ;

	XFillRectangle(XtDisplay(w), XtWindow(w), sw->slider.greyGC,
	  x,y, x2-x, y2-y) ;
}


	/* Utility: return the bounds of the thumb */

void
_SliderThumb(MwSliderWidget sw, int *x, int *y, int *wid, int *hgt)
{
	int	st = sw->slider.start + sw->slider.thumbpos ;
	int	tl = sw->slider.thumbLength ;
	int	tw = sw->slider.thumbWidth ;

	if( sw->slider.thumbpos < 0 )
	  sw->slider.thumbpos = 0 ;
	else if( sw->slider.thumbpos > sw->slider.length )
	  sw->slider.thumbpos = sw->slider.length ;

	/* assume thumb centered in slider */

	if( sw->slider.orientation == XtorientHorizontal ) {
	  *wid = tl ;
	  *hgt = tw ;
	  *x = st ;
	  *y = (sw->core.height - tw)/2 ;
	}
	else {
	  *wid = tw ;
	  *hgt = tl ;
	  *x = (sw->core.width - tw)/2 ;
	  *y = sw->core.height - st - tl ;
	}
}


static	void
SliderDrawThumb(MwSliderWidget sw)
{
	Display	*dpy = XtDisplay((Widget)sw) ;
	Window	win = XtWindow((Widget)sw) ;
	int	s = sw->slider.shadowWidth ;
	int	x,y, wid,hgt ;
	GC	botgc = sw->slider.botGC ;
	GC	topgc = sw->slider.topGC ;

	_SliderThumb(sw, &x, &y, &wid, &hgt) ;

	XClearArea(dpy,win, x,y, wid,hgt, False) ;

	if( sw->slider.orientation == XtorientHorizontal ) {
	  XDrawLine(dpy,win, botgc, x+wid/2-1, y, x+wid/2-1, y+hgt-1) ;
	  XDrawLine(dpy,win, topgc, x+wid/2, y, x+wid/2, y+hgt-1) ;
	}
	else {
	  XDrawLine(dpy,win, botgc, x, y+hgt/2-1, x+wid-1, y+hgt/2-1) ;
	  XDrawLine(dpy,win, topgc, x, y+hgt/2, x+wid-1, y+hgt/2) ;
	}
	Draw3dBox((Widget)sw, x,y, wid, hgt, s, topgc, botgc, topgc, botgc) ;
}


static	void
SliderMoveThumb(MwSliderWidget sw, int newpos)
{
	Display	*dpy = XtDisplay((Widget)sw) ;
	Window	win = XtWindow((Widget)sw) ;
	int	sx,sy, dx,dy, wid,hgt ;
	int	oldpos = sw->slider.thumbpos ;
	GC	gc = sw->slider.backgroundGC ;

	/* assume the thumb is centered in the slider */

	if( newpos < 0 )
	  newpos = 0 ;
	else if( newpos > sw->slider.length )
	  newpos = sw->slider.length ;

	if( newpos == oldpos )
	  return ;

	_SliderThumb(sw, &sx, &sy, &wid, &hgt) ;

	dx = sx ;
	dy = sy ;
	if( sw->slider.orientation == XtorientHorizontal )
	  dx += newpos - oldpos ;
	else
	  dy -= newpos - oldpos ;
	XCopyArea(dpy,win,win, gc, sx,sy, wid,hgt, dx,dy) ;

	if( sw->slider.orientation == XtorientHorizontal ) {
	  /* fill in left or right? */
	  if( newpos > oldpos ) {	/* right */
	    dx = sx ;
	    if( newpos - oldpos < wid ) wid = newpos - oldpos ;
	  }
	  else {			/* left */
	    dx = sx + wid ;
	    if( oldpos - newpos < wid ) wid = oldpos - newpos ;
	    dx -= wid ;
	  }
	}
	else {
	  /* fill in top or bottom? */
	  if( newpos > oldpos ) {	/* bottom */
	    dy = sy + hgt ;
	    if( newpos - oldpos < hgt ) hgt = newpos - oldpos ;
	    dy -= hgt ;
	  }
	  else {			/* top */
	    dy = sy ;
	    if( oldpos - newpos < hgt ) hgt = oldpos - newpos ;
	  }
	}
	SliderClass(sw)->slider_class.draw_background(sw, dx,dy, wid, hgt) ;

	sw->slider.thumbpos = newpos ;
}


	/* In the base class, we indicate focus by a solid border that
	 * will at least partially overlay the 3-d border.  Undraw the
	 * solid border by simply redrawing the 3-d border.
	 */

static	void
SliderFocusProc(MwSliderWidget sw)
{
	if( sw->slider.has_focus )
	  XDrawRectangle(XtDisplay((Widget)sw), XtWindow((Widget)sw),
	    sw->slider.foregroundGC,
	    0,0, sw->core.width-1, sw->core.height-1) ;
	else
	  SliderDrawBorder(sw) ;
}


static	void
ChangeSliderValue(MwSliderWidget sw, int newvalue)
{
	int	newpos ;
	int	minimum = sw->slider.minimum ;
	int	maximum = sw->slider.maximum ;

	if( newvalue < minimum )
	  newvalue = minimum ;
	else if( newvalue > maximum )
	  newvalue = maximum ;

	if( maximum != minimum )
	  newpos = sw->slider.length*(newvalue-minimum)/(maximum-minimum) ;
	else
	  newpos = 0 ;
	SliderClass(sw)->slider_class.move_thumb(sw, newpos) ;
	sw->slider.value = newvalue ;
}



static	void
SliderAllocGCs(MwSliderWidget sw)
{
	Widget w = (Widget) sw;

	if( !XtIsRealized(w) )
	  return ;

	sw->slider.backgroundGC = AllocBackgroundGC((Widget)sw, None) ;
	sw->slider.foregroundGC = AllocFgGC(w,
		sw->slider.foreground, sw->slider.font->fid) ;
	sw->slider.topGC = AllocTopShadowGC(w,
		sw->slider.top_contrast, sw->slider.be_nice_to_cmap) ;
	sw->slider.botGC = AllocBotShadowGC(w,
		sw->slider.bot_contrast, sw->slider.be_nice_to_cmap) ;
	sw->slider.greyGC = AllocArmGC(w,
		sw->slider.arm_contrast, sw->slider.be_nice_to_cmap) ;
}


static	void
SliderFreeGCs(MwSliderWidget sw)
{
	Widget w = (Widget) sw;

	if( !XtIsRealized(w) )
	  return ;

	XtReleaseGC(w, sw->slider.backgroundGC) ;
	XtReleaseGC(w, sw->slider.foregroundGC) ;
	XtReleaseGC(w, sw->slider.topGC) ;
	XtReleaseGC(w, sw->slider.botGC) ;
	XtReleaseGC(w, sw->slider.greyGC) ;
	XmuReleaseStippledPixmap(XtScreen(w), sw->slider.grey50) ;
}










/****************************************************************
 *
 * Geometry Utilities
 *
 ****************************************************************/




	/*
	 * Find preferred size.  Depends on widget contents.
	 */


static	void
SliderPreferredSize(MwSliderWidget sw,
	Dimension *reply_width, Dimension *reply_height,
	Dimension *min_wid, Dimension *min_hgt)
{
	int			s = sw->slider.shadowWidth ;

	if( sw->slider.orientation == XtorientHorizontal ) {
	  *reply_width = DEF_WID ;
	  *reply_height = THUMB_HGT + s*4 ;
	  *min_wid = sw->slider.thumbLength + s*2 + 1 ;
	  *min_hgt = s*2 + 3 ;
	}
	else {
	  *reply_width = THUMB_HGT + s*4 ;
	  *reply_height = DEF_WID ;
	  *min_wid = s*2 + 3 ;
	  *min_hgt = sw->slider.thumbLength + s*2 + 1 ;
	}
}
