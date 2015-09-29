
/*
 * MwTooltipP.h - Private Header file for MwTooltip widget.
 */

#ifndef s_MwTooltipP_h
#define s_MwTooltipP_h

#include "MwTooltip.h"
#include <X11/ShellP.h>

typedef struct {
    XtPointer extension;		/* For future needs. */
} MwTooltipClassPart;

typedef struct s_MwTooltipClassRec {
  CoreClassPart	          core_class;
  CompositeClassPart      composite_class;
  ShellClassPart          shell_class;
  OverrideShellClassPart  override_shell_class;
  MwTooltipClassPart	  tooltip_class;
} MwTooltipClassRec;

extern MwTooltipClassRec mwTooltipClassRec;

typedef struct p_list {
	Widget w;
	char *p;
	struct p_list *next;
} p_list;

typedef struct s_MwTooltipPart {

  /* resources */

  Widget label;			/* mode 1 => display tip in this label */
  int mode;			/* 0 = don't display
				   1 = display in label
				   2 = display in popup
				   3 = display in both */
  int interval;

  /* private state */

  Widget plabel;		/* to display the tip */
  XtIntervalId timer;		/* used waiting to popup */
  p_list *plist;		/* list of widgets and tips */

} MwTooltipPart;

typedef struct s_MwTooltipRec {
  CorePart		core;
  CompositePart 	composite;
  ShellPart 	        shell;
  OverrideShellPart     override;
  MwTooltipPart		tooltip;
} MwTooltipRec;

#endif /* s_MwTooltipP_h */

