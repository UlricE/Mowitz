
/*
 * MwTooltip.c - Source code file for MwTooltip widget.
 */

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include "MwTooltipP.h"
#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/Label.h>

#include "MwUtils.h"

#define offset(field) XtOffsetOf(MwTooltipRec, tooltip.field)

static XtResource resources[] = { 
	{
		XtNtooltipLabel,	/* name */
		XtCTooltipLabel,	/* class */
		XtRWidget,		/* type */
		sizeof(Widget),		/* size */
		offset(label),		/* offset */
		XtRImmediate,		/* default_type */
		(XtPointer)None		/* default_addr */
	}, {
		XtNtooltipMode,
		XtCTooltipMode,
		XtRInt,
		sizeof(int),
		offset(mode),
		XtRImmediate,
		(XtPointer)TOOLTIP_POPUP
	}, {
		XtNtooltipInterval,
		XtCTooltipInterval,
		XtRInt,
		sizeof(int),
		offset(interval),
		XtRImmediate,
		(XtPointer)500
	}, {
		XtNheight,
		XtCHeight,
		XtRDimension,
		sizeof(Dimension),
		XtOffsetOf(CoreRec, core.height),
		XtRImmediate,
		(XtPointer)20
	}
};

#undef offset


/*
 * Semi Public function definitions. 
 */

static void Resize(Widget), ChangeManaged(Widget);
static void Initialize(Widget, Widget, ArgList, Cardinal *),
		ClassPartInitialize(WidgetClass);
static void Destroy(Widget), DoLayout(Widget);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult GeometryManager(Widget,
		XtWidgetGeometry *, XtWidgetGeometry *);

static CompositeClassExtensionRec extension_rec = {
    /* next_extension */  NULL,
    /* record_type */     NULLQUARK,
    /* version */         XtCompositeExtensionVersion,
    /* record_size */     sizeof(CompositeClassExtensionRec),
    /* accepts_objects */ TRUE,
};

#define superclass (&overrideShellClassRec)
    
MwTooltipClassRec mwTooltipClassRec = {
  {
    /* superclass         */    (WidgetClass) superclass,
    /* class_name         */    "MwTooltip",
    /* size               */    sizeof(MwTooltipRec),
    /* class_initialize   */	NULL,
    /* class_part_initialize*/	ClassPartInitialize,
    /* Class init'ed      */	FALSE,
    /* initialize         */    Initialize,
    /* initialize_hook    */	NULL,
    /* realize            */    XtInheritRealize,
    /* actions            */    NULL,
    /* num_actions        */    0,
    /* resources          */    resources,
    /* resource_count     */	XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion    */    TRUE, 
    /* compress_exposure  */    TRUE,
    /* compress_enterleave*/ 	TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    Destroy,
    /* resize             */    Resize,
    /* expose             */    NULL,
    /* set_values         */    SetValues,
    /* set_values_hook    */	NULL,
    /* set_values_almost  */	XtInheritSetValuesAlmost,  
    /* get_values_hook    */	NULL,			
    /* accept_focus       */    NULL,
    /* intrinsics version */	XtVersion,
    /* callback offsets   */    NULL,
    /* tm_table		  */    NULL,
    /* query_geometry	  */    XtInheritQueryGeometry,
    /* display_accelerator*/    XtInheritDisplayAccelerator,
    /* extension	  */    NULL
  },{
    /* geometry_manager   */    GeometryManager,
    /* change_managed     */    ChangeManaged,
    /* insert_child	  */	XtInheritInsertChild,
    /* delete_child	  */	XtInheritDeleteChild,
    /* extension	  */    NULL
  },{
    /* Shell extension	  */    NULL
  },{
    /* Override extension */    NULL
  },{
    /* Simple Menu extension*/  NULL
  }
};

WidgetClass mwTooltipWidgetClass = (WidgetClass)&mwTooltipClassRec;


/*      Function Name: ClassPartInitialize
 *      Description: Class Part Initialize routine, called for every
 *                   subclass.  Makes sure that the subclasses pick up 
 *                   the extension record.
 *      Arguments: wc - the widget class of the subclass.
 *      Returns: none.
 */

static void ClassPartInitialize(WidgetClass wc)
{
    MwTooltipWidgetClass smwc = (MwTooltipWidgetClass) wc;

/*
 * Make sure that our subclass gets the extension rec too.
 */

    extension_rec.next_extension = smwc->composite_class.extension;
    smwc->composite_class.extension = (XtPointer) &extension_rec;
}

/*      Function Name: Initialize
 *      Description: Initializes the tooltip widget
 *      Arguments: request - the widget requested by the argument list.
 *                 new     - the new widget with both resource and non
 *                           resource values.
 *      Returns: none.
 */

/* ARGSUSED */
static void Initialize(Widget request, Widget new,
		ArgList args, Cardinal *num_args)
{
  MwTooltipWidget smw = (MwTooltipWidget) new;

  smw->tooltip.plabel = XtVaCreateManagedWidget("popup_label",
	labelWidgetClass, new,
	(char *)0);
  smw->tooltip.timer = None;
}

static void Resize(Widget w)
{
	DoLayout(w);
}

static void DoLayout(Widget w)
{
	int i;
	MwTooltipWidget tw = (MwTooltipWidget)w;

	for (i = 0; i < tw->composite.num_children; i++) {
		Widget cw = tw->composite.children[i];
		XtConfigureWidget(cw,
			0, 0, tw->core.width, tw->core.height, 0);
	}
}

static void Destroy(Widget w)
{
	MwTooltipWidget tw = (MwTooltipWidget)w;

	while (tw->tooltip.plist)
		MwTooltipRemove(w, tw->tooltip.plist->w);
	if (tw->tooltip.plabel)
		XtDestroyWidget(tw->tooltip.plabel);
}

static Boolean SetValues(Widget current, Widget request, Widget new,
			 ArgList args, Cardinal *num_args)
{
	return False;
}

/************************************************************
 *
 * Geometry Management routines.
 *
 ************************************************************/

static XtGeometryResult GeometryManager(Widget w,
	XtWidgetGeometry *request, XtWidgetGeometry *reply)
{
	return XtGeometryYes;
}

static void ChangeManaged(Widget w)
{
    DoLayout(w);
}


void MwTooltipAddGlobalActions(XtAppContext app_con)
{
    XtInitializeWidgetClass(mwTooltipWidgetClass);
} 

static void tooltip_popup(XtPointer client_data, XtIntervalId *id)
{
	Widget w = (Widget)client_data;
	MwTooltipWidget tw = (MwTooltipWidget)w;
        tw->tooltip.timer = None;
        XtPopup(w, XtGrabNone);
}

static void tooltip_popdown(MwTooltipWidget tw)
{
        if (tw->tooltip.timer != None)
                XtRemoveTimeOut(tw->tooltip.timer);
        tw->tooltip.timer = None;
        XtPopdown((Widget)tw);
}


static void tooltip_show(Widget w, XtPointer p, XEvent *event, Boolean *n)
{
	MwTooltipWidget tw = (MwTooltipWidget)p;
	char *t = MwTooltipGet((Widget)tw, w);

	if (t == NULL) return;
        if ((tw->tooltip.mode & TOOLTIP_LABEL)
	     && (tw->tooltip.label != None)) {
                XtVaSetValues(tw->tooltip.label,
			XtNlabel, (String)t,
			(char *)0);
        }
        if (tw->tooltip.mode & TOOLTIP_POPUP) {
		int x, y;
		Window child;
                Dimension height, width;
                XFontStruct *font;

                XtVaGetValues(w,
                        XtNheight, &height, (char *)0);
		XTranslateCoordinates(XtDisplay(w),
			XtWindow(w), DefaultRootWindow(XtDisplay(w)),
			0, height+10,
			&x, &y,
			&child);
                XtVaGetValues(tw->tooltip.plabel,
                        XtNfont, &font, (char *)0);
                width = XTextWidth(font, t, strlen(t));
                XtVaSetValues((Widget)tw,
                        XtNx, x,
                        XtNy, y,
                        XtNwidth, width+8,
			(char *)0);
                XtVaSetValues(tw->tooltip.plabel,
			XtNlabel, t,
			(char *)0);
                tw->tooltip.timer = XtAppAddTimeOut(
			XtWidgetToApplicationContext(w),
                        tw->tooltip.interval,
                        tooltip_popup, (XtPointer)tw);
        }
}

static void tooltip_hide(Widget w, XtPointer p, XEvent *event, Boolean *n)
{
	MwTooltipWidget tw = (MwTooltipWidget)p;

        if ((tw->tooltip.mode & TOOLTIP_LABEL)
	    && (tw->tooltip.label != None)) {
                XtVaSetValues(tw->tooltip.label,
			      XtNlabel, "",
			      (char *)0);
        }
        if (tw->tooltip.mode & TOOLTIP_POPUP) {
                tooltip_popdown(tw);
        }
}

void MwTooltipAdd(Widget w, Widget w2, char *p)
{
	MwTooltipWidget tw = (MwTooltipWidget)w;
	p_list *entry = (p_list *)MwMalloc(sizeof(p_list));

	entry->w = w2;
	entry->p = MwStrdup(p);
	entry->next = tw->tooltip.plist;
	tw->tooltip.plist = entry;
	XtAddEventHandler(w2, EnterWindowMask, False,
			 tooltip_show, (XtPointer)w);
	XtAddEventHandler(w2, LeaveWindowMask, False,
			 tooltip_hide, (XtPointer)w);
}

char *MwTooltipGet(Widget w, Widget w2)
{
	MwTooltipWidget tw = (MwTooltipWidget)w;
	char *p = NULL;

	p_list *entry;

	for (entry = tw->tooltip.plist; entry; entry = entry->next) {
		if (entry->w == w2) {
			p = entry->p;
			break;
		}
	}
	return p;
}

void MwTooltipRemove(Widget w, Widget w2)
{
	MwTooltipWidget tw = (MwTooltipWidget)w;

	p_list *entry = tw->tooltip.plist, *tmp = NULL;

	if (entry == NULL) return;	/* no tooltips at all */
	if (entry->w == w2) {		/* remove first tooltip */
		tmp = entry;
		tw->tooltip.plist = entry->next;
	}
	while (entry->next) {		/* check all others */
		if (entry->next->w == w2) {
			p_list *tmp = entry->next;
			entry->next = tmp->next;
		}
		entry = entry->next;
	}
	if (tmp) {
		MwFree(tmp->p);
		MwFree(tmp);
	}
}
 
    
