
#include <stdio.h>

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Core.h>
#include <X11/Object.h>
#include <X11/Shell.h>

#include <X11/Xaw/Form.h>

#include <Mowitz/Mowitz.h>

	/* X11 stuff */

static	Display	*dpy ;
static	Atom	wm_protocols ;
static	Atom	wm_delete_window ;

static	GC	canvasGc ;
static	Bool	canvas_init = False ;
static	Pixel	black, yellow ;

	/* Xt stuff */

static	XtAppContext	app_ctx ;

static	Widget	topLevel ;
static	Widget	mainPanel ;
static	Widget	leftRuler, topRuler ;
static	Widget	leftFrame, topFrame ;
static	Widget	canvas ;

static	void	CanvasCB(Widget, XtPointer, XEvent *, Boolean *) ;

static String fallback[] = {
	"*background:		gray",
	"*borderWidth:		0",
	"*Frame.shadowWidth:	1",
	"*Frame.shadowType:	raised",
	"*MwRuler.font:	-*-lucida-medium-r-normal-sans-12-*-*-*-*-*-iso8859-*",
	"*MwRuler.fracFont: -*-lucida-medium-r-normal-sans-8-*-*-*-*-*-iso8859-*",
	"*mainPanel.defaultDistance:	0",
	"*canvas.width:		300",
	"*canvas.height:	300",
	"*topRuler.width:	300",
	"*leftRuler.height:	300",
	"*topRuler.scale:	100",
	"*leftRuler.scale:	-100",
	"Rulerdemo.baseTranslations: #augment <Message>WM_PROTOCOLS: WMProtocols()",
	NULL
};

static	void	RulerCB(Widget, XtPointer, XtPointer) ;

static	void	WMProtocols(Widget, XEvent *, String *, Cardinal *) ;


	/* define global actions table for the application.  This is done
	 * to catch messages.  This could also be done through the main
	 * event loop if we wrote one.  It would be nice if Xt had a way
	 * of regestering a callback proc for specific kinds of events.
	 */

XtActionsRec	rulerdemo_actions[] = {
    {"WMProtocols", WMProtocols},
} ;


int
main(int argc, char **argv)
{
	Dimension len, bwid ;

	topLevel = XtAppInitialize(&app_ctx, "Rulerdemo", NULL,0,
	  &argc, argv, fallback, NULL,0) ;
	dpy = XtDisplay(topLevel) ;

	XtAppAddActions(app_ctx,
		rulerdemo_actions, XtNumber(rulerdemo_actions));

	mainPanel = XtVaCreateManagedWidget("mainPanel",
		formWidgetClass, topLevel,
		0) ;

	topFrame = XtVaCreateManagedWidget("topFrame",
		mwFrameWidgetClass, mainPanel,
		XtNleft, XtChainLeft,
		XtNright, XtChainRight,
		XtNtop, XtChainTop,
		XtNbottom, XtChainTop,
		0) ;

	topRuler = XtVaCreateManagedWidget("topRuler",
		mwRulerWidgetClass, topFrame,
		XtNorientation, NorthGravity,
		0) ;
	XtAddCallback(topRuler, XtNcallback, RulerCB, NULL) ;

	leftFrame = XtVaCreateManagedWidget("leftFrame",
		mwFrameWidgetClass, mainPanel,
		XtNleft, XtChainLeft,
		XtNright, XtChainLeft,
		XtNtop, XtChainTop,
		XtNbottom, XtChainBottom,
		XtNfromVert, topFrame,
		0) ;

	leftRuler = XtVaCreateManagedWidget("leftRuler",
		mwRulerWidgetClass, leftFrame,
		XtNorientation, WestGravity,
		0) ;
	XtAddCallback(leftRuler, XtNcallback, RulerCB, NULL) ;


	/* yuk.  Athena Form widget really sucks; I shouldn't have
	 * to go through these gyrations.
	 */

	XtVaGetValues(leftRuler, XtNwidth, &len, 0) ;
	XtVaGetValues(leftFrame, XtNshadowWidth, &bwid, 0) ;
	XtVaSetValues(topFrame, XtNhorizDistance, len+bwid*2, 0) ;

	canvas = XtVaCreateManagedWidget("canvas",
		coreWidgetClass, mainPanel,
		XtNleft, XtChainLeft,
		XtNright, XtChainRight,
		XtNtop, XtChainTop,
		XtNbottom, XtChainBottom,
		XtNfromHoriz, leftFrame,
		XtNfromVert, topFrame,
		0) ;
	XtAddEventHandler(canvas,
	  ExposureMask | ButtonPressMask | PointerMotionMask |
	  EnterWindowMask | LeaveWindowMask,
	  True, CanvasCB, NULL) ;

	XtRealizeWidget(topLevel) ;

	/* tell window system we're willing to handle window-delete messages */
	wm_protocols = XInternAtom(dpy, "WM_PROTOCOLS", False);
	wm_delete_window = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
	(void) XSetWMProtocols(dpy, XtWindow(topLevel), &wm_delete_window,1);

	MwRulerSetMinPosition(topRuler, 0., 0) ;
	XtVaGetValues(leftRuler, XtNheight, &len, 0) ;
	MwRulerSetMinPosition(leftRuler, 0., len-1) ;

	XtAppMainLoop(app_ctx) ;

	exit(0) ;
}









static	void
RulerCB(Widget r, XtPointer client, XtPointer call_data)
{
	MwRulerReport	*rep = (MwRulerReport *)call_data ;
	Display		*dpy = XtDisplay(canvas) ;
	Window		win = XtWindow(canvas) ;
	Position	sx,sy ;
	Dimension	wid,hgt ;

	switch( rep->what ) {
	  case SCROLL:
	  case STOP:
	    XtVaGetValues(canvas, XtNwidth, &wid, XtNheight, &hgt, 0) ;
	    sx = -rep->dx ;
	    sy = -rep->dy ;
	    XCopyArea(dpy, win,win, canvasGc,
	      (int)sx, (int)sy, (u_int)wid, (u_int)hgt, 0, 0 ) ;
	    break ;
	  default:
	    break ;
	}
}


static	void
CanvasCB(Widget canvas, XtPointer client, XEvent *ev, Boolean *cont)
{
	Display	*dpy = XtDisplay(canvas) ;
	Window	win = XtWindow(canvas) ;
	int	x0,y0 ;

	if( !canvas_init )
	{
	  Colormap cmap ;
	  XColor	y,b, dummy ;
	  XtVaGetValues(canvas, XtNcolormap, &cmap, 0) ;
	  canvasGc = XCreateGC(dpy,win, 0,NULL) ;
	  XSetLineAttributes(dpy, canvasGc, 8, LineSolid, CapButt, JoinRound);
	  XSetGraphicsExposures(dpy, canvasGc, True) ;
	  XAllocNamedColor(dpy, cmap, "yellow", &y, &dummy) ; yellow = y.pixel ;
	  XAllocNamedColor(dpy, cmap, "black", &b, &dummy) ; black = b.pixel ;
	  canvas_init = True ;
	}

	switch( ev->type ) {
	  case GraphicsExpose:
	  case Expose:
	    x0 = MwRulerValue2Position(topRuler, 1.) ;
	    y0 = MwRulerValue2Position(leftRuler, 1.) - 100 ;
	    XSetForeground(dpy,canvasGc, yellow) ;
	    XFillArc(dpy,win,canvasGc, x0,y0, 100,100, 0,360*64) ;
	    XSetForeground(dpy,canvasGc, black) ;
	    XDrawArc(dpy,win,canvasGc, x0,y0, 100,100, 0,360*64) ;
	    XFillArc(dpy,win,canvasGc, x0+30,y0+30, 10,10, 0,360*64) ;
	    XFillArc(dpy,win,canvasGc, x0+60,y0+30, 10,10, 0,360*64) ;
	    XDrawArc(dpy,win,canvasGc, x0+20,y0+20, 60,60, 225*64,90*64) ;
	    break ;
	  case EnterNotify:
	    MwRulerShowPointer(leftRuler, True) ;
	    MwRulerShowPointer(topRuler, True) ;
	    break ;
	  case LeaveNotify:
	    MwRulerShowPointer(leftRuler, False) ;
	    MwRulerShowPointer(topRuler, False) ;
	    break ;
	  case MotionNotify:
	    MwRulerSetIValue(leftRuler, ev->xmotion.y) ;
	    MwRulerSetIValue(topRuler, ev->xmotion.x) ;
	    break ;
	}

}


static	void
WMProtocols(w, ev, params, nparams)
	Widget	w ;
	XEvent	*ev ;
	String	*params ;
	Cardinal *nparams ;
{
	if( ev->type == ClientMessage  &&
	    ev->xclient.message_type == wm_protocols  &&
	    ev->xclient.data.l[0] == wm_delete_window )
	{
	  if( w == topLevel )
	    exit(0) ;
	}
}
