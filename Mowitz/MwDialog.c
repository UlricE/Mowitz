/*
Copyright (C) 1997-2002  Ulric Eriksson <ulric@siag.nu>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the Licence, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <X11/Shell.h>
#include <X11/xpm.h>

#include <X11/Xaw/Box.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/List.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Dialog.h>

#include <Mowitz/Mowitz.h>
#include "pixmaps/unknown.xpm"


static Widget pshell, dialog, dialogIcon, dialogLabel, dialogText,
	dialogCancel, dialogDone;

static Widget alert, listshell;
static long status;

static void wm_del(Widget w)
{
	Atom da;
	if (XtWindow(w)) {
		da = XInternAtom(XtDisplay(w), "WM_DELETE_WINDOW", False);
		XSetWMProtocols(XtDisplay(w), XtWindow(w), &da, 1);
	}
}

static Widget add_button(Widget pw, char *name, char *label,
        void (*cb)(Widget, XtPointer, XtPointer), XtPointer data)
{
        Widget w = XtVaCreateManagedWidget(name,
                commandWidgetClass, pw,
                (char *)0);
        MwLabelSet(w, label);
        XtVaSetValues(w,
		XtNwidth, 80,
		XtNshadowWidth, 1,
		(char *)0);
        if (cb) XtAddCallback(w, XtNcallback, cb, data);
        return w;
}

static void
alert_clicked(Widget w, XtPointer client_data, XtPointer call_data)
{
	XtPopdown(alert);
	status = (long)client_data;
}

/* ---
Pops up a box with a text and a bunch of buttons.
*/

int MwAlertBox(Widget pw, char *text, char *buttons[], int nbuttons)
{
	Widget topbox, buttonbox, label, command;
	int i;
	XtAppContext app_context = XtWidgetToApplicationContext(pw);

	alert = XtVaCreatePopupShell("alert",
		transientShellWidgetClass, pw,
		XtNtitle, _("Alert"),
		(char *)0);
	topbox = XtVaCreateManagedWidget("topbox",
		boxWidgetClass, alert, (char *)0);
	label = XtVaCreateManagedWidget("label",
		labelWidgetClass, topbox,
		(char *)0);
	MwLabelSet(label, text);
	buttonbox = XtVaCreateManagedWidget("buttonbox",
		boxWidgetClass, topbox, (char *)0);
	for (i = 0; i < nbuttons; i++) {
		command = add_button(buttonbox, "command", buttons[i],
			alert_clicked, (XtPointer)i);
	}
	status = MW_WAITING;
	MwCenter(alert);
	XtPopup(alert, XtGrabNonexclusive);
	wm_del(alert);
	while (status == MW_WAITING) {
		XEvent event_return;
		XtAppNextEvent(app_context, &event_return);
		XtDispatchEvent(&event_return);
	}
	XtDestroyWidget(alert);
	return status;
}

/* ---
Same as an alertbox with only an "OK" button.
*/

int MwErrorBox(Widget pw, char *message)
{
	char *buttons[] = {"OK"};

	return MwAlertBox(pw, message, buttons, 1);
}

/* ---
Display a logo, a text and an OK button.
*/
void MwAboutBox(Widget pw, char *pixmap, char *message)
{
	Pixel color;
	Pixmap pm;
	Widget topbox, label, command, image;
	XtAppContext app_context = XtWidgetToApplicationContext(pw);

	XtVaGetValues(pw, XtNbackground, &color, (char *)0);
	pm = MwLoadPixmap(XtDisplay(pw), color, pixmap);
	alert = XtVaCreatePopupShell("about",
		transientShellWidgetClass, pw,
		XtNtitle, _("About"),
		XtNwidth, 400,
		XtNheight, 200,
		(char *)0);
	topbox = XtVaCreateManagedWidget("topbox",
		mwRudegridWidgetClass, alert,
		XtNxLayout, "4 80 4 50% 80 50% 4 80 4",
		XtNyLayout, "4 50% 80 50% 4 22 4",
		(char *)0);
	image = XtVaCreateManagedWidget("image",
		mwImageWidgetClass, topbox,
		XtNbitmap, pm,
		XtNgridx, 1,
		XtNgridy, 2,
		(char *)0);
	label = XtVaCreateManagedWidget("label",
		labelWidgetClass, topbox,
		XtNgridx, 3,
		XtNgridy, 1,
		XtNgridWidth, 7,
		XtNgridHeight, 3,
		(char *)0);
	MwLabelSet(label, message);

	command = XtVaCreateManagedWidget("command",
		commandWidgetClass, topbox,
		XtNgridx, 4,
		XtNgridy, 5,
		XtNlabel, "OK",
		XtNwidth, 80,
		(char *)0);
	XtAddCallback(command, XtNcallback, alert_clicked, 0);

	status = MW_WAITING;
	MwCenter(alert);
	XtPopup(alert, XtGrabNonexclusive);
	wm_del(alert);
	while (status == MW_WAITING) {
		XEvent event_return;
		XtAppNextEvent(app_context, &event_return);
		XtDispatchEvent(&event_return);
	}
	XtDestroyWidget(alert);
}

/* ---
Display an About: Siag Office" box.
*/
void MwAboutSiag(Widget pw)
{
	char b[1024];
	sprintf(b, "%s\n\nhttp://siag.nu/", 
		_("This program is part of Siag Office,\n"
		"an effort to create a full-featured, free\n"
		"office package for Unix and X."));
	MwAboutBox(pw, "siagoffice.xpm", b);
}


static void
listsel_clicked(Widget w, XtPointer client_data, XtPointer call_data)
{
	XtPopdown(listshell);
	status = (long)client_data;
}

/* ---
Pops up a list box. Returns the index of the selected item, or
-1 for none.
*/

int MwListBox(Widget pw, char *text, char *choices[], int nchoices)
{
	Widget form, buttonframe,
     		buttonbox, label, viewport, listframe, list,
		okbutton, cancelbutton;
	int n;
	XtAppContext app_context = XtWidgetToApplicationContext(pw);
	Screen *s = XtScreen(pw);
	unsigned long white = WhitePixelOfScreen(s);

	listshell = XtVaCreatePopupShell("lsel_shell",
		transientShellWidgetClass, pw,
		XtNtitle, _("List"),
		XtNwidth, 300,
		XtNheight, 300,
		(char *)0);
	form = XtVaCreateManagedWidget("lsel_form",
		mwRudegridWidgetClass, listshell,
		XtNxLayout, "5 100% 5",
		XtNyLayout, "5 100% 5 35",
		(char *)0);
	buttonframe = XtVaCreateManagedWidget("lsel_buttonframe",
		mwFrameWidgetClass, form,
		XtNgridy, 3,
		XtNgridWidth, 3,
		XtNshadowWidth, 1,
		(char *)0);
	buttonbox = XtVaCreateManagedWidget("lsel_buttonbox",
		boxWidgetClass, buttonframe,
		XtNborderWidth, 0,
		(char *)0);
	listframe = XtVaCreateManagedWidget("lsel_listframe",
		mwFrameWidgetClass, form,
		XtNgridx, 1,
		XtNgridy, 1,
		XtNshadowType, Groove,
		XtNmarginWidth, 2,
		XtNmarginHeight, 2,
		XtNallowResize, False,
		(char *)0);
	label = XtVaCreateManagedWidget("lsel_label",
		labelWidgetClass, listframe,
		(char *)0);
	MwLabelSet(label, text);
	XtVaSetValues(listframe,
		XtNtitle, label,
		(char *)0);
	viewport = XtVaCreateManagedWidget("lsel_viewport",
		viewportWidgetClass, listframe,
		XtNforceBars, True,
		XtNborderWidth, 0,
		(char *)0);
	list = XtVaCreateManagedWidget("lsel_list",
		listWidgetClass, viewport,
		XtNbackground, white,
		(char *)0);
	XawListChange(list, choices, nchoices, 0, True);
	XtVaSetValues(list, XtNwidth, MwWidthGet(list)+20, (char *)0);
	okbutton = add_button(buttonbox, "lsel_okbutton",
		"OK", listsel_clicked, (XtPointer)1);
	cancelbutton = add_button(buttonbox, "lsel_cancelbutton",
		"Cancel", listsel_clicked, (XtPointer)0);
	status = MW_WAITING;
	MwCenter(listshell);
	XtPopup(listshell, XtGrabNonexclusive);
	XawListChange(list, choices, nchoices, 0, True);
	wm_del(listshell);

	while (status == MW_WAITING) {
		XEvent event_return;
		XtAppNextEvent(app_context, &event_return);
		XtDispatchEvent(&event_return);
	}
	n = XawListShowCurrent(list)->list_index;
	XtDestroyWidget(listshell);
	if (status != MW_ABORT)	/* clicked OK */
		return n;
	else		/* clicked Cancel */
		return -1;
}

static void DialogDone(Widget w, XtPointer client_data, XtPointer call_data)
{
	XtPopdown(pshell);
	status = MW_DONE;
}

static void DialogAbort(Widget w, XtPointer client_data, XtPointer call_data)
{
	XtPopdown(pshell);
	status = MW_ABORT;
}

static void DialogCancelAction(Widget w, XEvent * event,
		String * params, Cardinal * n)
{
	XtPopdown(pshell);
	status = MW_ABORT;
}

static void DialogDoneAction(Widget w, XEvent * event,
		String * params, Cardinal * n)
{
	XtPopdown(pshell);
	status = MW_DONE;
}

static XtActionsRec actions[] = {
	{"dialog-done", DialogDoneAction},
	{"dialog-cancel", DialogCancelAction},
};

static Atom wm_delete_window;
static Pixmap default_icon = None;

/* ---
Pops up a dialog box with an optional icon, a title, a prompt, a text field
and an OK and a Cancel button.
The text buffer must contain a valid initial value, i.e. it
must at least be cleared before calling this function.
*/

int MwDialogInputIcon(Widget pw, char *title,
		char *prompt, char *buffr, Pixmap icon)
{
	XtAppContext app_context = XtWidgetToApplicationContext(pw);
	String string;
	static int init_done = 0;

	status = MW_WAITING;

	if (!init_done) {
		XpmAttributes xpm_attr;
		static XpmColorSymbol none_color = { NULL, "None", (Pixel)0 };

		xpm_attr.valuemask = XpmReturnPixels|XpmColorSymbols;
		xpm_attr.colorsymbols = &none_color;
		xpm_attr.numsymbols = 1;
		XtVaGetValues(pw,
			XtNbackground, &none_color.pixel,
			(char *)0);
		XpmCreatePixmapFromData(XtDisplay(pw),
			DefaultRootWindow(XtDisplay(pw)),
			unknown_xpm, &default_icon, NULL, &xpm_attr);

		XtAppAddActions(app_context, actions, XtNumber(actions));
		init_done = 1;
	}

	pshell = XtVaCreatePopupShell("pshell",
		transientShellWidgetClass, pw,
		XtNtitle, _("Dialog"),
		(char *)0);
	dialog = XtVaCreateManagedWidget("dialog",
		formWidgetClass, pshell,
		(char *)0);
	dialogIcon = XtVaCreateManagedWidget("dialogIcon",
		labelWidgetClass, dialog,
		XtNleft, XtChainLeft,
		XtNright, XtChainLeft,
		XtNtop, XtChainTop,
		XtNright, XtChainTop,
		XtNpixmap, default_icon,
		(char *)0);
	dialogLabel = XtVaCreateManagedWidget("dialogLabel",
		labelWidgetClass, dialog,
		XtNleft, XtChainLeft,
		XtNright, XtChainRight,
		XtNtop, XtChainTop,
		XtNbottom, XtChainTop,
		XtNfromHoriz, dialogIcon,
		(char *)0);
	dialogText = XtVaCreateManagedWidget("dialogText",
		mwTextfieldWidgetClass, dialog,
		XtNwidth, 400,
		XtNleft, XtChainLeft,
		XtNright, XtChainRight,
		XtNtop, XtChainTop,
		XtNbottom, XtChainTop,
		XtNfromHoriz, dialogIcon,
		XtNfromVert, dialogLabel,
		(char *)0);
	wm_delete_window = XInternAtom(XtDisplay(pshell),
				"WM_DELETE_WINDOW", False);
	XtOverrideTranslations(pshell,
		XtParseTranslationTable(
			"<Message>WM_PROTOCOLS:	dialog-cancel()"));

	XtOverrideTranslations(dialogText,
		XtParseTranslationTable(
			"<Key>Return:	dialog-done()\n"
			"<Key>Escape:	dialog-cancel()"));

	dialogDone = add_button(dialog, "dialogDone", "OK",
		DialogDone, NULL);
	XtVaSetValues(dialogDone,
		XtNwidth, 80,
		XtNleft, XtChainLeft,
		XtNright, XtChainLeft,
		XtNtop, XtChainBottom,
		XtNbottom, XtChainBottom,
		XtNfromVert, dialogText,
		(char *)0);
	dialogCancel = add_button(dialog, "dialogCancel", "Cancel",
		DialogAbort, NULL);
	XtVaSetValues(dialogCancel,
		XtNwidth, 80,
		XtNleft, XtChainLeft,
		XtNright, XtChainLeft,
		XtNtop, XtChainBottom,
		XtNbottom, XtChainBottom,
		XtNfromVert, dialogText,
		XtNfromHoriz, dialogDone,
		(char *)0);

	if (icon == None) icon = default_icon;

	XtVaSetValues(dialogIcon,
		XtNbitmap, icon,
		(char *)0);
	XtVaSetValues(pshell,
		XtNtitle, title,
		(char *)0);
	MwLabelSet(dialogLabel, prompt);
	XtVaSetValues(dialogText,
		XtNstring, buffr,
		XtNinsertPosition, strlen(buffr),
		(char *)0);
	MwCenter(pshell);
	XtPopup(pshell, XtGrabExclusive);
	XSetWMProtocols(XtDisplay(pshell), XtWindow(pshell),
			&wm_delete_window, 1);
	XtSetKeyboardFocus(pshell, dialogText);

	while (status == MW_WAITING) {
		XEvent event_return;

		XtAppNextEvent(app_context, &event_return);
		XtDispatchEvent(&event_return);
	}

	XtVaGetValues(dialogText,
		XtNstring, &string,
		(char *)0);
	strcpy(buffr, string);
	XtDestroyWidget(pshell);
	return status;
}

/* ---
Pops up a dialog box with a prompt, a text field and an OK and a Cancel
button. The text buffer must contain a valid initial value, i.e. it
must at least be cleared before calling this function.
*/

int MwDialogInput(Widget pw, char *prompt, char *buffr)
{
	return MwDialogInputIcon(pw, "Dialog", prompt, buffr, None);
}

static void stderr_input(Widget pw, int *fd, XtInputId *id)
{
	static char data[1024];
	static int active = 0;
	int n;
	fd_set rfds;
	struct timeval tv;

	if (active || (status == MW_WAITING)) return;
	active = 1;
	FD_ZERO(&rfds);
	FD_SET(fd[0], &rfds);
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	if (select(fd[0]+1, &rfds, NULL, NULL, &tv)) {
		n = read(fd[0], &data[0], sizeof data-1);
		data[n] = '\0';
		MwErrorBox(pw, data);
	}
	active = 0;
}

void MwStderrRedir(Widget pw)
{
	int fd[2];

	pipe(fd);
	XtAppAddInput(XtWidgetToApplicationContext(pw),
		fd[0], (XtPointer)XtInputReadMask,
		(XtInputCallbackProc)stderr_input, pw);
	dup2(fd[1], 2);
	close(fd[1]);
}

