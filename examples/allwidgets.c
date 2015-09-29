#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Label.h>
#include <Mowitz/Mowitz.h>

#define MENUBAR (1)

static int bars = MENUBAR;

static Widget topLevel, topbox;
static Widget animator, canvas, check, color, combo, frame, image;
static Widget menubar, menubox, menubutton, menu;
static Widget richtext, rudegrid, ruler, slider, vslider;
static Widget spinner, tabbing, table, tabs, textfield, tooltip;

static XColor xcolor;

static String fallback[] = {
	"*background: grey",
	"*MwRichtext.background: white",
	"*MwTextField.background: white",
#if 0
	"*borderWidth: 0",
#endif
	"*Frame.shadowWidth: 1",
	"*font: -*-helvetica-medium-r-normal-*-12-*-*-*-*-*-iso8859-*",
	NULL
};

static char *years[] = {
	"2000", "2001", "2002", "2003", "2004", "2005", "2006",
	"2007", "2008", "2009", "2010", "2011", "2012", "2013",
	"2014", "2015", "2016", "2017", "2018", "2019", "2020"
};

static char *months[] = {
	"January", "February", "March", "April", "May", "June",
	"July", "August", "September", "October", "November", "December"
};

static char *days[] = {
	"Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
	"Saturday", "Sunday"
};

static MwAniScript script0[] = {
	{0, 0, 0, 10, 100, 1, script0+1},
	{1000, 0, 0, 100, 10, 1, NULL}};

static MwAniScript script1[] = {{0, 110, 20, 0, 0, 1, NULL}};
static MwAniScript script2[] = {{0, 110, 35, 0, 0, 1, NULL}};
static MwAniScript script3[] = {{0, 110, 50, 0, 0, 1, NULL}};
static MwAniScript script4[] = {{0, 110, 65, 0, 0, 1, NULL}};

static MwAniObject cast[] = {
	{MW_ANI_RECTANGLE, "Object0", script0, 0, "", cast+1},
	{MW_ANI_STRING, "Object1", script1, 0, "C = play", cast+2},
	{MW_ANI_STRING, "Object2", script2, 0, "S = pause", cast+3},
	{MW_ANI_STRING, "Object3", script3, 0, "P = previous frame", cast+4},
	{MW_ANI_STRING, "Object4", script4, 0, "N = next frame", NULL},
};

static MwFmt myfmt = {"Courier", 150, 1, 0, 0, 0, "red", "yellow", 0,
		MW_VADJ_CENTER, MW_HADJ_LEFT, 0};

/* width height colours a z colour1 colour2 */
static char *mygrad = "100 100 128 0 1 SpringGreen Chartreuse";

static void cb_drag(Widget w, XtPointer client_data, XtPointer call_data)
{
	DropPosition *where = (DropPosition *)call_data;
	printf("cb_drag at (%d,%d)\n", where->x, where->y);
}

static void cb_drop(Widget w, XtPointer client_data, XtPointer call_data)
{
	int prot = (int)call_data;
	int dnd_type;
	Atom xdnd_type;
	char *data;

	switch (prot) {
	case DndDrop:
		printf("cb_drop received OffiX drop\n");
		XtVaGetValues(w,
			XtNdndType, &dnd_type,
			XtNdropData, &data,
			(char *)0);
		printf("type = %d, data = '%s'\n", dnd_type, data);
		break;
	case XdndDrop:
		printf("cb_drop received Xdnd drop\n");
		XtVaGetValues(w,
			XtNxdndType, &xdnd_type,
			XtNdropData, &data,
			(char *)0);
#if 1
		printf("type = '%s', data = '%s'\n",
			XGetAtomName(XtDisplay(w), xdnd_type), data);
#else
		printf("type = %ld, data = '%s'\n",
			xdnd_type, data);
#endif
		break;
	case MotifDrop:
		printf("cb_drop received Motif drop\n");
		break;
	default:
		printf("cb_drop received unknown drop\n");
		break;
	}
}

static void redraw_canvas(Widget w, XtPointer client_data, XtPointer call_data)
{
	printf("redraw canvas\n");
}

static void cb_spinner(Widget w, XtPointer client_data, XtPointer call_data)
{
	int n = (int)call_data;
	printf("cb_spinner(%s) => %d\n", XtName(w), n);
}

static void set_color(XColor *xcolor)
{
	MwAllocColor(XtDisplay(color), None, xcolor);
	XtVaSetValues(color, XtNbackground, xcolor->pixel, (char *)0);
}

static void cb_red(Widget w, XtPointer client_data, XtPointer call_data)
{
	int n = (int)call_data;
	xcolor.red = n;
	set_color(&xcolor);
}

static void cb_green(Widget w, XtPointer client_data, XtPointer call_data)
{
	int n = (int)call_data;
	xcolor.green = n;
	set_color(&xcolor);
}

static void cb_blue(Widget w, XtPointer client_data, XtPointer call_data)
{
	int n = (int)call_data;
	xcolor.blue = n;
	set_color(&xcolor);
}

static void cb_open(Widget w, XtPointer client_data, XtPointer call_data)
{
	char path[1024] = ".";
	char name[1024] = "";
	char *patterns[] = { "All files (*)", NULL };
	char fmt[1024] = "";
	char *extra = NULL;
	int ext = 0;

	MwFileselInput(topLevel, path, name, patterns, fmt, extra, ext);
}

static void cb_quit(Widget w, XtPointer client_data, XtPointer call_data)
{
	MwMallocExit();
	exit(0);
}

static void cb_error(Widget w, XtPointer client_data, XtPointer call_data)
{
	MwErrorBox(topLevel, "Nothing to see here.");
}


static void cb_alert(Widget w, XtPointer client_data, XtPointer call_data)
{
	char *buttons[] = {"Abort", "Retry", "Continue", "Fail"};
	MwAlertBox(topLevel, "An unexpected error didn't occur at all.",
		buttons, 4);
}


static void cb_about(Widget w, XtPointer client_data, XtPointer call_data)
{
printf("cb_about(%s)\n", XtName(w));
	MwAboutSiag(topLevel);
}

static void quit(Widget w, XEvent *event, String *params, Cardinal *n)
{
	exit(0);
}

static XtActionsRec actions[] =
{
	{"quit", quit}
};

static void remake_ylayout(void)
{
	char b[100];
	sprintf(b, "%s 100%%",
		(bars&MENUBAR)?"30":"0");
	XtVaSetValues(rudegrid,
		XtNyLayout, b,
		(char *)0);
}

static void attach(Widget w, XtPointer client_data, XtPointer call_data)
{
	Widget vw = (Widget)call_data;
printf("attach(%s)\n", XtName(w));
	if (vw == menubar) bars |= MENUBAR;
	remake_ylayout();
}

static void detach(Widget w, XtPointer client_data, XtPointer call_data)
{
	Widget vw = (Widget)call_data;
printf("detach(%s)\n", XtName(w));
	if (vw == menubar) bars &= ~MENUBAR;
	remake_ylayout();
}

static void select_tab(Widget w, XtPointer client_data, XtPointer call_data)
{
	Widget label = (Widget)client_data;
	int pos = (int)call_data;
	char *n = MwTabbingPosToText(w, pos);
	XtVaSetValues(label, XtNlabel, n, (char *)0);
}

static char *table_text(void *p, int r, int c)
{
	static char b[1024];
	sprintf(b, "%d*%d = %d", r, c, r*c);
	return b;
}

int main(int argc, char **argv)
{
	Atom wm_protocols, wm_delete_window;
	Atom *drop_types;
	XtAppContext ac;
	Widget w;
	int i;

	topLevel = XtVaOpenApplication(&ac, "AllWidgets",
		NULL, 0,
		&argc, argv,
		fallback,
		mwApplicationShellWidgetClass,
		XtNwidth, 300,
		XtNheight, 200,
		(char *)0);
	XtAppAddActions(ac, actions, XtNumber(actions));
	drop_types = MwMalloc(3*sizeof *drop_types);
	drop_types[0] = XInternAtom(XtDisplay(topLevel), "text/plain", False);
	drop_types[1] = XInternAtom(XtDisplay(topLevel), "text/uri-list", False);
	drop_types[2] = None;
	XtVaSetValues(topLevel,
		XtNdropTypes, drop_types,
		(char *)0);
	XtAddCallback(topLevel, XtNdragCallback, cb_drag, NULL);
	XtAddCallback(topLevel, XtNdropCallback, cb_drop, NULL);
	rudegrid = XtVaCreateManagedWidget("rudegrid",
		mwRudegridWidgetClass, topLevel,
		XtNyLayout, "30 100%",
		(char *)0);
	menubar = XtVaCreateManagedWidget("menubar",
		mwRudegridWidgetClass, rudegrid,
		XtNxLayout, "9 100%",
		(char *)0);
	MwMakeHandle(menubar, menubar, detach, attach);
	menubox = XtVaCreateManagedWidget("menubox",
		mwMenuBarWidgetClass, menubar,
		XtNgridx, 1,
		(char *)0);
	bars |= MENUBAR;
	menubutton = XtVaCreateManagedWidget("menubutton",
		mwMBButtonObjectClass, menubox,
		XtNlabel, "File",
		XtNmenu_name, "filemenu",
		(char *)0);
	menu = XtVaCreatePopupShell("filemenu",
		mwMenuWidgetClass, menubox, NULL, 0);
	w = XtVaCreateManagedWidget("Open",
		mwLabelMEObjectClass, menu,
		XtNlabel, "Open",
		(char *)0);
	XtAddCallback(w, XtNcallback, cb_open, NULL);
	XtVaCreateManagedWidget("-",
		mwLineMEObjectClass, menu,
		(char *)0);
	w = XtVaCreateManagedWidget("Exit",
		mwLabelMEObjectClass, menu,
		XtNlabel, "Exit",
		(char *)0);
	XtAddCallback(w, XtNcallback, cb_quit, NULL);

	menubutton = XtVaCreateManagedWidget("menubutton",
		mwMBButtonObjectClass, menubox,
		XtNlabel, "Misc",
		XtNmenu_name, "miscmenu",
		(char *)0);
	menu = XtVaCreatePopupShell("miscmenu",
		mwMenuWidgetClass, menubox, NULL, 0);
	w = XtVaCreateManagedWidget("Alert",
		mwLabelMEObjectClass, menu,
		XtNlabel, "Alert",
		(char *)0);
	XtAddCallback(w, XtNcallback, cb_alert, NULL);
	w = XtVaCreateManagedWidget("Error",
		mwLabelMEObjectClass, menu,
		XtNlabel, "Error",
		(char *)0);
	XtAddCallback(w, XtNcallback, cb_error, NULL);

	menubutton = XtVaCreateManagedWidget("menubutton",
		mwMBButtonObjectClass, menubox,
		XtNlabel, "Help",
		XtNmenu_name, "helpmenu",
		(char *)0);
	menu = XtVaCreatePopupShell("helpmenu",
		mwMenuWidgetClass, menubox, NULL, 0);
	XtVaCreateManagedWidget("Contents",
		mwLabelMEObjectClass, menu,
		XtNlabel, "Contents",
		(char *)0);
	XtVaCreateManagedWidget("-",
		mwLineMEObjectClass, menu,
		(char *)0);
	w = XtVaCreateManagedWidget("About",
		mwLabelMEObjectClass, menu,
		XtNlabel, "About",
		(char *)0);
	XtAddCallback(w, XtNcallback, cb_about, NULL);
	tabs = XtVaCreateManagedWidget("tabs",
		mwTabsWidgetClass, rudegrid,
		XtNgridy, 1,
		(char *)0);

	for (i = 0; i < 5; i++) {
		cast[i].fmt = MwEncodeFormat(~0, &myfmt);
	}
	animator = XtVaCreateManagedWidget("animator",
		mwAnimatorWidgetClass, tabs,
		XtNanimatorCast, cast,
		XtNgradient, mygrad,
		(char *)0);

	canvas = XtVaCreateManagedWidget("canvas",
		mwCanvasWidgetClass, tabs,
		(char *)0);
	XtAddCallback(canvas, XtNcallback, redraw_canvas, (char *)0);
	topbox = XtVaCreateManagedWidget("check",
		boxWidgetClass, tabs,
		XtNborderWidth, 0,
		(char *)0);
	check = XtVaCreateManagedWidget("check",
		mwCheckWidgetClass, topbox,
		XtNlabel, "Check!",
		(char *)0);
	check = XtVaCreateManagedWidget("check",
		mwCheckWidgetClass, topbox,
		XtNlabel, "Radio!",
		XtNcheckStyle, MwRadioWin,
		XtNradioGroup, topbox,
		XtNradioStart, topbox,
		(char *)0);
	check = XtVaCreateManagedWidget("check",
		mwCheckWidgetClass, topbox,
		XtNlabel, "Radio!",
		XtNcheckStyle, MwRadioWin,
		XtNradioGroup, topbox,
		XtNradioStart, topbox,
		(char *)0);
	check = XtVaCreateManagedWidget("check",
		mwCheckWidgetClass, topbox,
		XtNlabel, "Check!",
		(char *)0);
	color = XtVaCreateManagedWidget("color",
		mwRudegridWidgetClass, tabs,
		XtNxLayout, "4 80 4 80",
		XtNyLayout, "4 22 4 22 4 22",
		(char *)0);
	MwAllocNamedColor(XtDisplay(color), "red", &xcolor);
	XtVaCreateManagedWidget("Red",
		labelWidgetClass, color,
		XtNgridx, 1, XtNgridy, 1,
		XtNbackground, xcolor.pixel,
		(char *)0);
	w = XtVaCreateManagedWidget("red",
		mwSpinnerWidgetClass, color,
		XtNgridx, 3, XtNgridy, 1,
		XtNmin, 0, XtNmax, 65535, XtNstep, 100,
		(char *)0);
	XtAddCallback(w, XtNcallback, cb_red, (char *)0);
	MwAllocNamedColor(XtDisplay(color), "green", &xcolor);
	XtVaCreateManagedWidget("Green",
		labelWidgetClass, color,
		XtNgridx, 1, XtNgridy, 3,
		XtNbackground, xcolor.pixel,
		(char *)0);
	w = XtVaCreateManagedWidget("green",
		mwSpinnerWidgetClass, color,
		XtNgridx, 3, XtNgridy, 3,
		XtNmin, 0, XtNmax, 65535, XtNstep, 100,
		(char *)0);
	XtAddCallback(w, XtNcallback, cb_green, (char *)0);
	MwAllocNamedColor(XtDisplay(color), "blue", &xcolor);
	XtVaCreateManagedWidget("Blue",
		labelWidgetClass, color,
		XtNgridx, 1, XtNgridy, 5,
		XtNbackground, xcolor.pixel,
		(char *)0);
	w = XtVaCreateManagedWidget("blue",
		mwSpinnerWidgetClass, color,
		XtNgridx, 3, XtNgridy, 5,
		XtNmin, 0, XtNmax, 65535, XtNstep, 100,
		(char *)0);
	XtAddCallback(w, XtNcallback, cb_blue, (char *)0);
	xcolor.red = 0;
	xcolor.green = 0;
	xcolor.blue = 0;
	set_color(&xcolor);
	topbox = XtVaCreateManagedWidget("combo",
		boxWidgetClass, tabs,
		XtNborderWidth, 0,
		(char *)0);
	combo = XtVaCreateManagedWidget("combo",
		mwComboWidgetClass, topbox,
		XtNcomboData, years,
		XtNcomboNData, sizeof(years)/sizeof(years[0]),
		(char *)0);
	combo = XtVaCreateManagedWidget("combo",
		mwComboWidgetClass, topbox,
		XtNcomboData, months,
		XtNcomboNData, sizeof(months)/sizeof(months[0]),
		(char *)0);
	combo = XtVaCreateManagedWidget("combo",
		mwComboWidgetClass, topbox,
		XtNcomboData, days,
		XtNcomboNData, sizeof(days)/sizeof(days[0]),
		(char *)0);
	combo = XtVaCreateManagedWidget("combo",
		mwComboWidgetClass, topbox,
		(char *)0);
	MwComboTextChange(combo, "Ulric");
	frame = XtVaCreateManagedWidget("frame",
		mwFrameWidgetClass, tabs,
		(char *)0);
	image = XtVaCreateManagedWidget("image",
		mwImageWidgetClass, tabs,
		(char *)0);
	richtext = XtVaCreateManagedWidget("richtext",
		boxWidgetClass, tabs,
		(char *)0);
	XtVaCreateManagedWidget("richtext",
		mwRichtextWidgetClass, richtext,
		(char *)0);
	XtVaCreateManagedWidget("richtext",
		mwRichtextWidgetClass, richtext,
		(char *)0);
	XtVaCreateManagedWidget("richtext",
		mwRichtextWidgetClass, richtext,
		(char *)0);
	ruler = XtVaCreateManagedWidget("ruler",
		mwRulerWidgetClass, tabs,
		(char *)0);
	topbox = XtVaCreateManagedWidget("slider",
		boxWidgetClass, tabs,
		XtNborderWidth, 0,
		(char *)0);
	slider = XtVaCreateManagedWidget("slider",
		mwSliderWidgetClass, topbox,
		(char *)0);
	vslider = XtVaCreateManagedWidget("vslider",
		mwVSliderWidgetClass, topbox,
		(char *)0);
	topbox = XtVaCreateManagedWidget("spinner",
		boxWidgetClass, tabs,
		XtNborderWidth, 0,
		(char *)0);
	spinner = XtVaCreateManagedWidget("spinner",
		mwSpinnerWidgetClass, topbox,
		(char *)0);
	XtAddCallback(spinner, XtNcallback, cb_spinner, (char *)0);
	spinner = XtVaCreateManagedWidget("spinner",
		mwSpinnerWidgetClass, topbox,
		XtNmin, -100, XtNstep, 10, XtNmax, 100,
		(char *)0);
	XtAddCallback(spinner, XtNcallback, cb_spinner, (char *)0);
	spinner = XtVaCreateManagedWidget("spinner",
		mwSpinnerWidgetClass, topbox,
		XtNmin, -1000, XtNstep, 20, XtNmax, 0,
		(char *)0);
	XtAddCallback(spinner, XtNcallback, cb_spinner, (char *)0);
	spinner = XtVaCreateManagedWidget("spinner",
		mwSpinnerWidgetClass, topbox,
		XtNmin, 0, XtNstep, 5, XtNmax, 1000,
		(char *)0);
	XtAddCallback(spinner, XtNcallback, cb_spinner, (char *)0);
	XtVaSetValues(spinner, XtNvalue, 10, (char *)0);
	topbox = XtVaCreateManagedWidget("textfield",
		boxWidgetClass, tabs,
		(char *)0);
	textfield = XtVaCreateManagedWidget("textfield",
		mwTextfieldWidgetClass, topbox,
		(char *)0);
	topbox = XtVaCreateManagedWidget("tabbing",
		mwRudegridWidgetClass, tabs,
		XtNyLayout, "100% 20",
		(char *)0);
	w = XtVaCreateManagedWidget("tabbing",
		labelWidgetClass, topbox,
		XtNlabel, "Click tab to rename",
		(char *)0);
	tabbing = XtVaCreateManagedWidget("tabbing",
		mwTabbingWidgetClass, topbox,
		XtNgridy, 1,
		(char *)0);
	MwTabbingInsert(tabbing, "Page 1", 1);
	MwTabbingInsert(tabbing, "Page 2", 2);
	XtAddCallback(tabbing, XtNselectCallback, select_tab, (XtPointer)w);
	table = XtVaCreateManagedWidget("table",
		mwTableWidgetClass, tabs,
		XtNtableText, table_text,
		(char *)0);
	tooltip = XtVaCreatePopupShell("tooltip",
		mwTooltipWidgetClass, topLevel,
		(char *)0);
	MwTooltipAdd(tooltip, textfield, "Ulric fibbar");
	XtRealizeWidget(topLevel);
	wm_protocols = XInternAtom(XtDisplay(topLevel),
				"WM_PROTOCOLS", False);
	wm_delete_window = XInternAtom(XtDisplay(topLevel),
				"WM_DELETE_WINDOW", False);
	XtOverrideTranslations(topLevel,
		XtParseTranslationTable(
			"<Message>WM_PROTOCOLS: quit()"));
	XSetWMProtocols(XtDisplay(topLevel), XtWindow(topLevel),
			&wm_delete_window, 1);
	XtAppMainLoop(ac);
	return 0;
}
