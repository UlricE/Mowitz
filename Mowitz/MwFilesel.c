/*
    Copyright (C) 1996-2002  Ulric Eriksson <ulric@siag.nu>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston,
    MA 02111-1307, USA.
*/

/* filesel.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <dirent.h>

#ifdef HAVE_FNMATCH
#include <fnmatch.h>
#else
#ifdef HAVE_GMATCH
#include <libgen.h>
#define fnmatch(a,b,c) (!gmatch(b,a))
#else
#define fnmatch(a,b,c) 0		/* match anything */
#endif
#endif

#include <sys/stat.h>
#include <sys/param.h>
#include <unistd.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <X11/Xaw/Form.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Scrollbar.h>
#include <X11/Xaw/List.h>

#include "Mowitz.h"

static Widget fsel_pshell, fsel_form, fsel_topframe, fsel_bottomframe,
	fsel_topbox, fsel_bottombox, fsel_filelabel2, fsel_dirlabel2,
	fsel_fileframe, fsel_dirframe, fsel_textframe,
	fsel_filelabel, fsel_filetext, fsel_fileviewport, fsel_filelist,
	fsel_formatlabel, fsel_formatbutton, fsel_formatmenu,
	fsel_dirlabel, fsel_dirbutton, fsel_dirmenu,
	fsel_dirviewport, fsel_dirlist,
	fsel_okbutton, fsel_cancelbutton, fsel_findbutton,
	fsel_helpbutton, fsel_extra[10], below;

static int nextra;
static int ch_ext;

/* collect directory contents */

static int compar(const void *p, const void *q)
{
	return strcmp(*(const char **)p, *(const char **)q);
}

static int
getdirent(char ***ldirp, int *ndirp, char ***lfilep, int *nfilep,
	char *path, char *pattern)
{
	DIR *dird;
	struct dirent *dire;
	char fn[1024];
	struct stat buf;
	char **ldir = NULL, **lfile = NULL;
	int ndir = 0, mdir = 0, nfile = 0, mfile = 0;

	if ((dird = opendir(path)) == NULL) {
		fprintf(stderr, "Can't open %s\n", path);
		return 1;
	}
	while ((dire = readdir(dird))) {
		sprintf(fn, "%s/%s", path, dire->d_name);
		if ((stat(fn, &buf))) {
			fprintf(stderr, "Can't stat %s\n", fn);
			continue;
		}
		if (S_IFDIR & buf.st_mode) {
			if (ndir >= mdir)
				ldir = MwRealloc(ldir,
					(mdir += 256)*sizeof(char *));
			ldir[ndir++] = MwStrdup(dire->d_name);
		}
		else if (S_IFREG & buf.st_mode && 
				!fnmatch(pattern, dire->d_name, 0)) {
			if (nfile >= mfile)
				lfile = MwRealloc(lfile,
					(mfile += 256)*sizeof(char *));
			lfile[nfile++] = MwStrdup(dire->d_name);
		}
	}
	closedir(dird);

	qsort(ldir, ndir, sizeof(char *), compar);
	qsort(lfile, nfile, sizeof(char *), compar);

	*ndirp = ndir;
	*ldirp = ldir;
	*nfilep = nfile;
	*lfilep = lfile;

	return 0;
}

static void
freedirent(char **ldir, int ndir, char **lfile, int nfile)
{
	int i;

	for (i = 0; i < ndir; i++)
		MwFree(ldir[i]);
	MwFree(ldir);
	for (i = 0; i < nfile; i++)
		MwFree(lfile[i]);
	MwFree(lfile);
}

/* and that's all there is to it */


/* Foolproof default format */
static char *fileformats[] = {
	"All (*)",
	NULL};

static char **files, **dirs;
static int nfiles, ndirs;

static void make_files(String files[], Cardinal nfiles)
{
	static String foo[1] = {"<none>"};

	if (nfiles == 0)
		XawListChange(fsel_filelist,
			foo, 1, 0, True);
	else
		XawListChange(fsel_filelist,
			files, nfiles, 0, True);
}

static void make_dirs(String dirs[], Cardinal ndirs)
{
	XawListChange(fsel_dirlist,
		dirs, ndirs, 0, True);
}

static void make_dirmenu(char *);

static void fsel_scan(void)
{
	String string;
	char dir[1024];
	char pattern[1024];
	char *pst;

	if (files != NULL)
		freedirent(dirs, ndirs, files, nfiles);
	
	strcpy(dir, MwLabelGet(fsel_dirbutton));
	string = MwLabelGet(fsel_formatbutton);
	pst = strchr(string, '(');
	if (pst == NULL) {
		/* apparently no pattern here; use foolproof default */
		strcpy(pattern, "*");
	} else {
		strcpy(pattern, pst+1);
		if ((pst = strchr(pattern, ')')))
			*pst = '\0';
	}
	if (getdirent(&dirs, &ndirs, &files, &nfiles, dir, pattern)) {
		fprintf(stderr, "Grmbl. getdirent() failed\n");
	}
	make_files(files, nfiles);
	make_dirs(dirs, ndirs);
	make_dirmenu(dir);
}

/* ---
A format pattern looks like this:
	Plaintext description of foo format (*.foo)
We want to return .foo
*/
static char *get_extension(char *f, char *x)
{
	int n;
	char *p = strrchr(f, '(');
	if (p == NULL) return NULL;
	p = strchr(p, '.');
	if (p == NULL) return NULL;
	strcpy(x, p);
	n = strcspn(x, ")[]*");
	x[n] = '\0';
	return x;
}

static void format_select(Widget w, XtPointer client_data, XtPointer call_data)
{
	static char filename[1024];
	char *p, *x, ext[100];
	strncpy(filename, (char *)client_data, sizeof filename);

	if (ch_ext) {
		p = MwTextFieldGetString(fsel_filetext);
		strncpy(filename, p, sizeof filename);
		p = strrchr(filename, '.');
		x = get_extension((char *)client_data, ext);
		if (p && x) {
			strcpy(p, x);
			XtVaSetValues(fsel_filetext,
				XtNstring, filename,
				(char *)0);
		}
	}
	MwLabelSet(fsel_formatbutton, (String)client_data);
	fsel_scan();
}

static void file_select(Widget w, XtPointer client_data, XtPointer call_data)
{
	XawListReturnStruct *list_struct = (XawListReturnStruct *)call_data;
	String string = list_struct->string;

	XtVaSetValues(fsel_filetext,
		XtNstring, string, (char *)0);
	XawListUnhighlight(fsel_filelist);
	XawListUnhighlight(fsel_dirlist);
}

static void dir_select(Widget w, XtPointer client_data, XtPointer call_data)
{
	char path[1024];
	char newpath[1024];

	XawListReturnStruct *list_struct = (XawListReturnStruct *)call_data;
	String string = list_struct->string;

	sprintf(path, "%s/%s", MwLabelGet(fsel_dirbutton), string);

	if (!realpath(path, newpath))
		fprintf(stderr, "Couldn't realpath %s\n", path);

	MwLabelSet(fsel_dirbutton, newpath);

	XawListUnhighlight(fsel_filelist);
	XawListUnhighlight(fsel_dirlist);
	fsel_scan();
}

static void make_menu(char **menu_entry_names, Widget menu)
{
	int i;
	Widget entry;

	for (i = 0; menu_entry_names[i]; i++) {
		String item = menu_entry_names[i];

		if (item[0] == '-')	/* line pane */
			entry = XtVaCreateManagedWidget(item,
				mwLineMEObjectClass, menu,
				(char *)0);
		else {
			entry = XtVaCreateManagedWidget(item,
				mwLabelMEObjectClass, menu,
				XtNlabel, item,
				(char *)0);
			XtAddCallback(entry,
				XtNcallback, format_select,
				menu_entry_names[i]);
		}
	}
}

static int nentry = 0;

static struct {
	Widget w;
	char *d;
} fsel_dir_entry[20];

static void change_dir(Widget w, XtPointer client_data, XtPointer call_data)
{
	char path[1024];
	char newpath[1024];

	strcpy(path, (char *)client_data);

	if (!realpath(path, newpath))
		fprintf(stderr, "Couldn't realpath %s\n", path);

	MwLabelSet(fsel_dirbutton, newpath);

	fsel_scan();
}

static void make_dirmenu(char *dir)
{
        int i;
	char *p, *b, *item;

	b = MwStrdup(dir);
	for (i = 0; i < nentry; i++) {
		if (fsel_dir_entry[i].w != None) {
			XtDestroyWidget(fsel_dir_entry[i].w);
			MwFree(fsel_dir_entry[i].d);
		}
	}
	nentry = 0;

	XtVaSetValues(fsel_dirbutton,
		XtNwidth, 200, (char *)0);
	if (fsel_dirmenu != None)
		XtDestroyWidget(fsel_dirmenu);
	fsel_dirmenu = XtVaCreatePopupShell("fsel_dirmenu",
		mwMenuWidgetClass, XtParent(fsel_dirbutton),
		(char *)0);

	while ((p = strrchr(b, '/'))) {
		*p = '\0';
		if (p == b)	/* root dir */
			item = MwStrdup("/");
		else
			item = MwStrdup(b);
		fsel_dir_entry[nentry].d = item;

		fsel_dir_entry[nentry].w = XtVaCreateManagedWidget(item,
			mwLabelMEObjectClass, fsel_dirmenu,
			XtNlabel, item,
			(char *)0);
		XtAddCallback(fsel_dir_entry[nentry].w,
			XtNcallback, change_dir,
			fsel_dir_entry[nentry].d);
		nentry++;
	}
}

static int status;

static void fsel_done(Widget w, XtPointer client_data, XtPointer call_data)
{
	char path[1024], newpath[1024];
	struct stat buf;
	String string;
	XtVaGetValues(fsel_filetext,
		XtNstring, &string, (char *)0);
	if (string[0] == '/') strcpy(path, string);
	else sprintf(path, "%s/%s", MwLabelGet(fsel_dirbutton), string);
	realpath(path, newpath);
	if (!stat(newpath, &buf) &&
		(S_IFDIR & buf.st_mode)) {
		MwLabelSet(fsel_dirbutton, newpath);
		XtVaSetValues(fsel_filetext,
			XtNstring, "", (char *)0);
		fsel_scan();
	} else {
		XtPopdown(fsel_pshell);
		status = MW_DONE;
        }
}

static void fsel_abort(Widget w, XtPointer client_data, XtPointer call_data)
{
	XtPopdown(fsel_pshell);
	status = MW_ABORT;
}

static void fsel_done_action(Widget w, XEvent *event, String *params, Cardinal *n)
{
	fsel_done(w, NULL, NULL);
}

static void fsel_cancel_action(Widget w, XEvent *event,
		String *params, Cardinal *n)
{
	fsel_abort(w, NULL, NULL);
}

static XtActionsRec actions[] =
{
	{"fsel-done", fsel_done_action},
	{"fsel-cancel", fsel_cancel_action}
};

/* ---
*/
static Widget add_button(char *name, char *label,
	void (*cb)(Widget, XtPointer, XtPointer), XtPointer data, Widget pw)
{
	below = XtVaCreateManagedWidget(name,
		commandWidgetClass, pw,
		XtNwidth, 80,
		(char *)0);
	MwLabelSet(below, label);
	XtVaSetValues(below, XtNwidth, 80, (char *)0);
	if (cb) XtAddCallback(below, XtNcallback, cb, data);
	return below;
}

static void fsel_find(Widget w, XtPointer client_data, XtPointer call_data)
{
	char fn[1024];
	char cmd[1024];	
	char **files = NULL;
	int nfiles = 0, i;
	FILE *fp;

	fn[0] = '\0';
	if (MwDialogInput(fsel_pshell, "Find pattern:", fn)) {
		if (fn[0] == '!') {
			sprintf(cmd, "find %s %s -print",
				MwLabelGet(fsel_dirbutton), fn+1);
		} else {
			sprintf(cmd, "find %s -name '%s' -print",
				MwLabelGet(fsel_dirbutton), fn);
		}
		if (!(fp = popen(cmd, "r"))) {
			MwErrorBox(fsel_pshell, "Can't exec find");
			return;
		}
		while ((fgets(fn, sizeof fn, fp))) {
			MwChomp(fn);
			files = MwRealloc(files, (nfiles+1)*(sizeof(char *)));
			files[nfiles++] = MwStrdup(fn);
		}
		pclose(fp);
		if (nfiles) {
			int n = MwListBox(fsel_pshell,
				"Pick one:", files, nfiles);
			if (n != -1) {
				char *p = strrchr(files[n], '/');
				if (p) {
					*p = '\0';
					XtVaSetValues(fsel_filetext,
						XtNstring, p+1, (char *)0);
					change_dir(w,
						(XtPointer)files[n], NULL);
				} else {
					XtVaSetValues(fsel_filetext,
						XtNstring, files[n], (char *)0);
				}
			}
		} else {
			MwErrorBox(fsel_pshell, "No files found");
		}
		for (i = 0; i < nfiles; i++)
			MwFree(files[i]);
		MwFree(files);
	}
}

static void fsel_help(Widget w, XtPointer client_data, XtPointer call_data)
{
	MwHelp("Filesel.html");
}

static Atom wm_delete_window;

static void fsel_init(Widget topLevel)
{
	XtAppContext app_context = XtWidgetToApplicationContext(topLevel);
	XtAppAddActions(app_context, actions, XtNumber(actions));

	fsel_pshell = XtVaCreatePopupShell("fsel_pshell",
		transientShellWidgetClass, topLevel,
		XtNtitle, _("Select File"),
		XtNwidth, 400,
		XtNheight, 400,
		(char *)0);

	fsel_form = XtVaCreateManagedWidget("fsel_form",
		mwRudegridWidgetClass, fsel_pshell,
		XtNxLayout, "5 100 5 50% 5 50% 5 100 5",
		XtNyLayout, "35 5 25 5 25 5 100% 5 22 5 32",
		(char *)0);

	fsel_topframe = XtVaCreateManagedWidget("fsel_topframe",
		mwFrameWidgetClass, fsel_form,
		XtNgridWidth, 9,
		XtNshadowWidth, 1,
		XtNshadowType, Raised,
		(char *)0);
	fsel_topbox = XtVaCreateManagedWidget("fsel_topbox",
		boxWidgetClass, fsel_topframe,
		XtNborderWidth, 0,
		(char *)0);
	fsel_bottomframe = XtVaCreateManagedWidget("fsel_bottomframe",
		mwFrameWidgetClass, fsel_form,
		XtNgridy, 10,
		XtNgridWidth, 9,
		XtNshadowWidth, 1,
		XtNshadowType, Raised,
		(char *)0);
	fsel_bottombox = XtVaCreateManagedWidget("fsel_bottombox",
		boxWidgetClass, fsel_bottomframe,
		XtNborderWidth, 0,
		(char *)0);
	fsel_filelabel = XtVaCreateManagedWidget("fsel_filelabel",
		labelWidgetClass, fsel_form,
		XtNjustify, XtJustifyLeft,
		XtNgridx, 1,
		XtNgridy, 8,
		(char *)0);
	MwLabelSet(fsel_filelabel, "File Name:");

	fsel_textframe = XtVaCreateManagedWidget("fsel_textframe",
		mwFrameWidgetClass, fsel_form,
		XtNgridx, 3,
		XtNgridy, 8,
		XtNgridWidth, 5,
		XtNallowResize, False,
		XtNshadowType, Lowered,
		XtNshadowWidth, 1,
		(char *)0);
	fsel_filetext = XtVaCreateManagedWidget("fsel_filetext",
		mwTextfieldWidgetClass, fsel_textframe,
		XtNborderWidth, 0,
		(char *)0);
	XtVaSetValues(fsel_filetext,
		XtNstring, "", (char *)0);

	fsel_fileframe = XtVaCreateManagedWidget("fsel_fileframe",
		mwFrameWidgetClass, fsel_form,
		XtNgridx, 5,
		XtNgridy, 6,
		XtNgridWidth, 3,
		XtNshadowType, Groove,
		XtNmarginWidth, 2,
		XtNmarginHeight, 2,
		XtNallowResize, False,
		(char *)0);
	fsel_filelabel2 = XtVaCreateManagedWidget("fsel_filelabel2",
		labelWidgetClass, fsel_fileframe, (char *)0);
	MwLabelSet(fsel_filelabel2, "Files");
	XtVaSetValues(fsel_fileframe,
		XtNtitle, fsel_filelabel2,
		(char *)0);
	fsel_fileviewport = XtVaCreateManagedWidget("fsel_fileviewport",
		viewportWidgetClass, fsel_fileframe,
		XtNallowHoriz, True,
		XtNallowVert, True,
		XtNuseBottom, True,
		XtNuseRight, True,
		XtNforceBars, True,
		XtNborderWidth, 0,
		(char *)0);
	fsel_filelist = XtVaCreateManagedWidget("fsel_filelist",
		listWidgetClass, fsel_fileviewport,
		XtNdefaultColumns, 1,
		XtNforceColumns, 1,
		(char *)0);

	XtAddCallback(fsel_filelist, XtNcallback, file_select, NULL);

	fsel_formatlabel = XtVaCreateManagedWidget("fsel_formatlabel",
		labelWidgetClass, fsel_form,
		XtNjustify, XtJustifyLeft,
		XtNgridx, 1,
		XtNgridy, 2,
		(char *)0);
	MwLabelSet(fsel_formatlabel, "Format:");

	fsel_formatbutton = XtVaCreateManagedWidget("fsel_formatbutton",
		mwMenuButtonWidgetClass, fsel_form,
		XtNmenu_name, "fsel_formatmenu",
		XtNlabel, fileformats[0],
		XtNgridx, 3,
		XtNgridy, 2,
		XtNgridWidth, 5,
		XtNjustify, XtJustifyLeft,
		XtNshadowWidth, 1,
		(char *)0);

	fsel_dirlabel = XtVaCreateManagedWidget("fsel_dirlabel",
		labelWidgetClass, fsel_form,
		XtNjustify, XtJustifyLeft,
		XtNgridx, 1,
		XtNgridy, 4,
		(char *)0);
	MwLabelSet(fsel_dirlabel, "Directory:");

	fsel_dirbutton = XtVaCreateManagedWidget("fsel_dirbutton",
		mwMenuButtonWidgetClass, fsel_form,
		XtNmenu_name, "fsel_dirmenu",
		XtNgridx, 3,
		XtNgridy, 4,
		XtNgridWidth, 5,
		XtNjustify, XtJustifyLeft,
		XtNshadowWidth, 1,
		(char *)0);

	fsel_dirframe = XtVaCreateManagedWidget("fsel_dirframe",
		mwFrameWidgetClass, fsel_form,
		XtNgridx, 1,
		XtNgridy, 6,
		XtNgridWidth, 3,
		XtNshadowType, Groove,
		XtNmarginWidth, 2,
		XtNmarginHeight, 2,
		XtNallowResize, False,
		(char *)0);
	fsel_dirlabel2 = XtVaCreateManagedWidget("fsel_dirlabel2",
		labelWidgetClass, fsel_dirframe, (char *)0);
	MwLabelSet(fsel_dirlabel2, "Directories");
	XtVaSetValues(fsel_dirframe,
		XtNtitle, fsel_dirlabel2,
		(char *)0);
	fsel_dirviewport = XtVaCreateManagedWidget("fsel_dirviewport",
		viewportWidgetClass, fsel_dirframe,
		XtNallowHoriz, True,
		XtNallowVert, True,
		XtNuseBottom, True,
		XtNuseRight, True,
		XtNforceBars, True,
		XtNborderWidth, 0,
		(char *)0);

	fsel_dirlist = XtVaCreateManagedWidget("fsel_dirlist",
		listWidgetClass, fsel_dirviewport,
		XtNdefaultColumns, 1,
		XtNforceColumns, 1,
		(char *)0);

	XtAddCallback(fsel_dirlist, XtNcallback, dir_select, NULL);

	below = None;

	fsel_okbutton = add_button("fsel_okbutton", "OK",
		fsel_done, NULL, fsel_bottombox);
	fsel_cancelbutton = add_button("fsel_cancelbutton", "Cancel",
		fsel_abort, NULL, fsel_bottombox);
	fsel_findbutton = add_button("fsel_findbutton", "Find",
		fsel_find, NULL, fsel_topbox);
	fsel_helpbutton = add_button("fsel_helpbutton", "Help",
		fsel_help, NULL, fsel_bottombox);

	wm_delete_window = XInternAtom(XtDisplay(fsel_pshell),
				"WM_DELETE_WINDOW", False);
	XtOverrideTranslations(fsel_pshell,
		XtParseTranslationTable(
			"<Message>WM_PROTOCOLS: fsel-cancel()"));
}

static void add_extra(char *extra)
{
	char *p, *q;

	nextra = 0;
	if (!extra) return;
	below = fsel_helpbutton;
	for (p = strtok(extra, ":"); p; p = strtok(NULL, ":")) {
		q = strchr(p, '=');
		if (!q) continue;
		*q++ = '\0';
		fsel_extra[nextra] = add_button("fsel_extra", p,
			change_dir, q, fsel_topbox);
		XtVaSetValues(fsel_extra[nextra++],
			XtNwidth, 80, (char *)0);
	}
}

/* ---
Select a file and directory. The path and name must contain valid strings.
The format is returned in fmt. Extra is a list of extra directory buttons
to appear on the top row. If ext != 0, change extension of file name
when format pattern is changed.
*/

int MwFileselInput(Widget pw, char *path, char *name, char **patterns,
	char *fmt, char *extra, int ext)
{
	String string;
	XtAppContext app_context = XtWidgetToApplicationContext(pw);
	int i;
	char newpath[PATH_MAX];

	if (!realpath(path, newpath))
		getcwd(newpath, sizeof newpath);
	strcpy(path, newpath);

	if (fsel_pshell == None) fsel_init(pw);

	if (patterns == NULL)
		patterns = fileformats;

	/* First, create all the formats on the menu */
	XtVaSetValues(fsel_formatbutton,
		XtNlabel, patterns[0],
		XtNwidth, 200, (char *)0);
	fsel_formatmenu = XtVaCreatePopupShell("fsel_formatmenu",
		mwMenuWidgetClass, XtParent(fsel_formatbutton),
		(char *)0);

	make_menu(patterns, fsel_formatmenu);
	ch_ext = ext;

	MwLabelSet(fsel_dirbutton, path);
	XtVaSetValues(fsel_filetext, XtNstring, name, (char *)0);
	add_extra(extra);
	status = MW_WAITING;

	MwCenter(fsel_pshell);
	XtPopup(fsel_pshell, XtGrabNonexclusive);
	XSetWMProtocols(XtDisplay(fsel_pshell), XtWindow(fsel_pshell),
			&wm_delete_window, 1);
	fsel_scan();

	XtSetKeyboardFocus(fsel_pshell, fsel_filetext);

	while (status == MW_WAITING) {
		XEvent event_return;
		XtAppNextEvent(app_context, &event_return);
		XtDispatchEvent(&event_return);
	}

	/* And get rid of the formats */
	XtDestroyWidget(fsel_formatmenu);
	make_dirmenu("");
	XtDestroyWidget(fsel_dirmenu);
	fsel_dirmenu = None;
	for (i = 0; i < nextra; i++)
		XtDestroyWidget(fsel_extra[i]);

	XtVaGetValues(fsel_filetext, XtNstring, &string, (char *)0);
	strcpy(name, string);
	strcpy(path, MwLabelGet(fsel_dirbutton));
	strcpy(fmt, MwLabelGet(fsel_formatbutton));
	return status;
}

