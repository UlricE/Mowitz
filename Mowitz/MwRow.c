/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include	"MwRowP.h"
#include 	"MwNws.h"
#include	"MwTraverse.h"


static XtResource rowConstraintsResources [] = {
	{
	 XtNgravitation ,
	 XtCGravitation ,
	 XtRNwsJustify ,
	 sizeof(int) ,
	 XtOffsetOf(MwRowConstraintsRec , row.gravitation) ,
	 XtRImmediate ,
	 (XtPointer) XtCleft
        },
        {
         XtNleft_space ,
         XtCLeft_space ,
         XtRInt ,
         sizeof(int) ,
         XtOffsetOf(MwRowConstraintsRec , row.left_space) ,
         XtRImmediate ,
         (XtPointer) 1
        },
        {
         XtNright_space ,
         XtCRight_space ,
         XtRInt ,
         sizeof(int) ,
         XtOffsetOf(MwRowConstraintsRec , row.right_space) ,
         XtRImmediate ,
         (XtPointer) 1
        },
        {
         XtNresizable ,
         XtCResizable ,
         XtRBoolean ,
         sizeof(Boolean) ,
         XtOffsetOf(MwRowConstraintsRec , row.resizable) ,
         XtRImmediate ,
         (XtPointer) False
        },
};


#define offset(field) XtOffsetOf(MwRowRec, row.field)

static XtResource resources[] = {
	{
         XtNspacing ,
         XtCSpacing ,
         XtRInt ,
         sizeof(int) ,
         offset(spacing) ,
         XtRImmediate ,
         (XtPointer) 1
        },
	{
         XtNhomogenous ,
         XtCHomogenous ,
         XtRBoolean ,
         sizeof(Boolean) ,
         offset(homogenous) ,
         XtRImmediate ,
         (XtPointer) FALSE
        },
        {
	 XtNbox_type ,
	 XtCBox_type ,
	 XtRBox_type ,
	 sizeof(int) ,
	 XtOffsetOf(MwRowRec , baseConst.box_type) ,
	 XtRImmediate ,
	 (XtPointer) XtCup_box 
	},
};

static void ClassInitialize(void);
static void ChangeManaged (Widget);
static XtGeometryResult GeometryManager (Widget,
		XtWidgetGeometry *, XtWidgetGeometry *);
static XtGeometryResult QueryGeometry(Widget,
		XtWidgetGeometry *, XtWidgetGeometry *);
static void Resize (Widget);
static Widget TraverseInside(Widget, int, Time *);
static void Layout(Widget);

MwRowClassRec mwRowClassRec = {
/* core */
   {
    /* superclass            */ (WidgetClass) &mwBaseConstClassRec,
    /* class_name            */ "MwRow",
    /* widget_size           */ sizeof(MwRowRec),
    /* class_initialize      */ ClassInitialize,
    /* class_part_initialize */ NULL,
    /* class_inited          */ FALSE,
    /* initialize            */ NULL,
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
    /* destroy               */ NULL,
    /* resize                */ Resize,
    /* expose                */ XtInheritExpose,
    /* set_values            */ NULL,
    /* set_values_hook       */ NULL,
    /* set_values_almost     */ XtInheritSetValuesAlmost,
    /* get_values_hook       */ NULL,
    /* accept_focus          */ XtInheritAcceptFocus,
    /* version               */ XtVersion,
    /* callback_private      */ NULL,
    /* tm_table              */ XtInheritTranslations,
    /* query_geometry        */ QueryGeometry,
    /* display_accelerator   */ XtInheritDisplayAccelerator,
    /* extension             */ NULL
   },
/* composite */
   {
    /* geometry_manager	     */ GeometryManager,
    /* change_managed	     */ ChangeManaged,
    /* insert_child	     */ XtInheritInsertChild,
    /* delete_child	     */ XtInheritDeleteChild,
    /* extension	     */ NULL
   },
   {
/* constraint */
    /* subresourses       */   rowConstraintsResources,
    /* subresource_count  */   XtNumber(rowConstraintsResources),
    /* constraint_size    */   sizeof(MwRowConstraintsRec),
    /* initialize         */   NULL,
    /* destroy            */   NULL,
    /* set_values         */   NULL,
    /* extension          */   NULL
   },
/* baseConst */
   {
    /* get_internal_dimension  */ XtInheritGetInternalDimension,
    /* set_internal_dimension  */ XtInheritSetInternalDimension,
    /* traverse                */ XtInheritTraverse,
    /* traverseTo              */ XtInheritTraverseTo,
    /* traverseOut	       */ XtInheritTraverseOut,
    /* traverseInside          */ TraverseInside,
    /* highlightBorder         */ XtInheritHighlightBorder,
    /* unhighlightBorder       */ XtInheritUnhighlightBorder,
   },
/* row */
   {
    /* empty		       */ 0
   },
};


WidgetClass rowWidgetClass = (WidgetClass) & mwRowClassRec;



#define ForAllChildren(cw, child) \
        for ( (child) = (cw)->composite.children ; \
                (child) < ((cw)->composite.children + \
                (cw)->composite.num_children ) ; \
                (child)++ )

static void ClassInitialize(void)
{
	_InitializeWidgetSet();

	XtSetTypeConverter(XtRString, XtRNwsJustify, cvtStringToJustify,
		NULL, 0, XtCacheNone, NULL);
}


static void Resize(Widget w)
{
	Layout(w);
}

static XtGeometryResult GeometryManager(Widget w ,
		XtWidgetGeometry *request , XtWidgetGeometry *reply)
{

        return XtGeometryYes;
}

static XtGeometryResult QueryGeometry(Widget w,
		XtWidgetGeometry *intended , XtWidgetGeometry *preferred)
{
        MwRowWidget cw = (MwRowWidget) w;
	XtWidgetGeometry pref_geom , inten_geom;
	Widget *child;
	Dimension width , height;
	Position x,y;
	int lspace , rspace;
	int maxwidth = 0;
	int chnum = 0;
	
	mwRowClassRec.baseConst_class.get_internal_dimension(w , &x , &y , &width,&height);

	preferred->request_mode = CWWidth | CWHeight;
	preferred->width = cw->core.width - width + 2 * cw->row.spacing;
	preferred->height = cw->core.height - height + 2 * cw->row.spacing;
	
	inten_geom.request_mode = CWWidth | CWHeight;
	inten_geom.width = width;
	inten_geom.height = height;

	ForAllChildren(cw , child)
	{
		if (!XtIsManaged(*child)) continue;

		XtQueryGeometry(*child , &inten_geom , &pref_geom);		
		XtVaGetValues(*child , XtNleft_space , &lspace ,
			XtNright_space , &rspace , NULL);

		preferred->width += lspace + rspace  + pref_geom.width;
		preferred->height = MW_MAX (preferred->height , pref_geom.height + 
			2 * cw->row.spacing);

		chnum++;
		maxwidth = MW_MAX(maxwidth , lspace + rspace  + pref_geom.width);
	}

	if (cw->row.homogenous)
		preferred->width = cw->core.width - width + 2 * cw->row.spacing +
				chnum * maxwidth; 


	if (((intended->request_mode & (CWWidth | CWHeight))
		== (CWWidth | CWHeight)) &&
		intended->width == preferred->width &&
		intended->height == preferred->height)
		return XtGeometryYes;

	else if (preferred->width == cw->core.width &&
		preferred->height == cw->core.height)
		return XtGeometryNo;

	else return XtGeometryAlmost;
}

static void ChangeManaged(Widget w)
{
	if (!XtIsRealized(w))
	{
		XtWidgetGeometry pref_geom , inten_geom;

		inten_geom.request_mode = CWWidth | CWHeight;
		inten_geom.width = w->core.width;
		inten_geom.height = w->core.height;

		((MwRowWidgetClass)XtClass(w))->core_class.query_geometry(w ,
			&inten_geom , &pref_geom);

		w->core.width = pref_geom.width;
		w->core.height = pref_geom.height;
	}
	Layout(w);
}

static void Layout(Widget w)
{
	MwRowWidget cw = (MwRowWidget) w;
	XtWidgetGeometry pref_geom , inten_geom;
	Widget *child;
	Dimension width , height;
	Dimension chwidth , chheight ,bwidth;
	Position x,y;
	int lposition , rposition;
	int gravitation;
	int lspace , rspace;
	Boolean resizable;
	int normalwidth = 0 , ressize = 0;
	double psize;
	int chnum = 0;
	int rawwidth;

	inten_geom.request_mode = CWWidth | CWHeight;
	inten_geom.width = cw->core.width;
	inten_geom.height = cw->core.height;

	mwRowClassRec.baseConst_class.get_internal_dimension(w , &x , &y , &width,&height);

	rawwidth = width;

	ForAllChildren(cw , child)
	{

		if (!XtIsManaged(*child)) continue;

		XtVaGetValues(*child , XtNwidth , &chwidth ,
			XtNleft_space , &lspace ,
			XtNright_space , &rspace ,
			XtNresizable , &resizable ,
			NULL);

		if (resizable)
		{
			XtQueryGeometry(*child , &inten_geom , &pref_geom);
			ressize += pref_geom.width;
			chwidth = 0;
		}

		chnum ++;
		rawwidth -= rspace + lspace;
		normalwidth += rspace + lspace + chwidth;
	}	

	if (ressize)
	{
		psize = (double) (width - (cw->core.width - width) -normalwidth) / 
			(double) ressize;
	}
	else
	{
		psize = 0;
	}

	inten_geom.request_mode = CWWidth | CWHeight;
	inten_geom.width = width;
	inten_geom.height = height;

	lposition = x + cw->row.spacing;
	rposition = x + width - cw->row.spacing;	

	ForAllChildren(cw , child)
	{

		if (!XtIsManaged(*child)) continue;

		XtVaGetValues(*child , XtNheight , &chheight ,
			XtNwidth , &chwidth ,
			XtNgravitation , &gravitation ,
			XtNborderWidth , &bwidth ,
			XtNleft_space , &lspace ,
			XtNright_space , &rspace ,
			XtNresizable , &resizable ,
			NULL);

		if (cw->row.homogenous)
		{
			chwidth = rawwidth / chnum;
		}
		else
		{
			if (resizable)
			{
				XtQueryGeometry(*child , &inten_geom , &pref_geom);
				chwidth = (int) pref_geom.width * psize;
				chwidth = chwidth ? chwidth : 4;
			}
		}

		switch (gravitation)
		{
			case XtCright:
				rposition -= rspace + chwidth;
				XtConfigureWidget(*child , rposition ,
					y + cw->row.spacing , chwidth ,
					height - 2 * cw->row.spacing , bwidth);

				rposition -= lspace;
				break;
			case XtCleft:
			default:
				lposition += lspace;
				XtConfigureWidget(*child , lposition ,
					y + cw->row.spacing , chwidth ,
					height - 2 * cw->row.spacing , bwidth);

				lposition += chwidth + rspace;
		}
	}
}

static Widget TraverseInside(Widget w , int where , Time *time)
{
	MwBaseConstWidget cw = (MwBaseConstWidget) w;
	int gravitation,fgrav;
	int i;

	switch (where)
	{
	    case MW_TRAVERSE_FIRST:
		cw->baseConst.current_focused = NULL;
		for(i = 0  ; i < cw->composite.num_children ; i++)
		{
			XtVaGetValues(cw->composite.children[i] , 
				XtNgravitation , &gravitation , NULL);

			if (gravitation == XtCleft)
			{
			    	MwSetTraverseDirection(cw->composite.children[i] , where);
				if (XtCallAcceptFocus(cw->composite.children[i] , time))
				{
					cw->baseConst.current_focused = 
						cw->composite.children[i];
					break;
				}
			}
		}
		if (!cw->baseConst.current_focused)
		{
		    for(i = cw->composite.num_children - 1 ; i >= 0 ; i--)
		    {
			XtVaGetValues(cw->composite.children[i] , 
				XtNgravitation , &gravitation , NULL);

			if (gravitation == XtCright)
			{
				MwSetTraverseDirection(cw->composite.children[i] , where);
				if (XtCallAcceptFocus(cw->composite.children[i] , time))
				{
					cw->baseConst.current_focused = 
						cw->composite.children[i];
					break;
				}
			}
		    }			
		}
		break;
	    case MW_TRAVERSE_LAST:
		cw->baseConst.current_focused = NULL;
		for(i = 0 ; i < cw->composite.num_children ; i++)
		{
			XtVaGetValues(cw->composite.children[i] , 
				XtNgravitation , &gravitation , NULL);

			if (gravitation == XtCright)
			{
				MwSetTraverseDirection(cw->composite.children[i] , where);
				if (XtCallAcceptFocus(cw->composite.children[i] , time))
				{
					cw->baseConst.current_focused = 
						cw->composite.children[i];
					break;
				}
			}
		}
		if (!cw->baseConst.current_focused)
		{
		    for(i = cw->composite.num_children - 1; i >= 0 ; i--)
		    {
			XtVaGetValues(cw->composite.children[i] , 
				XtNgravitation , &gravitation , NULL);

			if (gravitation == XtCleft)
			{
			    	MwSetTraverseDirection(cw->composite.children[i] , where);
				if (XtCallAcceptFocus(cw->composite.children[i] , time))
				{
					cw->baseConst.current_focused = 
						cw->composite.children[i];
					break;
				}
			}
		    }
		}
		break;
	    case MW_TRAVERSE_NEXT:
		if (cw->composite.num_children)
		{
			for(i = 0 ;  i < cw->composite.num_children &&
				cw->baseConst.current_focused != 
				cw->composite.children[i]  ; i++);

			if (i < cw->composite.num_children &&
				cw->baseConst.current_focused)
			{
				XtVaGetValues(cw->composite.children[i] , 
					XtNgravitation , &fgrav , NULL);

				cw->baseConst.current_focused = NULL;

				if (fgrav == XtCleft)
				{
				    for ( i = i + 1 ; i < cw->composite.num_children ; i++)
				    {
					XtVaGetValues(cw->composite.children[i] , 
						XtNgravitation , &gravitation , NULL);
					if (gravitation == fgrav)
					{
					    	MwSetTraverseDirection(cw->composite.children[i] , where);
						if (XtCallAcceptFocus(cw->composite.children[i] , time))
						{
							cw->baseConst.current_focused = 
								cw->composite.children[i];
							break;
						}
					}
				    }
				}
				else
				{
				    for ( i = i - 1 ; i >= 0 ; i--)
				    {
					XtVaGetValues(cw->composite.children[i] , 
						XtNgravitation , &gravitation , NULL);
					if (gravitation == fgrav)
					{
					    	MwSetTraverseDirection(cw->composite.children[i] , where);
						if (XtCallAcceptFocus(cw->composite.children[i] , time))
						{
							cw->baseConst.current_focused = 
								cw->composite.children[i];
							break;
						}
					}
				    }
				    
				}
			    	if (!cw->baseConst.current_focused && fgrav == XtCleft)
			    	{
			    	    	for(i = cw->composite.num_children - 1 ; i >= 0 ; i--)
			    	    	{
						XtVaGetValues(cw->composite.children[i] , 
							XtNgravitation , &gravitation , NULL);

						if (gravitation != fgrav)
						{
					    	    MwSetTraverseDirection(cw->composite.children[i] , where);
						    if (XtCallAcceptFocus(cw->composite.children[i] , time))
						    {
							cw->baseConst.current_focused = 
								cw->composite.children[i];
							break;
						    }						
						}
			    	    	}
				}
			}
			else
			{
				cw->baseConst.current_focused = NULL;
				for(i = 0  ; i < cw->composite.num_children ; i++)
				{
					XtVaGetValues(cw->composite.children[i] , 
						XtNgravitation , &gravitation , NULL);

					if (gravitation == XtCleft)
					{
			    			MwSetTraverseDirection(cw->composite.children[i] , where);
						if (XtCallAcceptFocus(cw->composite.children[i] , time))
						{
							cw->baseConst.current_focused = 
								cw->composite.children[i];
							break;
						}
					}
				}
				if (!cw->baseConst.current_focused)
				{
				    for(i = cw->composite.num_children - 1 ; i >= 0 ; i--)
				    {
					XtVaGetValues(cw->composite.children[i] , 
						XtNgravitation , &gravitation , NULL);

					if (gravitation == XtCright)
					{
						MwSetTraverseDirection(cw->composite.children[i] , where);
						if (XtCallAcceptFocus(cw->composite.children[i] , time))
						{
							cw->baseConst.current_focused = 
								cw->composite.children[i];
							break;
						}
					}
				    }			
				}
			}
	    	}
		break;
	    case MW_TRAVERSE_PREV:
		if (cw->composite.num_children)
		{
			for(i = 0 ;  i < cw->composite.num_children &&
				cw->baseConst.current_focused != 
				cw->composite.children[i]  ; i++);

			if (i < cw->composite.num_children &&
				cw->baseConst.current_focused)
			{
				XtVaGetValues(cw->composite.children[i] , 
					XtNgravitation , &fgrav , NULL);

				cw->baseConst.current_focused = NULL;

				if (fgrav == XtCleft)
				{
				    cw->baseConst.current_focused = NULL;
				    for( i = i - 1 ; i >= 0 ; i--)
				    {
					XtVaGetValues(cw->composite.children[i] , 
						XtNgravitation , &gravitation , NULL);
					if (fgrav == gravitation)
					{
					    	MwSetTraverseDirection(cw->composite.children[i] , where);
						if (XtCallAcceptFocus(cw->composite.children[i] , time))
						{
							cw->baseConst.current_focused = 
								cw->composite.children[i];
							break;
						}
					}
				    }
				}
				else
				{
				    cw->baseConst.current_focused = NULL;
				    for( i = i + 1 ; i < cw->composite.num_children ; i++)
				    {
					XtVaGetValues(cw->composite.children[i] , 
						XtNgravitation , &gravitation , NULL);
					if (fgrav == gravitation)
					{
					    	MwSetTraverseDirection(cw->composite.children[i] , where);
						if (XtCallAcceptFocus(cw->composite.children[i] , time))
						{
							cw->baseConst.current_focused = 
								cw->composite.children[i];
							break;
						}
					}
				    }
				}
			    	if (!cw->baseConst.current_focused && fgrav == XtCright)
			    	{
					for(i = cw->composite.num_children - 1 ; i >= 0 ; i--)
					{
						XtVaGetValues(cw->composite.children[i] , 
							XtNgravitation , &gravitation , NULL);

						if (gravitation != fgrav)
						{
							MwSetTraverseDirection(cw->composite.children[i] , where);
							if (XtCallAcceptFocus(cw->composite.children[i] , time))
							{
								cw->baseConst.current_focused = 
									cw->composite.children[i];
								break;
							}
						}
					}
			    		
				}
			}
			else
			{
				for(i = 0 ; i < cw->composite.num_children ; i++)
				{
					XtVaGetValues(cw->composite.children[i] , 
					XtNgravitation , &gravitation , NULL);

					if (gravitation == XtCright)
					{
						MwSetTraverseDirection(cw->composite.children[i] , where);
						if (XtCallAcceptFocus(cw->composite.children[i] , time))
						{
							cw->baseConst.current_focused = 
								cw->composite.children[i];
							break;
						}
					}
				}
				if (!cw->baseConst.current_focused)
				{
				    for(i = cw->composite.num_children - 1 ; i >= 0 ; i--)
				    {
					XtVaGetValues(cw->composite.children[i] , 
						XtNgravitation , &gravitation , NULL);

					if (gravitation == XtCleft)
					{
					    	MwSetTraverseDirection(cw->composite.children[i] , where);
						if (XtCallAcceptFocus(cw->composite.children[i] , time))
						{
							cw->baseConst.current_focused = 
								cw->composite.children[i];
							break;
						}
					}
				    }
				}

			}
		}
		break;
	    case MW_TRAVERSE_ACTUAL:
		if (cw->composite.num_children)
		{
			for(i = 0 ;  i < cw->composite.num_children &&
				cw->baseConst.current_focused != 
				cw->composite.children[i]  ; i++);

			if (i < cw->composite.num_children &&
				cw->baseConst.current_focused)
			{
			    	MwSetTraverseDirection(cw->composite.children[i] , where);
				XtCallAcceptFocus(cw->composite.children[i] , time);
			}
		}
		else
		{
			cw->baseConst.current_focused = NULL;		    	
		}
		break;
	    default: XtWarning("Unknown direction");
	}

	return cw->baseConst.current_focused;		
}
