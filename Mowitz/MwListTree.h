/*-----------------------------------------------------------------------------
** MwListTree.c	A Specialized List widget
**
** Public header file
**
** Copyright (c) 1995 Robert W. McMullen
**
** Permission to use, copy, modify, distribute, and sell this software and its
** documentation for any purpose is hereby granted without fee, provided that
** the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  The author makes no representations about the suitability
** of this software for any purpose.  It is provided "as is" without express
** or implied warranty.
**
** THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
** ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.  IN NO EVENT SHALL
** THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
** ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
** WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
** ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
** SOFTWARE.
*/

#ifndef _MwListTree_H
#define _MwListTree_H

#include <X11/Core.h>

#define _MwListTree_WIDGET_VERSION	2.0

#define XtNmargin		"margin"
#define XtNindent		"indent"
#define XtNhorizontalSpacing	"horizontalSpacing"
#define XtNverticalSpacing	"verticalSpacing"
#define XtNlineWidth		"lineWidth"
#define XtNbranchPixmap		"branchPixmap"
#define XtNbranchOpenPixmap	"branchOpenPixmap"
#define XtNleafPixmap		"leafPixmap"
#define XtNleafOpenPixmap	"leafOpenPixmap"
#define XtNbranchCallback	"branchCallback"
#define XtNleafCallback		"leafCallback"
#define XtNpathCallback		"pathCallback"
#define XtNhighlightCallback	"highlightCallback"
#define XtNactivateCallback	"activateCallback"

#define XtBRANCH	1
#define XtLEAF		2

extern WidgetClass listtreeWidgetClass;

typedef struct _MwListTreeClassRec *MwListTreeWidgetClass;
typedef struct _MwListTreeRec      *MwListTreeWidget;

typedef struct _MwListTreeItem {
	Boolean		open;
	Boolean		highlighted;
	char		*text;
	int		length;
	int		x,y,ytext;
	Dimension	height;
	struct _MwListTreeItem 	*parent,
			*firstchild,
			*prevsibling,*nextsibling;
	XtPointer	user_data;
} MwListTreeItem;

typedef struct _MwListTreeReturnStruct {
	int		reason;
	MwListTreeItem	*item;
	MwListTreeItem	**path;
	int		count;
	Boolean		open;
} MwListTreeReturnStruct;

typedef struct _MwListTreeMultiReturnStruct {
	MwListTreeItem	**items;
	int		count;
} MwListTreeMultiReturnStruct;

typedef struct _MwListTreeActivateStruct {
	int		reason;
	MwListTreeItem	*item;
	Boolean		open;
	MwListTreeItem	**path;
	int		count;
} MwListTreeActivateStruct;

/*
** Public function declarations
*/
#ifndef _MwListTree_
#if __STDC__ || defined(__cplusplus)
#define P_(s) s
#else
#define P_(s) ()
#endif

/* MwListTree.c */
void MwListTreeRefresh P_((Widget w));
void MwListTreeRefreshOff P_((Widget w));
void MwListTreeRefreshOn P_((Widget w));
MwListTreeItem *MwListTreeAdd P_((Widget w, MwListTreeItem *parent, char *string));
void MwListTreeRenameItem P_((Widget w, MwListTreeItem *item, char *string));
int MwListTreeDelete P_((Widget w, MwListTreeItem *item));
int MwListTreeDeleteChildren P_((Widget w, MwListTreeItem *item));
int MwListTreeReparent P_((Widget w, MwListTreeItem *item, MwListTreeItem *newparent));
int MwListTreeReparentChildren P_((Widget w, MwListTreeItem *item, MwListTreeItem *newparent));
int MwListTreeOrderSiblings P_((Widget w, MwListTreeItem *item));
int MwListTreeOrderChildren P_((Widget w, MwListTreeItem *item));
MwListTreeItem *MwListTreeFindSiblingName P_((Widget w, MwListTreeItem *item, char *name));
MwListTreeItem *MwListTreeFindChildName P_((Widget w, MwListTreeItem *item, char *name));
void MwListTreeHighlightItem P_((Widget w, MwListTreeItem *item));
MwListTreeItem *MwListTreeFirstItem P_((Widget w));
#ifdef USE_RDD
void MwListTreeHighlightDrop P_((Widget w, XEvent *event, String *params, Cardinal *num_params));
MwListTreeReturnStruct *MwListTreeGetDrop P_((Widget w));
#endif
extern void MwListTreeSetHighlighted(Widget,MwListTreeItem **,int,Boolean);
extern void MwListTreeGetHighlighted(Widget,MwListTreeMultiReturnStruct *);

#undef P_
#endif

#endif /* _MwListTree_H */
