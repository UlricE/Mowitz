
#ifndef s_MwTooltip_h
#define s_MwTooltip_h

#include <X11/Shell.h>
#include <X11/Xmu/Converters.h>

/****************************************************************
 *
 * MwTooltip widget
 *
 ****************************************************************/

/* MwTooltip Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		XtDefaultBackground
 backgroundPixmap    BackgroundPixmap	Pixmap          None
 borderColor	     BorderColor	Pixel		XtDefaultForeground
 borderPixmap	     BorderPixmap	Pixmap		None
 borderWidth	     BorderWidth	Dimension	1
 bottomMargin        VerticalMargins    Dimension       VerticalSpace
 columnWidth         ColumnWidth        Dimension       Width of widest text
 cursor              Cursor             Cursor          None
 destroyCallback     Callback		Pointer		NULL
 height		     Height		Dimension	0
 label               Label              String          NULL (No label)
 labelClass          LabelClass         Pointer         smeBSBObjectClass
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 rowHeight           RowHeight          Dimension       Height of Font
 sensitive	     Sensitive		Boolean		True
 topMargin           VerticalMargins    Dimension       VerticalSpace
 width		     Width		Dimension	0
 x		     Position		Position	0n
 y		     Position		Position	0

*/

typedef struct s_MwTooltipClassRec*	MwTooltipWidgetClass;
typedef struct s_MwTooltipRec*		MwTooltipWidget;

extern WidgetClass mwTooltipWidgetClass;

#define XtNtooltipLabel "tooltipLabel"
#define XtCTooltipLabel "TooltipLabel"
#define XtNtooltipMode "tooltipMode"
#define XtCTooltipMode "TooltipMode"
#define XtNtooltipInterval "tooltipInterval"
#define XtCTooltipInterval "TooltipInterval"

#ifndef TOOLTIP_NONE
#define TOOLTIP_NONE (0)
#define TOOLTIP_LABEL (1)
#define TOOLTIP_POPUP (2)
#define TOOLTIP_BOTH (3)
#endif

/************************************************************
 *
 * Public Functions.
 *
 ************************************************************/

extern void MwTooltipAdd(Widget, Widget, char *);
extern char *MwTooltipGet(Widget, Widget);
extern void MwTooltipRemove(Widget, Widget);

#endif /* s_MwTooltip_h */
