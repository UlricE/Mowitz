/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <stdio.h>
#include "MwUtils.h"

#include	"MwMenuBarP.h"
#include	"MwNws.h"
#define HIDE_PROTO 1
#include	"MwMBButtonP.h"
#undef HIDE_PROTO 

static XtResource menuBarConstraintsResources [] = {
        {
         XtNleft_space ,
         XtCLeft_space ,
         XtRInt ,
         sizeof(int) ,
         XtOffsetOf(MwMenuBarConstraintsRec , row.left_space) ,
         XtRImmediate ,
         (XtPointer) 0
        },
        {
         XtNright_space ,
         XtCRight_space ,
         XtRInt ,
         sizeof(int) ,
         XtOffsetOf(MwMenuBarConstraintsRec , row.right_space) ,
         XtRImmediate ,
         (XtPointer) 0
        },
};

#define offset(field) XtOffsetOf(MwMenuBarRec, menuBar.field)

static void ClassInitialize(void);
static void Initialize (Widget, Widget, ArgList, Cardinal *);
static void ResolveInheritance (WidgetClass);
static void Redisplay(Widget, XEvent *, Region);

static void DispatchEvent (Widget, XtPointer, XEvent *, Boolean *);
static void GetPositionEntry (Widget, int, int, XEvent *, MwBaseMEObject **);

static void Unactivate(Widget);

MwMenuBarClassRec mwMenuBarClassRec = {
/* core */
   {
    /* superclass            */ (WidgetClass) &mwRowClassRec,
    /* class_name            */ "MwMenuBar",
    /* widget_size           */ sizeof(MwMenuBarRec),
    /* class_initialize      */ ClassInitialize,
    /* class_part_initialize */ ResolveInheritance,
    /* class_inited          */ FALSE,
    /* initialize            */ Initialize,
    /* initialize_hook       */ NULL,
    /* realize               */ XtInheritRealize,
    /* actions               */ NULL,
    /* num_actions           */ 0,
    /* resources             */ NULL,
    /* num_resources         */ 0,
    /* xrm_class             */ NULLQUARK,
    /* compress_motion       */ False,
    /* compress_exposure     */ True,
    /* compress_enterleave   */ True,
    /* visible_interest      */ FALSE,
    /* destroy               */ NULL,
    /* resize                */ XtInheritResize,
    /* expose                */ Redisplay,
    /* set_values            */ NULL,
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
/* composite */
   {
    /* geometry_manager	     */ XtInheritGeometryManager,
    /* change_managed	     */ XtInheritChangeManaged,
    /* insert_child	     */ XtInheritInsertChild,
    /* delete_child	     */ XtInheritDeleteChild,
    /* extension	     */ NULL
   },
   {
/* constraint */
    /* subresourses       */   menuBarConstraintsResources ,
    /* subresource_count  */   XtNumber(menuBarConstraintsResources) ,
    /* constraint_size    */   sizeof(MwMenuBarConstraintsRec),
    /* initialize         */   NULL,
    /* destroy            */   NULL,
    /* set_values         */   NULL,
    /* extension          */   NULL
   },
/* baseConst */
   {
    /* get_internal_dimension  */ XtInheritGetInternalDimension,
    /* set_internal_dimension  */ XtInheritSetInternalDimension,
    /* traverse		       */ XtInheritTraverse,
    /* traverseTo              */ XtInheritTraverseTo,
    /* traverseOut	       */ XtInheritTraverseOut,
    /* traverseInside          */ XtInheritTraverseInside,
    /* highlightBorder         */ XtInheritHighlightBorder,
    /* unhighlightBorder       */ XtInheritUnhighlightBorder,
   },
/* row */
   {
    /* empty		       */ 0
   },
/* menuBar */
   {
    /* unactivate	       */ Unactivate
   },
};

WidgetClass mwMenuBarWidgetClass = (WidgetClass) & mwMenuBarClassRec;

#define ForAllChildren(cw, childP) \
	for ( (childP) = (MwBaseMEObject *) (cw)->composite.children ; \
		(childP) < (MwBaseMEObject *) ((cw)->composite.children + \
		(cw)->composite.num_children ) ; \
		(childP)++ )


static void ClassInitialize(void)
{
	_InitializeWidgetSet();
}

static void ResolveInheritance(WidgetClass class)
{
        MwMenuBarWidgetClass c = (MwMenuBarWidgetClass) class;
        MwMenuBarWidgetClass super;
        static CompositeClassExtensionRec extension_rec = {
                NULL, NULLQUARK, XtCompositeExtensionVersion,
                sizeof(CompositeClassExtensionRec), True };
        CompositeClassExtensionRec *ext;

        ext = MwMalloc(sizeof(*ext));
        *ext = extension_rec;
        ext->next_extension = c->composite_class.extension;
        c->composite_class.extension = ext;

	if (class == mwMenuBarWidgetClass) return;

	super = (MwMenuBarWidgetClass)class->core_class.superclass;

	if (c->menuBar_class.unactivate == XtInheritUnactivate)
                c->menuBar_class.unactivate =
                        super->menuBar_class.unactivate;


}

static void Initialize(Widget req_widget, Widget new_widget,
		ArgList args, Cardinal *num_args)
{
	MwMenuBarWidget nw = (MwMenuBarWidget) new_widget;

	nw->menuBar.is_activated = False;
	nw->menuBar.do_popdown = False;
	nw->menuBar.have_grab = False;

	XtAddEventHandler(new_widget ,   ButtonPressMask | 
                ButtonMotionMask | PointerMotionMask,
                True , DispatchEvent , (XtPointer)NULL);

}

static void Redisplay(Widget w, XEvent *event, Region region)
{
	MwMenuBarWidget cw = (MwMenuBarWidget) w;
        MwBaseMEObject * entry;   
        MwBaseMEObjectClass class;

        if (region == NULL) XClearWindow(XtDisplay(w) , XtWindow(w));

	mwRowClassRec.core_class.expose(w,event,region);

        ForAllChildren(cw, entry)
        {
                if (!XtIsManaged ( (Widget) *entry)) continue;

                if (region != NULL) 
                        switch(XRectInRegion(region , (*entry)->rectangle.x,
                                (*entry)->rectangle.y , (*entry)->rectangle.width ,
                                (*entry)->rectangle.height)) 
                        {

                                case RectangleIn:
                                case RectanglePart:
                                        break;
                                default:
                                        continue;
                        }
                class = (MwBaseMEObjectClass) (*entry)->object.widget_class;

                if (class->rect_class.expose != NULL)
                        (class->rect_class.expose)( (Widget) *entry, NULL, NULL);
        }

}

/* volane ak je menu zrusene 
 treba osetrit flag 'do_popdown' ktory zisti ci menu zrusil 
 menuBar alebo bolo zrusene z ineho dovodu
 metoda je volana z MBButton !!!!!!!!!!!!!!!!!!!!!!!!
 */
 
static void Unactivate(Widget w)
{
	MwMenuBarWidget cw = (MwMenuBarWidget) w;

	if (cw->menuBar.is_activated && !cw->menuBar.do_popdown)
	{
		cw->menuBar.is_activated = False;
		cw->menuBar.last_activated = NULL;
	}
}

static void DispatchEvent(Widget w , XtPointer client_data ,
		XEvent *event , Boolean *ctd)
{
	MwMenuBarWidget cw = (MwMenuBarWidget) w;
	MwMBButtonObject *entry =  NULL;
	MwMBButtonObjectClass class;

	switch(event->type)
	{
		case ButtonPress:
			if (cw->menuBar.is_activated)
			{
				if (cw->menuBar.last_activated)
				{
					class = (MwMBButtonObjectClass)
						((MwMBButtonObject)
						*cw->menuBar.last_activated)
						->object.widget_class;


					if (class->mBButton_class.popdown_menu)
						class->mBButton_class.
						popdown_menu((Widget) 
						*cw->menuBar.last_activated);

				}

				cw->menuBar.is_activated = False;
				cw->menuBar.last_activated = NULL;

				return;
			}

			GetPositionEntry(w , event->xbutton.x , event->xbutton.y ,
				event , (MwBaseMEObject **)&entry);

			if (entry)
			{
				class = (MwMBButtonObjectClass)
					(*entry)->object.widget_class;

				if (class->baseME_class.activate)
					class->baseME_class.activate((Widget) *entry);


				if ((*entry)->mBButton.menu)
				{
					XtAddGrab(w,False,False);
					cw->menuBar.is_activated = True;
				}
			}
			cw->menuBar.last_activated = (Widget *) entry;
			break;

		case MotionNotify:
			if (!cw->menuBar.is_activated) return;

			GetPositionEntry(w , event->xmotion.x , event->xmotion.y ,
				event , (MwBaseMEObject **)&entry);

			if (entry)
			{
				if (cw->menuBar.last_activated &&
				   cw->menuBar.last_activated != (Widget *) entry)
				{
					class = (MwMBButtonObjectClass)
						((MwMBButtonObject)
						*cw->menuBar.last_activated)
						->object.widget_class;

					cw->menuBar.do_popdown = True;

					if (class->mBButton_class.popdown_menu)
						class->mBButton_class.
						popdown_menu((Widget) 
						*cw->menuBar.last_activated);

					cw->menuBar.do_popdown = False;

					cw->menuBar.last_activated = (Widget *) entry;

					class = (MwMBButtonObjectClass)
						(*entry)->object.widget_class;

					if (class->baseME_class.activate)
						class->baseME_class.activate((Widget) 
						*entry);

					if ((*entry)->mBButton.menu)
					{
						XtAddGrab(w,False,False);
					}
				}
			}
			break;
		default:
			printf("unsupported event type\n");
	}
}

static void GetPositionEntry(Widget w , int x , int y ,
		XEvent *event , MwBaseMEObject **entry )
{
        MwMenuBarWidget cw = (MwMenuBarWidget) w;
        MwBaseMEObject * pentry;
        Position pomx , pomy;
        Dimension width , height;

        if (XtWindow(w) != event->xany.window) return;

        mwMenuBarClassRec.baseConst_class.get_internal_dimension(w , &pomx , &pomy , 
                &width , &height);

        if ((x < pomx) || (x > pomx + width) || (y < pomy) || (y > pomy + height)) return;
        
        ForAllChildren(cw , pentry)
        {
                if (!XtIsManaged ((Widget) *pentry)) continue;

                if ( ( (*pentry)->rectangle.x <= x) && (( (*pentry)->rectangle.x +
                        (int) (*pentry)->rectangle.width) >= x) 
                        && (*pentry)->rectangle.sensitive)
                {
                        *entry = pentry;
                        return;
                }
                        
                        
        }

}
