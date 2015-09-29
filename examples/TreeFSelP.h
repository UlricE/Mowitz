/*-----------------------------------------------------------------------------
** TreeFSel.c	A File Selector widget using the ListTree widget
**
** Private header file
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

#ifndef _TreeFSelP_H
#define _TreeFSelP_H

#include <X11/Core.h>

#include "Directory.h"
#include "TreeFSel.h"

typedef struct {
	int		dummy;		/* keep compiler happy with dummy field */
} TreeFSelClassPart;

typedef struct _TreeFSelClassRec {
	CoreClassPart		core_class;
	CompositeClassPart	composite_class;
	TreeFSelClassPart	TreeFSel_class;
} TreeFSelClassRec;

extern TreeFSelClassRec treefselClassRec;

typedef struct {
	/* Public stuff ... */
	long		foreground_pixel;
	XFontStruct	*font;
	int		NumItems;
	Dimension	Spacing;
	Dimension	LabelSpacing;
	Dimension	Margin;
	Dimension	Indent;
        Dimension       LineWidth;
	XtCallbackList  Callback;
	Pixmap		Dir;
	Pixmap		DirOpen;

	/* Private stuff ... */
	GC		drawGC;
	Widget		toplevel,
			mainpane,
			paned,
			form,
			tree,
			list,
			showdir,
			box1,box2,
			sel,
			okbtn,cancelbtn;
	char 		**files;
	char		*filebuf;
	int 		files_alloc;
	int		filecount;
	char		CurrentDir[MAXNAMLEN*2],
			WorkingDir[MAXNAMLEN*2],
			SelectedFile[MAXNAMLEN];
} TreeFSelPart;

typedef struct _TreeFSelRec {
	CorePart	core;
	CompositePart	composite;
	TreeFSelPart	fsel;
} TreeFSelRec;


#endif /* _TreeFSelP_H */
