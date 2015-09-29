/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "MwUtils.h"

#include "MwPopTextP.h"
#include "MwNws.h"

#define offset(field) XtOffsetOf(MwPopTextRec, popText.field)

static XtResource resources[] = { 
	{
	 XtNfont,
	 XtCFont,
	 XtRFontStruct ,
	 sizeof(XFontStruct *) ,
	 offset(font) ,
	 XtRString ,
	 (XtPointer) XtDefaultFont 
	},
	{
	 XtNforeground,
	 XtCForeground,
	 XtRPixel,
	 sizeof(Pixel),
	 offset(foreground),
	 XtRString,
	 (XtPointer) XtDefaultForeground
	},
	{
	 XtNtext,
	 XtCText,
	 XtRString,
	 sizeof(String),
	 offset(text),
	 XtRImmediate,
	 (XtPointer) "  "
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

static void ClassInitialize (void);
static void Initialize (Widget, Widget, ArgList, Cardinal *);
static Boolean SetValues (Widget, Widget, Widget, ArgList, Cardinal *);
static void Redisplay (Widget, XEvent *, Region);
static void Destroy (Widget);

MwPopTextClassRec mwPopTextClassRec = {
/* core */
   {
    /* superclass            */ (WidgetClass) &overrideShellClassRec,
    /* class_name            */ "MwPopText",
    /* widget_size           */ sizeof(MwPopTextRec),
    /* class_initialize      */ ClassInitialize,
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
    /* compress_motion       */ False,
    /* compress_exposure     */ False,
    /* compress_enterleave   */ False,
    /* visible_interest      */ FALSE,
    /* destroy               */ Destroy,
    /* resize                */ XtInheritResize,
    /* expose                */ Redisplay,
    /* set_values            */ (XtSetValuesFunc) SetValues,
    /* set_values_hook       */ NULL,
    /* set_values_almost     */ XtInheritSetValuesAlmost,
    /* get_values_hook       */ NULL,
    /* accept_focus          */ XtInheritAcceptFocus,
    /* version               */ XtVersion,
    /* callback_private      */ NULL,
    /* tm_table              */ NULL,
    /* query_geometry        */ XtInheritQueryGeometry,
    /* display_accelerator   */ XtInheritDisplayAccelerator,
    /* extension             */ NULL
   },
   {
/* composite */
    /* geometry_manager      */ XtInheritGeometryManager,
    /* change_managed        */ XtInheritChangeManaged,
    /* insert_child          */ XtInheritInsertChild,
    /* delete_child          */ XtInheritDeleteChild,
    /* extension             */ NULL 
   },
   {
/* shell */
    /* extension	     */	NULL
   },
/* override shell */
   {
    /* extension	     */ NULL
   },
/* popText */
   {
    /* dummy 		     */ 0
   },
};

WidgetClass mwPopTextWidgetClass = (WidgetClass) &mwPopTextClassRec;

static void ClassInitialize(void)
{
	_InitializeWidgetSet();
}

static void Initialize(Widget req_widget, Widget new_widget,
		ArgList args, Cardinal *num_args)
{
	MwPopTextWidget	nw = (MwPopTextWidget) new_widget;
	XGCValues gc_res;
	XtGCMask  gc_mask;

	nw->popText.text = MwStrdup(nw->popText.text);

	gc_res.background = nw->core.background_pixel;
	gc_res.foreground = nw->popText.foreground;
	gc_res.font = nw->popText.font->fid;
	
	gc_mask = GCForeground | GCBackground | GCFont;
	
	nw->popText.gc = XCreateGC(XtDisplay(new_widget),
			DefaultRootWindow(XtDisplay(new_widget)),
			gc_mask, &gc_res);
	
	XtVaSetValues(new_widget ,
			XtNwidth , 
			max_line_width(nw->popText.text,nw->popText.font) +
			2*nw->popText.spacing ,
			XtNheight ,
			2 * nw->popText.spacing + 
			(nw->popText.font->max_bounds.ascent +
			nw->popText.font->max_bounds.descent) *
			cnt_lines(nw->popText.text),
			NULL);

}

static void Redisplay(Widget w, XEvent *event, Region region)
{
	MwPopTextWidget cw = (MwPopTextWidget) w;
	Window win = XtWindow(w);
	Display * dpy = XtDisplay(w);
	int i;
	char *s1,*s,*p;
	
	if (cw->popText.text)
	{
		s1=s=p=MwMalloc(strlen(cw->popText.text)+1);
		strcpy(s,cw->popText.text);
		XClearWindow(dpy,win);
		
		for(i=1; ;i++)
		{
			p=strchr(s,'\n');
			if (p!=NULL) *p=0;
			
			XDrawString(dpy,win,cw->popText.gc , cw->popText.spacing ,
				 cw->popText.spacing +
				(i - 1) * cw->popText.font->max_bounds.descent +
				i * cw->popText.font->max_bounds.ascent, s ,strlen(s));

			if (p==NULL) break;

			s=++p;
		}
		MwFree(s1);
	}
}

#define WidgetValuesDiffer(w1,w2,component) (w1 -> popText.component != \
                                             w2 -> popText.component)

static Boolean SetValues(Widget current, Widget request, Widget new_widget,
		ArgList args, Cardinal *num_args)
{
	MwPopTextWidget cw = (MwPopTextWidget) current;
	MwPopTextWidget nw = (MwPopTextWidget) new_widget;

	if WidgetValuesDiffer(nw , cw , font)
	{
		XSetFont(XtDisplay(current),nw->popText.gc,nw->popText.font->fid);
	}

	if WidgetValuesDiffer(nw , cw , foreground)
	{
		XSetFont(XtDisplay(current),nw->popText.gc,nw->popText.foreground);
	}

	if (WidgetValuesDiffer(nw , cw , text) || 
		WidgetValuesDiffer(nw , cw , font) ||
		WidgetValuesDiffer(nw , cw , spacing))
			XtVaSetValues(new_widget ,
				XtNwidth , 
				max_line_width(nw->popText.text,nw->popText.font) +
				2*nw->popText.spacing ,
				XtNheight ,
				2 * nw->popText.spacing + 
				(nw->popText.font->max_bounds.ascent +
				nw->popText.font->max_bounds.descent) *
				cnt_lines(nw->popText.text),
				NULL);

	if WidgetValuesDiffer(nw , cw , text)
	{
		if (cw->popText.text)
		{
			MwFree(cw->popText.text);
			cw->popText.text = NULL;
		}
	
		if (nw->popText.text) nw->popText.text = MwStrdup(nw->popText.text);
	}
	return True;
}

static void Destroy(Widget w)
{
	MwPopTextWidget cw = (MwPopTextWidget) w;

	MwFree(cw->popText.text);
}

