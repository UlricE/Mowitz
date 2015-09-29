/*-----------------------------------------------------------------------------
** TreeFSel.c	A File Selector widget using the ListTree widget
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

#ifndef _TreeFSel_H
#define _TreeFSel_H

#include <X11/Core.h>

#include <Mowitz/MwListTree.h>

#define _TreeFSel_WIDGET_VERSION	1.00

#define XtNspacing		"spacing"
#define XtNdirPixmap		"dirPixmap"
#define XtNdirOpenPixmap	"dirOpenPixmap"

extern WidgetClass treefselWidgetClass;

typedef struct _TreeFSelClassRec *TreeFSelWidgetClass;
typedef struct _TreeFSelRec      *TreeFSelWidget;

typedef struct _TreeFSelReturnStruct {
	char	*pathname;
	int	button;
} TreeFSelReturnStruct;

/*
** Public function declarations
*/

#endif /* _TreeFSel_H */
