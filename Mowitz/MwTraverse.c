/***************************************************************************/
/* 	This code is part of X-toolkit widget library called Nws 	   */
/*	Copyright (c) 1997,1998,1999 Ondrejicka Stefan			   */
/*	(ondrej@idata.sk)						   */
/*	Distributed under GPL 2 or later				   */
/***************************************************************************/

#include <X11/IntrinsicP.h>
#include "MwTraverse.h"
#include "MwBaseConstP.h"
#include "MwBaseCompP.h"

void MwSetTraverseDirection(w , where)
Widget w;
int where;
{
	if (XtIsSubclass(w , mwBaseCompWidgetClass))
		((MwBaseCompWidget)w)->baseComp.traverse_direction = where;
	else if (XtIsSubclass(w , mwBaseConstWidgetClass))
		((MwBaseConstWidget)w)->baseConst.traverse_direction = where;
}
