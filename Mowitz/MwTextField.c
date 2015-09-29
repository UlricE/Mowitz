/*-----------------------------------------------------------------------------
 * MwTextField	A single line text entry widget
 *
 * Copyright (c) 1995 Robert W. McMullen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * Author: Rob McMullen <rwmcm@orion.ae.utexas.edu>
 *         http://www.ae.utexas.edu/~rwmcm
 */

#define _MwTextField_

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Xmu/Xmu.h>

#include <stdio.h>

#include <stdlib.h>

#include "MwUtils.h"
#include "MwTextFieldP.h"

#define offset(field) XtOffsetOf(MwTextFieldRec, text.field)
static XtResource resources[] =
{
  {XtNallowSelection, XtCBoolean, XtRBoolean, sizeof(Boolean),
    offset(AllowSelection), XtRString, "True"},
  {XtNdisplayCaret, XtCBoolean, XtRBoolean, sizeof(Boolean),
    offset(DisplayCursor), XtRString, "True"},
  {XtNecho, XtCBoolean, XtRBoolean, sizeof(Boolean),
    offset(Echo), XtRString, "True"},
  {XtNeditable, XtCBoolean, XtRBoolean, sizeof(Boolean),
    offset(Editable), XtRString, "True"},
  {XtNfont, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
    offset(font), XtRString, XtDefaultFont},
  {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
    offset(foreground_pixel), XtRString, XtDefaultForeground},
  {XtNinsertPosition, XtCInsertPosition, XtRInt, sizeof(int),
    offset(CursorPos), XtRString, "0"},
  {XtNlength, XtCLength, XtRInt, sizeof(int),
    offset(TextMaxLen), XtRString, "0"},
  {XtNmargin, XtCMargin, XtRDimension, sizeof(Dimension),
    offset(Margin), XtRString, "3"},
  {XtNpendingDelete, XtCBoolean, XtRBoolean, sizeof(Boolean),
    offset(PendingDelete), XtRString, "True"},
  {XtNstring, XtCString, XtRString, sizeof(char *),
    offset(DefaultString), XtRString, NULL},
  {XtNactivateCallback, XtCCallback, XtRCallback, sizeof(XtPointer),
    offset(ActivateCallback), XtRCallback, NULL},
};

#undef offset

static void Initialize();
static void Realize(Widget, XtValueMask *, XSetWindowAttributes *);
static void Destroy();
static void Redisplay();
static void Resize();
static Boolean SetValues();
static void Draw(), DrawInsert(), MassiveChangeDraw(), DrawTextReposition(),
  ClearHighlight(), DrawHighlight(), DrawCursor(), EraseCursor();
static Boolean PositionCursor(), MassiveCursorAdjust();
static void Nothing(), Activate(), InsertChar(), ForwardChar(),
  ForwardToEnd(), BackwardChar(), BackwardToStart(), DeleteNext(),
  DeleteToEnd(), DeletePrev(), DeleteToStart(), DeleteHighlighted(),
  TransposeChars(), SelectStart(), ExtendStart(), ExtendAdjust(),
  ExtendEnd(), InsertSelection();

static char defaultTranslations[] =
"<Key>Right:	forward-char()\n\
 :<Key>KP_Right:	forward-char()\n\
 Ctrl<Key>f:	forward-char()\n\
 Ctrl<Key>e:	forward-to-end()\n\
 <Key>Left:	backward-char()\n\
 :<Key>KP_Left:	backward-char()\n\
 Ctrl<Key>b:	backward-char()\n\
 Ctrl<Key>a:	backward-to-start()\n\
 Ctrl<Key>d:	delete-next-char()\n\
 Ctrl<Key>k:	delete-to-end()\n\
 <Key>Delete:	delete-previous-char()\n\
 <Key>BackSpace:	delete-previous-char()\n\
 Ctrl<Key>h:	delete-previous-char()\n\
 Ctrl<Key>u:	delete-to-start()\n\
 Ctrl<Key>w:	delete-highlighted()\n\
 Ctrl<Key>t:	transpose-chars()\n\
 <Key>Return:	activate()\n\
 <Key>KP_Enter:	activate()\n\
 <Key>:		insert-char()\n\
 Shift<Btn1Down>:	extend-start()\n\
 <Btn1Down>:	select-start()\n\
 <Btn1Motion>:	extend-adjust()\n\
 <Btn1Up>:	extend-end()\n\
 <Btn2Down>:	insert-selection()\n\
 <Btn3Down>:	extend-start()\n\
 <Btn3Motion>:	extend-adjust()\n\
 <Btn3Up>:	extend-end()\n\
 <EnterWindow>:	enter-window()\n\
 <LeaveWindow>:	leave-window()\n\
 <FocusIn>:	focus-in()\n\
 <FocusOut>:	focus-out()";

static XtActionsRec actions[] =
{
  {"insert-char", InsertChar},
  {"forward-char", ForwardChar},
  {"forward-to-end", ForwardToEnd},
  {"backward-char", BackwardChar},
  {"backward-to-start", BackwardToStart},
  {"delete-next-char", DeleteNext},
  {"delete-to-end", DeleteToEnd},
  {"delete-previous-char", DeletePrev},
  {"delete-to-start", DeleteToStart},
  {"delete-highlighted", DeleteHighlighted},
  {"transpose-chars", TransposeChars},
  {"activate", Activate},
  {"select-start", SelectStart},
  {"extend-start", ExtendStart},
  {"extend-adjust", ExtendAdjust},
  {"extend-end", ExtendEnd},
  {"insert-selection", InsertSelection},
  {"enter-window", Nothing},
  {"leave-window", Nothing},
  {"focus-in", Nothing},
  {"focus-out", Nothing},
};

MwTextFieldClassRec mwTextfieldClassRec =
{
  {
	/* core_class fields     */
	/* superclass            */ (WidgetClass) & widgetClassRec,
	/* class_name            */ "MwTextField",
	/* widget_size           */ sizeof(MwTextFieldRec),
	/* class_initialize      */ NULL,
	/* class_part_initialize */ NULL,
	/* class_inited          */ False,
	/* initialize            */ Initialize,
	/* initialize_hook       */ NULL,
	/* realize               */ /*XtInherit*/Realize,
	/* actions               */ actions,
	/* num_actions           */ XtNumber(actions),
	/* resources             */ resources,
	/* num_resources         */ XtNumber(resources),
	/* xrm_class             */ NULLQUARK,
	/* compress_motion       */ True,
	/* compress_exposure     */ XtExposeCompressMultiple,
	/* compress_enterleave   */ True,
	/* visible_interest      */ True,
	/* destroy               */ Destroy,
	/* resize                */ Resize,
	/* expose                */ Redisplay,
	/* set_values            */ SetValues,
	/* set_values_hook       */ NULL,
	/* set_values_almost     */ XtInheritSetValuesAlmost,
	/* get_values_hook       */ NULL,
	/* accept_focus          */ NULL,
	/* version               */ XtVersion,
	/* callback_private      */ NULL,
	/* tm_table              */ defaultTranslations,
	/* query_geometry        */ XtInheritQueryGeometry,
	/* display_accelerator   */ XtInheritDisplayAccelerator,
	/* extension             */ NULL
  },
  {
    0				/* some stupid compilers barf on empty structures */
  }
};

WidgetClass mwTextfieldWidgetClass = (WidgetClass) & mwTextfieldClassRec;

/* Convenience macros */
#define TopMargin(w) (int)(w->text.Margin - 1)
#define BottomMargin(w) (int)(w->text.Margin)

/* Font functions */
#define FontHeight(f)  (int)(f->max_bounds.ascent + f->max_bounds.descent)
#define FontDescent(f) (int)(f->max_bounds.descent)
#define FontAscent(f)  (int)(f->max_bounds.ascent)
#define FontTextWidth(f,c,l) (int)XTextWidth(f, c, l)

static void
InitializeGC(MwTextFieldWidget w)
{
  static char dots[] =
  {2, 1, 1};
  XGCValues values;
  XtGCMask mask;

  values.line_style = LineSolid;
  values.line_width = 0;
  values.fill_style = FillSolid;
  values.font = w->text.font->fid;
  values.background = w->core.background_pixel;
  values.foreground = w->text.foreground_pixel;
  mask = GCLineStyle | GCLineWidth | GCFillStyle | GCForeground | GCBackground | GCFont;
  w->text.drawGC = XtGetGC((Widget) w, mask, &values);

  values.foreground = w->core.background_pixel;
  values.background = w->text.foreground_pixel;
  w->text.highlightGC = XtGetGC((Widget) w, mask, &values);

  values.line_style = LineSolid;
  values.line_width = 0;
  values.background = w->core.background_pixel;
  values.foreground = w->text.foreground_pixel;
  mask = GCLineStyle | GCLineWidth | GCForeground | GCBackground;
  w->text.cursorGC = XtGetGC((Widget) w, mask, &values);

  values.foreground = w->core.background_pixel;
  values.background = w->text.foreground_pixel;
  w->text.eraseGC = XtGetGC((Widget) w, mask, &values);

  values.line_style = LineOnOffDash;
  values.background = w->core.background_pixel;
  values.foreground = w->text.foreground_pixel;
  w->text.dashGC = XtGetGC((Widget) w, mask, &values);
  XSetDashes(XtDisplay(w), w->text.dashGC, 0, &dots[1],
    (int) dots[0]);

  w->text.YOffset = TopMargin(w) + FontAscent(w->text.font);
}

static void
ClipGC(MwTextFieldWidget w)
{
  XRectangle clip;

  clip.x = 0;
  clip.y = w->text.YOffset - FontAscent(w->text.font) + 1;
  clip.width = w->text.ViewWidth + 1;
  clip.height = FontHeight(w->text.font);
  XSetClipRectangles(XtDisplay((Widget) w), w->text.drawGC,
    w->text.Margin, 0, &clip, 1, Unsorted);
  XSetClipRectangles(XtDisplay((Widget) w), w->text.highlightGC,
    w->text.Margin, 0, &clip, 1, Unsorted);
}

static void
SetString(MwTextFieldWidget w, char *s)
{
  int len;

  if (s) {
    len = strlen(s);
    if (len > w->text.TextAlloc) {
      w->text.TextAlloc += len;
      w->text.Text = MwRealloc(w->text.Text, w->text.TextAlloc);
    }
    strcpy(w->text.Text, s);
    w->text.TextLen = len;
    w->text.TextWidth = w->text.OldTextWidth =
      FontTextWidth(w->text.font, w->text.Text,
      w->text.TextLen);
    if ((w->text.TextMaxLen > 0) && (w->text.TextLen > w->text.TextMaxLen))
      w->text.TextMaxLen = w->text.TextLen;
  }
  w->text.DefaultString = w->text.Text;
}

#define superclass (&coreClassRec)
static void Realize(Widget w, XtValueMask *valueMask,
		XSetWindowAttributes *attributes)
{
	MwTextFieldWidget rtw = (MwTextFieldWidget)w;
	Display *dpy;
	char *name, *class;
	XIMStyles *styles;
	int i;

	(*superclass->core_class.realize)(w, valueMask, attributes);

#ifdef HAVE_XCREATEIC
	/* Set up input methods */
	dpy = XtDisplay(w);
	XtGetApplicationNameAndClass(dpy, &name, &class);
	rtw->text.xim = XOpenIM(dpy, XtDatabase(dpy), name, class);
	if (rtw->text.xim) {
		XGetIMValues(rtw->text.xim,
			XNQueryInputStyle, &styles,
			(char *)0);
		for (i = 0; i < styles->count_styles; i++) {
			if (styles->supported_styles[i] ==
				(XIMPreeditNothing | XIMStatusNothing)) break;
		}
		if (i == styles->count_styles) i = 0;
		rtw->text.xic = XCreateIC(rtw->text.xim,
			XNInputStyle, styles->supported_styles[i],
			XNClientWindow, XtWindow(w),
			(char *)0);
	}
#endif
}

static void
Initialize(Widget treq, Widget tnew, ArgList args, Cardinal * num)
{
  MwTextFieldWidget new;
  int height;

  new = (MwTextFieldWidget) tnew;

  new->text.timer_id = (XtIntervalId) 0;
  new->text.multi_click_time = XtGetMultiClickTime(XtDisplay((Widget) new));
  new->text.highlight_time = new->text.multi_click_time / 2;

  if (new->text.TextMaxLen > 0) {
    new->text.TextAlloc = new->text.TextMaxLen + 1;
  }
  else {
    new->text.TextAlloc = TEXTFIELD_ALLOC_SIZE;
  }
  new->text.Text = (char *) MwMalloc(new->text.TextAlloc);
  new->text.TextLen = 0;
  new->text.SelectionText = NULL;
  new->text.TextWidth = new->text.OldTextWidth = 0;
  if (new->text.DefaultString)
    SetString(new, new->text.DefaultString);

  if (new->text.CursorPos > 0) {
    if (new->text.CursorPos > new->text.TextLen) {
      new->text.CursorPos = new->text.TextLen;
    }
  }
  else {
    new->text.CursorPos = 0;
  }
  new->text.OldCursorX = -1;
  new->text.HighlightStart = new->text.HighlightEnd = -1;
  new->text.OldHighlightStart = new->text.OldHighlightEnd = -1;

  height = FontHeight(new->text.font);
  if (new->core.height == 0)
    new->core.height = (Dimension) height + TopMargin(new) + BottomMargin(new);

  if (new->core.width == 0) {
    new->text.ViewWidth = 200;
    new->core.width = new->text.ViewWidth + 2 * new->text.Margin;
  }
  else {
    int width;

    width = (int) new->core.width - 2 * new->text.Margin;
    if (width < 0)
      new->text.ViewWidth = new->core.width;
    else
      new->text.ViewWidth = width;
  }

  new->text.XOffset = new->text.OldXOffset = 0;

  InitializeGC(new);

  ClipGC(new);
}

static void
Destroy(MwTextFieldWidget w)
{
  XtReleaseGC((Widget) w, w->text.drawGC);
  XtReleaseGC((Widget) w, w->text.highlightGC);
  if (w->text.SelectionText)
    MwFree(w->text.SelectionText);
  MwFree(w->text.Text);
#ifdef HAVE_XCREATEIC
  if (w->text.xim) {
    XDestroyIC(w->text.xic);
    XCloseIM(w->text.xim);
  }    
#endif
}

static void
Redisplay(Widget aw, XExposeEvent * event, Region region)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;

  if (!XtIsRealized(aw))
    return;

  Draw(w);
}

static Boolean
SetValues(Widget current, Widget request, Widget reply,
  ArgList args, Cardinal * nargs)
{
  MwTextFieldWidget w = (MwTextFieldWidget) current;
  MwTextFieldWidget new = (MwTextFieldWidget) reply;
  Boolean redraw = False;

  if ((w->text.foreground_pixel != new->text.foreground_pixel) ||
    (w->core.background_pixel != new->core.background_pixel) ||
    (w->text.font != new->text.font)) {
    XtReleaseGC((Widget) w, w->text.drawGC);
    XtReleaseGC((Widget) w, w->text.highlightGC);
    XtReleaseGC((Widget) w, w->text.cursorGC);
    XtReleaseGC((Widget) w, w->text.eraseGC);
    XtReleaseGC((Widget) w, w->text.dashGC);
    InitializeGC(new);
    redraw = True;
  }

  if ((w->text.CursorPos != new->text.CursorPos) ||
    (w->text.DisplayCursor != new->text.DisplayCursor)) {
    redraw = True;
  }
  if (w->text.DefaultString != new->text.DefaultString) {
    redraw = True;
    SetString(new, new->text.DefaultString);
    new->text.HighlightStart = new->text.HighlightEnd = -1;
    new->text.CursorPos = new->text.TextLen;
#ifdef DEBUG_TF
    printf("SetValues: %s\n", new->text.DefaultString);
#endif
  }

  return redraw;
}


static void
Resize(Widget aw)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;
  int width, height;

  if (!XtIsRealized(aw))
    return;

  width = w->core.width - 2 * w->text.Margin;
  if (width < 0)
    w->text.ViewWidth = w->core.width;
  else
    w->text.ViewWidth = width;


  height = (((int) w->core.height - FontHeight(w->text.font)) / 2) +
    FontAscent(w->text.font);
  w->text.YOffset = height;

  ClipGC(w);

  MassiveChangeDraw(w);
}

static void
TextDelete(MwTextFieldWidget w, int start, int len)
{
  int i;

  if (len > 0) {
    for (i = start + len; i < w->text.TextLen; i++)
      w->text.Text[i - len] = w->text.Text[i];
    w->text.TextLen -= len;
    w->text.TextWidth = FontTextWidth(w->text.font, w->text.Text,
      w->text.TextLen);
    w->text.Text[w->text.TextLen] = 0;
  }
}

static void
TextDeleteHighlighted(MwTextFieldWidget w)
{
  if (w->text.HighlightStart >= 0) {
    TextDelete(w, w->text.HighlightStart,
      w->text.HighlightEnd - w->text.HighlightStart);
    w->text.CursorPos = w->text.HighlightStart;
    w->text.HighlightStart = w->text.HighlightEnd = -1;
  }
}


/* returns value indicating if the text can be redrawn using the fast
 * method */
static Boolean
TextInsert(MwTextFieldWidget w, char *buf, int len)
{
  int i;
  Boolean regular_copy, fast_insert;

  fast_insert = True;
  if (len > 0) {
    if (w->text.HighlightStart >= 0) {
      fast_insert = False;
      if (w->text.PendingDelete)
	TextDeleteHighlighted(w);
      else
	ClearHighlight(w);
    }

    regular_copy = True;
    if (w->text.TextMaxLen > 0) {
      if (w->text.TextLen + len > w->text.TextMaxLen)
	regular_copy = False;
    }
    else if (w->text.TextLen + len > w->text.TextAlloc) {
      i = TEXTFIELD_ALLOC_SIZE;
      if (i < len)
	i = len;
      w->text.TextAlloc += i + 1;
      w->text.Text = MwRealloc(w->text.Text, w->text.TextAlloc);
#ifdef DEBUG_TF
      printf("TextInsert: Alloced new space: %d bytes\n", w->text.TextAlloc);
#endif
    }
    if (regular_copy) {
      for (i = w->text.TextLen - 1; i >= w->text.CursorPos; i--)
	w->text.Text[i + len] = w->text.Text[i];
      strncpy(&w->text.Text[w->text.CursorPos], buf, len);
      w->text.FastInsertCursorStart = w->text.CursorPos;
      w->text.FastInsertTextLen = len;
      w->text.TextLen += len;
      w->text.CursorPos += len;
    }
    else {
      int i1;

      for (i = w->text.TextLen - 1; i >= w->text.CursorPos; i--)
	if (i + len < w->text.TextMaxLen)
	  w->text.Text[i + len] = w->text.Text[i];
      w->text.TextLen += len;
      if (w->text.TextLen > w->text.TextMaxLen)
	w->text.TextLen = w->text.TextMaxLen;
      i1 = w->text.CursorPos;
      for (i = 0; i < len; i++) {
	if (i1 < w->text.TextMaxLen)
	  w->text.Text[i1] = *buf++;
	else
	  break;
	i1++;
      }
      w->text.FastInsertCursorStart = w->text.CursorPos;
      w->text.FastInsertTextLen = i1 - w->text.CursorPos;
      w->text.CursorPos = i1;
    }
    w->text.TextWidth = FontTextWidth(w->text.font,
      w->text.Text, w->text.TextLen);
    w->text.Text[w->text.TextLen] = 0;
  }
  return fast_insert;
}

static int
TextPixelToPos(MwTextFieldWidget w, int x)
{
  int i, tot, cur, pos;

  pos = 0;

  x -= (int) w->text.Margin + w->text.XOffset;

/* check if the cursor is before the 1st character */
  if (x <= 0) {
    pos = 0;
  }

/* OK, how 'bout after the last character */
  else if (x > FontTextWidth(w->text.font, w->text.Text, w->text.TextLen)) {
    pos = w->text.TextLen;
  }

/* must be in between somewhere... */
  else {
    tot = 0;
    pos = -1;
    for (i = 0; i < w->text.TextLen; i++) {
      cur = FontTextWidth(w->text.font, &w->text.Text[i], 1);
      if (x < tot + (cur / 2)) {
	pos = i;
	break;
      }
      tot += cur;
    }
    if (pos < 0)
      pos = w->text.TextLen;
  }
  return pos;
}

/*
 * MwTextField Widget Action procedures
 */

/* ARGSUSED */
static void
Nothing(Widget aw, XEvent * event, String * params, Cardinal * num_params)
{
}

/* ARGSUSED */
static void
Activate(Widget aw, XEvent * event, String * params, Cardinal * num_params)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;
  MwTextFieldReturnStruct ret;

  ret.reason = 0;
  ret.event = event;
  ret.string = w->text.Text;

  if (XtNactivateCallback)
    XtCallCallbacks(aw, XtNactivateCallback, &ret);
}

/* ARGSUSED */
static void
ForwardChar(Widget aw, XEvent * event, String * params, Cardinal * num_params)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;

  if (!w->text.Editable)
    return;

  ClearHighlight(w);
  if (w->text.CursorPos < w->text.TextLen) {
    w->text.CursorPos++;
    EraseCursor(w);
    if (PositionCursor(w))
      DrawTextReposition(w);
    DrawCursor(w);
  }
}

/* ARGSUSED */
static void
ForwardToEnd(Widget aw, XEvent * event, String * params, Cardinal * num_params)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;

  if (!w->text.Editable)
    return;

  ClearHighlight(w);
  if (w->text.CursorPos < w->text.TextLen) {
    w->text.CursorPos = w->text.TextLen;
    EraseCursor(w);
    if (PositionCursor(w))
      DrawTextReposition(w);
    DrawCursor(w);
  }
}

/* ARGSUSED */
static void
BackwardChar(Widget aw, XEvent * event, String * params, Cardinal * num_params)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;

  if (!w->text.Editable)
    return;

  ClearHighlight(w);
  if (w->text.CursorPos > 0) {
    w->text.CursorPos--;
    EraseCursor(w);
    if (PositionCursor(w))
      DrawTextReposition(w);
    DrawCursor(w);
  }
}

/* ARGSUSED */
static void
BackwardToStart(Widget aw, XEvent * event, String * params, Cardinal * num_params)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;

  if (!w->text.Editable)
    return;

  ClearHighlight(w);
  if (w->text.CursorPos > 0) {
    w->text.CursorPos=0;
    EraseCursor(w);
    if (PositionCursor(w))
      DrawTextReposition(w);
    DrawCursor(w);
  }
}

/* ARGSUSED */
static void
InsertChar(Widget aw, XEvent * event, String * params, Cardinal * num_params)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;
  int len, i, j;
  Status status;
  KeySym keysym;

#define INSERTCHARBUFSIZ 32
  char buf[INSERTCHARBUFSIZ];

  if (!w->text.Editable)
    return;

#ifdef HAVE_XCREATEIC
	if (((MwTextFieldWidget)w)->text.xim)
		len = XmbLookupString(((MwTextFieldWidget)w)->text.xic,
					(XKeyEvent *)event, (char *)buf, BUFSIZ,
					&keysym, &status);
	else
		len = XLookupString((XKeyEvent *) event, buf, BUFSIZ, NULL, NULL);
#else
  len = XLookupString((XKeyEvent *) event, buf, BUFSIZ, NULL, NULL);
#endif

  /* Throw away characters with values < Space (32) */
/* Unbroken by Ulric: keep negative values */
/* Unbroken some more: keep tabs */
/* Even more: compare bits rather than integral values, to avoid
   bogus warnings because of signed/unsigned char. It's simpler, too. */
  for (i = 0; i < len; i++) {
	if (buf[i] != '\t' && !(buf[i] & ~31)) {
		for (j = 0; j < len && j < (BUFSIZ - 1); j++) {
			buf[j] = buf[j+1];
		}
		buf[j] = '\0';
		len-=1;
	}
  }
  if (len > 0) {
    EraseCursor(w);
    if (TextInsert(w, buf, len))
      DrawInsert(w);
    else
      Draw(w);
  }
}

/* ARGSUSED */
static void
DeleteNext(Widget aw, XEvent * event, String * params, Cardinal * num_params)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;

  if (!w->text.Editable)
    return;

  if (w->text.CursorPos < w->text.TextLen) {
    ClearHighlight(w);
    TextDelete(w, w->text.CursorPos, 1);
    Draw(w);
  }
}

/* ARGSUSED */
static void
DeleteToEnd(Widget aw, XEvent * event, String * params, Cardinal * num_params)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;

  if (!w->text.Editable)
    return;

  if (w->text.CursorPos < w->text.TextLen) {
    ClearHighlight(w);
    TextDelete(w, w->text.CursorPos, w->text.TextLen-w->text.CursorPos);
    Draw(w);
  }
}

/* ARGSUSED */
static void
DeletePrev(Widget aw, XEvent * event, String * params, Cardinal * num_params)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;

  if (!w->text.Editable)
    return;

  if (w->text.CursorPos > 0) {
    ClearHighlight(w);
    TextDelete(w, w->text.CursorPos - 1, 1);
    w->text.CursorPos--;
    Draw(w);
  }
}

/* ARGSUSED */
static void
DeleteToStart(Widget aw, XEvent * event, String * params, Cardinal * num_params)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;

  if (!w->text.Editable)
    return;

  if (w->text.CursorPos > 0) {
    ClearHighlight(w);
    TextDelete(w, 0, w->text.CursorPos);
    w->text.CursorPos=0;
    Draw(w);
  }
}

/* ARGSUSED */
static void
DeleteHighlighted(Widget aw, XEvent * event, String * params, Cardinal * num_params)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;

  if (!w->text.Editable)
    return;

  if (w->text.HighlightStart >= 0 && w->text.PendingDelete) {
    TextDeleteHighlighted(w);
    MassiveChangeDraw(w);
  }
}

/* ARGSUSED */
static void
TransposeChars(Widget aw, XEvent * event, String * params, Cardinal * num_params)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;
  char c;

  if (!w->text.Editable)
    return;

  ClearHighlight(w);
  if (w->text.CursorPos > 0 && w->text.CursorPos < w->text.TextLen) {
    c = w->text.Text[w->text.CursorPos -1];
    TextDelete(w, w->text.CursorPos - 1, 1);
    TextInsert(w, &c, 1);
    Draw(w);
  }
}

/* ARGSUSED */
static void
SelectStart(Widget aw, XEvent * event, String * params, Cardinal * num_params)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;

  if (!w->text.AllowSelection)
    return;

  w->text.CursorPos = TextPixelToPos(w, event->xbutton.x);
  w->text.HighlightPivotStart =
    w->text.HighlightPivotEnd = w->text.CursorPos;

  if (w->text.HighlightStart >= 0) {
    ClearHighlight(w);
  }
  else {
    EraseCursor(w);
    DrawCursor(w);
  }
}

/* ARGSUSED */
static void
ExtendStart(Widget aw, XEvent * event, String * params, Cardinal * num_params)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;
  int pos;

  if (!w->text.AllowSelection)
    return;

  pos = TextPixelToPos(w, event->xbutton.x);

  EraseCursor(w);
  if (w->text.HighlightStart < 0) {
    w->text.HighlightStart =
      w->text.HighlightEnd =
      w->text.HighlightPivotStart =
      w->text.HighlightPivotEnd = w->text.CursorPos;
  }
  else {
    w->text.HighlightPivotStart = w->text.HighlightStart;
    w->text.HighlightPivotEnd = w->text.HighlightEnd;
  }
  if (pos < w->text.HighlightStart) {
    w->text.HighlightStart = pos;
  }
  else {
    w->text.HighlightEnd = pos;
  }
  w->text.CursorPos = pos;
#ifdef DEBUG_TF
  printf("ExtendStart: %d - %d\n", w->text.HighlightStart,
    w->text.HighlightEnd);
#endif
  DrawHighlight(w);
  DrawCursor(w);
}

static void
ExtendHighlight(MwTextFieldWidget w)
{
  int x, pos;

  if (!w->text.AllowSelection)
    return;

  x = w->text.timer_x;
  pos = TextPixelToPos(w, x);

  if (x < (int) w->text.Margin) {
    pos = TextPixelToPos(w, (int) 0);
    if (pos > 0)
      pos--;
    else if (pos == w->text.CursorPos)
      return;
  }
  else if (x > (int) (w->text.Margin + w->text.ViewWidth)) {
    pos = TextPixelToPos(w, (int) (w->text.Margin + w->text.ViewWidth));
    if (pos < w->text.TextLen)
      pos++;
    else if (pos == w->text.CursorPos)
      return;
  }
  if (pos == w->text.CursorPos)
    return;

  EraseCursor(w);
  if (pos <= w->text.HighlightPivotStart) {
    w->text.HighlightStart = pos;
    w->text.HighlightEnd = w->text.HighlightPivotEnd;
  }
  else {
    w->text.HighlightStart = w->text.HighlightPivotStart;
    w->text.HighlightEnd = pos;
  }
  w->text.CursorPos = pos;
#ifdef DEBUG_TF
  printf("Highlighting: x=%d pos=%d  %d - %d\n", x, pos, w->text.HighlightStart,
    w->text.HighlightEnd);
#endif
  if (PositionCursor(w))
    DrawTextReposition(w);
  DrawHighlight(w);
  DrawCursor(w);
}

static void
ExtendTimer(XtPointer client_data, XtIntervalId * idp)
{
  MwTextFieldWidget w = (MwTextFieldWidget) client_data;

  ExtendHighlight(w);
  w->text.timer_id = XtAppAddTimeOut(
    XtWidgetToApplicationContext((Widget) w),
    (unsigned long) w->text.highlight_time,
    ExtendTimer,
    (XtPointer) w);

}

/* ARGSUSED */
static void
ExtendAdjust(Widget aw, XEvent * event, String * params, Cardinal * num_params)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;

  if (!w->text.AllowSelection)
    return;

  w->text.timer_x = event->xbutton.x;

  if (event->xbutton.x < w->text.Margin || event->xbutton.x > w->text.Margin + w->text.ViewWidth) {
    if (w->text.timer_id)
      ExtendHighlight(w);
    else
      ExtendTimer((XtPointer) w, (XtIntervalId) 0);
  }
  else {
    if (w->text.timer_id) {
      XtRemoveTimeOut(w->text.timer_id);
      w->text.timer_id = (XtIntervalId) 0;
    }
    ExtendHighlight(w);
  }
}

/* ARGSUSED */
static Boolean
ConvertSelection(Widget aw, Atom * selection, Atom * target, Atom * type,
  XtPointer * value, unsigned long *length, int *format)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;
  XSelectionRequestEvent *req = XtGetSelectionRequest(aw, *selection, NULL);

  if (*target == XA_TARGETS(XtDisplay(aw))) {
    Atom *targetP, *std_targets;
    unsigned long std_length;

    XmuConvertStandardSelection(aw, req->time, selection,
      target, type, (XtPointer)&std_targets,
      &std_length, format);

    *value = MwMalloc((unsigned) sizeof(Atom) * (std_length + 1));
    targetP = *(Atom **) value;
    *length = std_length + 1;
    *targetP++ = XA_STRING;
    memmove((char *) targetP, (char *) std_targets, sizeof(Atom) * std_length);
    MwFree((char *) std_targets);
    *type = XA_ATOM;
    *format = sizeof(Atom) * 8;
    return True;
  }
  else if (*target == XA_STRING) {
    *length = (long) w->text.SelectionLen;
    *value = w->text.SelectionText;
    *type = XA_STRING;
    *format = 8;
    return True;
  }
  return False;
}

/* ARGSUSED */
static void
LoseSelection(Widget aw, Atom * selection)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;

  ClearHighlight(w);
}

/* ARGSUSED */
static void
ExtendEnd(Widget aw, XEvent * event, String * params, Cardinal * num_params)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;
  int len;

  if (!w->text.AllowSelection)
    return;

  if (w->text.timer_id) {
    XtRemoveTimeOut(w->text.timer_id);
    w->text.timer_id = (XtIntervalId) 0;
  }
  len = w->text.HighlightEnd - w->text.HighlightStart;
  if (len > 0) {
    w->text.SelectionLen = len;
    if (w->text.SelectionText)
      MwFree(w->text.SelectionText);
    w->text.SelectionText = MwMalloc(len);
    strncpy(w->text.SelectionText, &w->text.Text[w->text.HighlightStart], len);

    XtOwnSelection(aw, XA_PRIMARY, event->xbutton.time,
      ConvertSelection, LoseSelection, NULL);
    XChangeProperty(XtDisplay(aw), DefaultRootWindow(XtDisplay(aw)),
      XA_CUT_BUFFER0, XA_STRING, 8, PropModeReplace,
      (unsigned char *) w->text.SelectionText, len);

  }
}

/* ARGSUSED */
static void
RequestSelection(Widget aw, XtPointer client, Atom * selection, Atom * type,
  XtPointer value, unsigned long *length, int *format)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;

  if ((value == NULL) || (*length == 0)) {
#ifdef DEBUG_TF
    printf("RequestSelection: no selection\n");
#endif
  }
  else {
    int savex;

    ClearHighlight(w);
    savex = w->text.OldCursorX;
    w->text.CursorPos = (int) client;
#ifdef DEBUG_TF
    printf("RequestSelection: inserting %s length=%d at pos: %d\n",
      (char *) value, (int) (*length), w->text.CursorPos);
#endif
    TextInsert(w, (char *) value, (int) (*length));
    w->text.OldCursorX = savex;
    Draw(w);
  }
}

/* ARGSUSED */
static void
InsertSelection(Widget aw, XEvent * event, String * params, Cardinal * num_params)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;
  int pos;

  if (!w->text.AllowSelection)
    return;

  pos = TextPixelToPos(w, event->xbutton.x);
#ifdef DEBUG_TF
  printf("InsertSelection: event at pos: %d\n", pos);
#endif
  XtGetSelectionValue(aw, XA_PRIMARY, XA_STRING,
    RequestSelection,
    (XtPointer) pos, event->xbutton.time);
}


/*
 * MwTextField private drawing functions
 */

static Boolean
PositionCursor(MwTextFieldWidget w)
{
  int x, start, end;
  Boolean moved;

  moved = False;
  if (w->text.CursorPos < 0)
    w->text.CursorPos = 0;
  else if (w->text.CursorPos > w->text.TextLen)
    w->text.CursorPos = w->text.TextLen;
  x = FontTextWidth(w->text.font, w->text.Text, w->text.CursorPos);
  start = -w->text.XOffset;
  end = start + w->text.ViewWidth;
  if (x < start) {
    w->text.XOffset = -x;
    moved = True;
  }
  else if (x > end) {
    w->text.XOffset = w->text.ViewWidth - x;
    moved = True;
  }
  return moved;
}

static Boolean
MassiveCursorAdjust(MwTextFieldWidget w)
{
  int start, end, last;
  Boolean moved;

  moved = False;
  end = FontTextWidth(w->text.font, w->text.Text, w->text.CursorPos);
  if (w->text.HighlightStart >= 0)
    start = FontTextWidth(w->text.font, w->text.Text, w->text.HighlightStart);
  else
    start = end;

  if (end < w->text.ViewWidth) {
    if (w->text.XOffset < 0) {
      w->text.XOffset = 0;
      moved = True;
    }
  }
  else if (start >= w->text.XOffset && end < w->text.XOffset + w->text.ViewWidth)
    return moved;
  else {
    last = FontTextWidth(w->text.font, w->text.Text, w->text.TextLen);
    if (start - end > w->text.ViewWidth) {
      if (last - end > w->text.ViewWidth)
	w->text.XOffset = w->text.ViewWidth - last;
      else
	w->text.XOffset = w->text.ViewWidth - end;
    }
    else if (end > w->text.ViewWidth)
      w->text.XOffset = w->text.ViewWidth - end;
    else
      w->text.XOffset = 0;
    moved = True;
  }
  return moved;
}

/*
 * Actually draw a range of text onto the widget
 */
static void
DrawText(MwTextFieldWidget w, int start, int end, Boolean highlight)
{
  int x;
  GC gc;

  if (!w->text.Echo)
    return;

  if (w->text.TextLen > 0) {
    if (start < 0)
      return;
    else if (end < start) {
      int temp;

      temp = start;
      start = end;
      end = temp;
    }
    if (end <= w->text.TextLen) {
      x = w->text.Margin + w->text.XOffset +
	FontTextWidth(w->text.font, w->text.Text, start);
      if (highlight)
	gc = w->text.highlightGC;
      else
	gc = w->text.drawGC;
      XDrawImageString(XtDisplay(w), XtWindow(w), gc,
	x, w->text.YOffset,
	&w->text.Text[start], end - start);
    }
  }
}

static void
DrawTextRange(MwTextFieldWidget w, int start, int end)
{
  if (!w->text.Echo)
    return;

  if (w->text.TextLen > 0) {
    if (start < 0)
      return;
    else if (end < start) {
      int temp;

      temp = start;
      start = end;
      end = temp;
    }

/* If there is no highlighting, or the refresh area doesn't cross the */
/* the highlight borders, just redraw it. */
    if (w->text.HighlightStart < 0 ||
      start >= w->text.HighlightEnd ||
      end <= w->text.HighlightStart) {
      DrawText(w, start, end, False);
    }

/* OK, the refresh area crosses one or both highlight borders. */
    else {
      int clip;

      while (start < end) {
	if (start < w->text.HighlightStart) {
	  if (end <= w->text.HighlightStart)
	    clip = end;
	  else
	    clip = w->text.HighlightStart;
	  DrawText(w, start, clip, False);
	  start = clip;
	}
	else if (start < w->text.HighlightEnd) {
	  if (end <= w->text.HighlightEnd)
	    clip = end;
	  else
	    clip = w->text.HighlightEnd;
	  DrawText(w, start, clip, True);
	  start = clip;
	}
	else {
	  DrawText(w, start, end, False);
	  start = end;
	}
      }


    }
  }
}

static void
DrawTextReposition(MwTextFieldWidget w)
{
  int xsrc, xdest, width, start, end;

  if (!w->text.Echo)
    return;

  if (w->text.XOffset < w->text.OldXOffset) {
    xsrc = w->text.OldXOffset - w->text.XOffset;
    xdest = 0;
    width = w->text.ViewWidth - xsrc + 1;

    /* Need to redraw some characters at the end. */

    end = TextPixelToPos(w, w->text.Margin + w->text.ViewWidth);
    start = TextPixelToPos(w, w->text.Margin + w->text.ViewWidth - xsrc);
  }
  else if (w->text.XOffset > w->text.OldXOffset) {
    xsrc = 0;
    xdest = w->text.XOffset - w->text.OldXOffset;
    width = w->text.ViewWidth - xdest + 1;

    /* Need to redraw some characters at the beginning. */

    start = TextPixelToPos(w, w->text.Margin);
    end = TextPixelToPos(w, w->text.Margin + xdest);
  }
  else
    return;

  if (width > 0) {
#ifdef DEBUG_TF
    printf("Reposition: xoff=%d old=%d src=%d dest=%d width=%d refresh %d-%d\n",
      w->text.XOffset, w->text.OldXOffset, xsrc, xdest, width, start, end);
#endif
    XCopyArea(XtDisplay(w), XtWindow(w), XtWindow(w),
      w->text.drawGC,
      w->text.Margin + xsrc, 0,
      (unsigned int) width, (unsigned int) w->core.height,
      w->text.Margin + xdest, 0);
    DrawTextRange(w, start, end);
  }
  w->text.OldXOffset = w->text.XOffset;
}

static void
DrawTextWithCopyArea(MwTextFieldWidget w)
{
  int x, insert_width;
  int xsrc, xdest, width;

  if (!w->text.Echo)
    return;

  x = w->text.XOffset;
  insert_width = FontTextWidth(w->text.font, &w->text.Text[w->text.FastInsertCursorStart], w->text.FastInsertTextLen);
  if (PositionCursor(w)) {
    /*
     *  if the text is scrolled, then:
     * 1.  the cursor is at the end
     * 2.  the copy will move to the left.
     */
    xsrc = 0;
    width = w->text.OldCursorX + x;
    xdest = w->text.ViewWidth - (x + w->text.OldCursorX) - insert_width;
    XCopyArea(XtDisplay(w), XtWindow(w), XtWindow(w),
      w->text.drawGC,
      w->text.Margin + xsrc, 0,
      (unsigned int) width, (unsigned int) w->core.height,
      w->text.Margin + xdest, 0);
#ifdef DEBUG_TF
    printf("DrawInsert: x=%d xsrc=%d xdest=%d width=%d\n", x, xsrc, xdest, width);
#endif
  }
  else {

    /*
     * the text hasn't been scrolled, so:
     * 1.  the text left of the cursor won't change
     * 2.  the stuff after the cursor will be moved right.
     */
    xsrc = FontTextWidth(w->text.font, w->text.Text, w->text.FastInsertCursorStart) + x;
    width = w->text.ViewWidth - xsrc;
    xdest = xsrc + insert_width;
    XCopyArea(XtDisplay(w), XtWindow(w), XtWindow(w),
      w->text.drawGC,
      w->text.Margin + xsrc, 0,
      (unsigned int) width, (unsigned int) w->core.height,
      w->text.Margin + xdest, 0);
#ifdef DEBUG_TF
    printf("DrawInsert: x=%d xsrc=%d xdest=%d width=%d\n", x, xsrc, xdest, width);
#endif
  }
  DrawTextRange(w, w->text.FastInsertCursorStart,
    w->text.FastInsertCursorStart + w->text.FastInsertTextLen);
  if (w->text.TextMaxLen > 0) {

    /*
     * This is pretty much a hack:
     * clear everything to end of window if this is a
     * fixed length MwTextField
     */
    xsrc = w->text.XOffset + w->text.TextWidth;
    width = w->text.ViewWidth - xsrc;
    /*    printf("DrawInsert: TextWidth=%d Old=%d\n", w->text.TextWidth, w->text.OldTextWidth); */
    XClearArea(XtDisplay(w), XtWindow(w),
      w->text.Margin + xsrc, 0,
      (unsigned int) width, w->core.height, False);
  }
  else if (w->text.TextWidth < w->text.OldTextWidth) {
    XClearArea(XtDisplay(w), XtWindow(w),
      w->text.Margin + w->text.XOffset + w->text.TextWidth, 0,
      w->text.OldTextWidth - w->text.TextWidth + 1,
      w->core.height, False);
  }
  w->text.OldTextWidth = w->text.TextWidth;
  w->text.OldXOffset = w->text.XOffset;
}

static void
DrawAllText(MwTextFieldWidget w)
{
  if (!w->text.Echo)
    return;

  DrawTextRange(w, 0, w->text.TextLen);
  if (w->text.TextWidth < w->text.OldTextWidth) {
    XClearArea(XtDisplay(w), XtWindow(w),
      w->text.Margin + w->text.XOffset + w->text.TextWidth, 0,
      w->text.OldTextWidth - w->text.TextWidth + 1,
      w->core.height, False);
  }
  w->text.OldTextWidth = w->text.TextWidth;
  w->text.OldXOffset = w->text.XOffset;
  w->text.OldHighlightStart = w->text.HighlightStart;
  w->text.OldHighlightEnd = w->text.HighlightEnd;
}

/* Draw an I-beam cursor */
static void
DrawIBeamCursor(MwTextFieldWidget w, int x, GC gc)
{
  XDrawLine(XtDisplay(w), XtWindow(w), gc,
    x, w->text.YOffset - FontAscent(w->text.font) - 1,
    x, w->text.YOffset + FontDescent(w->text.font) + 1);
  XDrawLine(XtDisplay(w), XtWindow(w), gc,
    x - 2, w->text.YOffset - FontAscent(w->text.font) - 1,
    x + 2, w->text.YOffset - FontAscent(w->text.font) - 1);
  XDrawLine(XtDisplay(w), XtWindow(w), gc,
    x - 2, w->text.YOffset + FontDescent(w->text.font) + 1,
    x + 2, w->text.YOffset + FontDescent(w->text.font) + 1);
}

static void
DrawCursor(MwTextFieldWidget w)
{
  int x;
  GC gc;

  if (w->text.DisplayCursor) {
    x = FontTextWidth(w->text.font, w->text.Text, w->text.CursorPos);
    w->text.OldCursorPos = w->text.CursorPos;
    w->text.OldCursorX = x;
    x += w->text.Margin + w->text.XOffset;

    gc = w->text.cursorGC;
    DrawIBeamCursor(w, x, gc);
  }
}

static void
EraseCursor(MwTextFieldWidget w)
{
  int x;

  if (w->text.DisplayCursor && w->text.OldCursorX >= 0) {
    x = w->text.OldCursorX + w->text.Margin + w->text.XOffset;
    DrawIBeamCursor(w, x, w->text.eraseGC);

    /* Little hack to fix up the character that might have been affected by
     * erasing the old cursor.
     */
    if (w->text.OldCursorPos < w->text.TextLen)
      DrawTextRange(w, w->text.OldCursorPos, w->text.OldCursorPos + 1);
  }
}

static void
ClearHighlight(MwTextFieldWidget w)
{
  if (!w->text.Echo)
    return;

  if (w->text.HighlightStart >= 0) {
    EraseCursor(w);
    DrawText(w, w->text.HighlightStart, w->text.HighlightEnd, False);
    DrawCursor(w);
    w->text.HighlightStart = w->text.HighlightEnd = -1;
  }
  w->text.OldHighlightStart = w->text.OldHighlightEnd = -1;
}

static void
DrawHighlight(MwTextFieldWidget w)
{
  if (!w->text.Echo)
    return;

  if (w->text.OldHighlightStart < 0) {
    DrawText(w, w->text.HighlightStart, w->text.HighlightEnd,
      True);
  }
  else {
    DrawText(w, w->text.HighlightStart, w->text.OldHighlightStart,
      (w->text.HighlightStart < w->text.OldHighlightStart));
    DrawText(w, w->text.HighlightEnd, w->text.OldHighlightEnd,
      (w->text.HighlightEnd > w->text.OldHighlightEnd));
  }
  w->text.OldHighlightStart = w->text.HighlightStart;
  w->text.OldHighlightEnd = w->text.HighlightEnd;
}

/*
 * Special redraw function after a text insertion
 */
static void
DrawInsert(MwTextFieldWidget w)
{
  /*  EraseCursor must be called before this */
  DrawTextWithCopyArea(w);
  DrawCursor(w);
}

/*
 * Redraw the entire widget, but don't scroll the window much
 */
static void
Draw(MwTextFieldWidget w)
{
  EraseCursor(w);
  PositionCursor(w);
  DrawAllText(w);
  DrawCursor(w);
}

/*
 * Like Draw(), but has different rules about scrolling the window to
 * place the cursor in a good place
 */
static void
MassiveChangeDraw(MwTextFieldWidget w)
{
  if (w->core.window == None) return;
  EraseCursor(w);
  MassiveCursorAdjust(w);
  DrawAllText(w);
  DrawCursor(w);
}

/*
 * Motif-like MwTextField public functions
 *
 * Note that this set of functions is only a subset of the functions available
 * in the real Motif XmTextField.
 */

Boolean
MwTextFieldGetEditable(Widget aw)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;

  if (!XtIsMwTextField(aw))
    return 0;

  return w->text.Editable;
}

int
MwTextFieldGetInsertionPosition(Widget aw)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;

  if (!XtIsMwTextField(aw))
    return 0;

  return w->text.CursorPos;
}

char *
MwTextFieldGetString(Widget aw)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;
  char *ret;

  if (!XtIsMwTextField(aw)) {
    ret = MwMalloc(1);
    *ret = '\0';
    return ret;
  }

  ret = MwMalloc(w->text.TextLen + 1);
  strncpy(ret, w->text.Text, w->text.TextLen);
  ret[w->text.TextLen] = '\0';
  return ret;
}

void
MwTextFieldInsert(Widget aw, int pos, char *str)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;
  int len;

  if (!XtIsMwTextField(aw))
    return;

  if (str && ((len = strlen(str)) > 0) && pos >= 0 && pos <= w->text.TextLen) {
    w->text.HighlightStart = w->text.HighlightEnd = pos;
    TextInsert(w, str, len);
    MassiveChangeDraw(w);
  }
}

void
MwTextFieldReplace(Widget aw, int first, int last, char *str)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;
  int len;

  if (!XtIsMwTextField(aw))
    return;

  if (str) {
    len = strlen(str);
    if (last > w->text.TextLen)
      last = w->text.TextLen;
    if (first <= last) {
      w->text.HighlightStart = first;
      w->text.HighlightEnd = last;
      TextDeleteHighlighted(w);
      TextInsert(w, str, len);
      MassiveChangeDraw(w);
    }
  }
}

void
MwTextFieldSetEditable(Widget aw, Boolean editable)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;

  if (!XtIsMwTextField(aw))
    return;

  w->text.Editable = editable;
}

void
MwTextFieldSetInsertionPosition(Widget aw, int pos)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;

  if (!XtIsMwTextField(aw))
    return;

  if (pos >= 0 && pos <= w->text.TextLen) {
    w->text.CursorPos = pos;
    MassiveChangeDraw(w);
  }
}

/* ARGSUSED */
void
MwTextFieldSetSelection(Widget aw, int start, int end, Time time)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;

  if (!XtIsMwTextField(aw))
    return;

  if (end < start) {
    int temp;

    temp = start;
    start = end;
    end = temp;
  }
  if (start < 0)
    start = 0;
  if (end > w->text.TextLen)
    end = w->text.TextLen;
  w->text.HighlightStart = start;
  w->text.HighlightEnd = w->text.CursorPos = end;
  MassiveChangeDraw(w);
}

void
MwTextFieldSetString(Widget aw, char *str)
{
  MwTextFieldWidget w = (MwTextFieldWidget) aw;
  int len;

  if (!XtIsMwTextField(aw))
    return;

  if (str) {
    len = strlen(str);
    w->text.HighlightStart = 0;
    w->text.HighlightEnd = w->text.TextLen;
    TextDeleteHighlighted(w);
    TextInsert(w, str, len);
    MassiveChangeDraw(w);
  }
}
