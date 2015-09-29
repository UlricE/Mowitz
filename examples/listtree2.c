#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "TreeFSel.h"

Widget	toplevel,tree;
XtAppContext app_con;

void
FileCallback(w,client,call)
Widget w;
XtPointer client;
XtPointer call;
{
TreeFSelReturnStruct *ret;

	ret=(TreeFSelReturnStruct *)call;
	if (ret->button==1) {
		printf("file: item=%s btn=%d\n",ret->pathname,ret->button);
	}
	else {
		XtUnmanageChild(w);
		printf("Cancelled.  file=%s\n",ret->pathname);
		exit(0);
	}
}

#ifdef USE_XPM
#include <X11/xpm.h>
static char * folder_xpm[] = {
"17 13 4 1",
"       c #FFFFFFFFFFFF",
".      c #FFFF00000000",
"X      c #000000000000",
"o      c #FFFFFFFF0000",
"        .....    ",
"       .......X  ",
"  oooooooooooooX ",
" ooooooooooooooX ",
" ooooooooooooooX ",
" ooooooooooooooX ",
" ooooooooooooooX ",
" ooooooooooooooX ",
" ooooooooooooooX ",
" ooooooooooooooX ",
" ooooooooooooooX ",
"  ooooooooooooX  ",
"   XXXXXXXXXXX   "};
static char * folderopen_xpm[] = {
"17 13 5 1",
"       c #FFFFFFFFFFFF",
".      c #FFFF00000000",
"X      c #000000000000",
"o      c #FFFFCCCC4242",
"O      c #FFFFFFFF0000",
"         .....   ",
"        .......X ",
"   oooooooooooooX",
"  ooooooooooooooX",
"  ooooooooooooooX",
" OOOOOOOOOOOooooX",
"OOOOOOOOOOOOOoooX",
" OOOOOOOOOOOOOooX",
" OOOOOOOOOOOOOOoX",
"  OOOOOOOOOOOOOoX",
"  OOOOOOOOOOOOOoX",
"   OOOOOOOOOOOOX ",
"    XXXXXXXXXXX  "};
#endif


int main (argc, argv)
int argc;
char **argv;
{
Pixmap open,closed;

	toplevel = XtAppInitialize(&app_con,"MwListTreeDemo",NULL,0,
				&argc,argv,NULL,NULL,0);

#ifdef USE_XPM
	XpmCreatePixmapFromData(XtDisplay(toplevel),
		RootWindowOfScreen(XtScreen(toplevel)),
		folderopen_xpm,&open,NULL,NULL);
	XpmCreatePixmapFromData(XtDisplay(toplevel),
		RootWindowOfScreen(XtScreen(toplevel)),
		folder_xpm,&closed,NULL,NULL);
#endif
	tree = XtVaCreateManagedWidget("fseltreedemo",treefselWidgetClass,toplevel,
                XtNheight,      (XtArgVal)500,
                XtNwidth,       (XtArgVal)500,
		XtNfont,        XLoadQueryFont(XtDisplay(toplevel),"-adobe-helvetica-medium-r-normal--*-120-*"),
		NULL,0);

	XtAddCallback(tree,XtNcallback,FileCallback,NULL);

	XtRealizeWidget(toplevel);

	XtAppMainLoop(app_con);
	return 0;
}
