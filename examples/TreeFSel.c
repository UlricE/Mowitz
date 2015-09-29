/*-----------------------------------------------------------------------------
** TreeFSel.c	A File Selector widget using the ListTree widget
**
** Widget source code
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

#define _TreeFileSelector_

#include "Directory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/List.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/AsciiText.h>
#include <Mowitz/MwListTree.h>
#include "TreeFSelP.h"

#define INITIAL_NUM_FILES	256

#define folder_width 16
#define folder_height 12
static char folder_bits[] = {
  0x00, 0x1f, 0x80, 0x20, 0x7c, 0x5f, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40,
  0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0xfc, 0x3f,
  };
#define folderopen_width 16
#define folderopen_height 12
static char folderopen_bits[] = {
  0x00, 0x3e, 0x00, 0x41, 0xf8, 0xd5, 0xac, 0xaa, 0x54, 0xd5, 0xfe, 0xaf,
  0x01, 0xd0, 0x02, 0xa0, 0x02, 0xe0, 0x04, 0xc0, 0x04, 0xc0, 0xf8, 0x7f,
  };

#define offset(field) XtOffsetOf(TreeFSelRec, fsel.field)
static XtResource resources[] = {
	{XtNforeground, XtCForeground, XtRPixel, sizeof (Pixel),
		offset(foreground_pixel), XtRString, XtDefaultForeground},
	{XtNmargin, XtCMargin, XtRDimension, sizeof(Dimension),
		offset(Margin), XtRString, "2"},
	{XtNspacing, XtCMargin, XtRDimension, sizeof(Dimension),
		offset(Spacing), XtRString, "2"},
	{XtNindent, XtCMargin, XtRDimension, sizeof(Dimension),
		offset(Indent), XtRString, "0"},
	{XtNlineWidth, XtCMargin, XtRDimension, sizeof(Dimension),
		offset(LineWidth), XtRString, "0"},
	{XtNfont,  XtCFont, XtRFontStruct, sizeof(XFontStruct *),
		offset(font), XtRString, XtDefaultFont},
	{XtNdirPixmap, XtCPixmap, XtRBitmap, sizeof(Pixmap),
		offset(Dir), XtRImmediate, (XtPointer)XtUnspecifiedPixmap },
	{XtNdirOpenPixmap, XtCPixmap, XtRBitmap, sizeof(Pixmap),
		offset(DirOpen), XtRImmediate, (XtPointer)XtUnspecifiedPixmap },
	{XtNcallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
		offset(Callback), XtRCallback, NULL},
};
#undef offset

static void Initialize();
static void Realize();
static void Resize();
static Boolean SetValues();
static XtGeometryResult GeometryManager();
static void Destroy();
static void PathCallback();
static void FileCallback();
static void OkCallback();
static void CancelCallback();
static void InitTree();

TreeFSelClassRec treefselClassRec = {
	{
	/* core_class fields	 */
	/* superclass	  	 */ (WidgetClass) &compositeClassRec,
	/* class_name	  	 */ "TreeFSel",
	/* widget_size	  	 */ sizeof(TreeFSelRec),
	/* class_initialize   	 */ NULL,
	/* class_part_initialize */ NULL,
	/* class_inited       	 */ False,
	/* initialize	  	 */ Initialize,
	/* initialize_hook	 */ NULL,
	/* realize		 */ Realize,
	/* actions		 */ NULL,
	/* num_actions	  	 */ 0,
	/* resources	  	 */ resources,
	/* num_resources	 */ XtNumber(resources),
	/* xrm_class	  	 */ NULLQUARK,
	/* compress_motion	 */ True,
	/* compress_exposure  	 */ XtExposeCompressMultiple,
	/* compress_enterleave	 */ True,
	/* visible_interest	 */ True,
	/* destroy		 */ Destroy,
	/* resize		 */ Resize,
	/* expose		 */ XtInheritExpose,
	/* set_values	  	 */ SetValues,
	/* set_values_hook	 */ NULL,
	/* set_values_almost	 */ XtInheritSetValuesAlmost,
	/* get_values_hook	 */ NULL,
	/* accept_focus	 	 */ NULL,
	/* version		 */ XtVersion,
	/* callback_private   	 */ NULL,
	/* tm_table		 */ NULL,
	/* query_geometry	 */ XtInheritQueryGeometry,
	/* display_accelerator   */ XtInheritDisplayAccelerator,
	/* extension             */ NULL
	}, /* Core part */
	{
	/* geometry_manager      */ GeometryManager,
	/* change_managed        */ XtInheritChangeManaged,
	/* insert_child          */ XtInheritInsertChild,
	/* delete_child          */ XtInheritDeleteChild,
	/* extension             */ NULL
	}, /* Composite Part */		
	{
	0 /* some stupid compilers barf on empty structures */
	},
};

WidgetClass treefselWidgetClass = (WidgetClass) & treefselClassRec;

void
Initialize(treq,tnew,args,num)
Widget treq,tnew;
ArgList args;
Cardinal *num;
{
	TreeFSelWidget new;

	new=(TreeFSelWidget) tnew;
	
	if (treq->core.width<=200) new->core.width=200;
	if (treq->core.height<=300) new->core.height=300;

	new->fsel.mainpane = XtVaCreateManagedWidget("paned",
		panedWidgetClass,(Widget)new,
                XtNorientation,		XtorientVertical,
		NULL,0);

	new->fsel.showdir = XtVaCreateManagedWidget("titlelabel",
		labelWidgetClass,new->fsel.mainpane,
		XtNfont,		new->fsel.font,
		XtNshowGrip,		False,
		XtNjustify,		XtJustifyLeft,
		NULL, 0);
    
	new->fsel.paned = XtVaCreateManagedWidget("paned",
		panedWidgetClass,new->fsel.mainpane,
                XtNorientation,		XtorientHorizontal,
		NULL,0);

	new->fsel.form = XtVaCreateManagedWidget("form",
		viewportWidgetClass,new->fsel.paned,
		XtNallowVert,		True,
		XtNallowHoriz,		True,
		XtNforceBars,		True,
                XtNuseBottom,		True,
		NULL,0);

	if (new->fsel.Dir == XtUnspecifiedPixmap)
		new->fsel.Dir=XCreateBitmapFromData(XtDisplay((Widget)new),
			RootWindowOfScreen(XtScreen((Widget)new)),
			folder_bits,folder_width,folder_height);
	if (new->fsel.DirOpen == XtUnspecifiedPixmap)
		new->fsel.DirOpen=XCreateBitmapFromData(XtDisplay((Widget)new),
			RootWindowOfScreen(XtScreen((Widget)new)),
			folderopen_bits,folderopen_width,folderopen_height);
	new->fsel.tree = XtVaCreateManagedWidget("form",
		listtreeWidgetClass,new->fsel.form,
		XtNheight,      	new->core.height/2,
		XtNwidth,		new->core.width/3,
		XtNforeground,		new->fsel.foreground_pixel,
		XtNmargin,		new->fsel.Margin,
		XtNspacing,		new->fsel.Spacing,
		XtNindent,		new->fsel.Indent,
		XtNlineWidth,		new->fsel.LineWidth,
		XtNfont,		new->fsel.font,
		XtNbranchPixmap,	new->fsel.Dir,
		XtNbranchOpenPixmap,	new->fsel.DirOpen,
		XtNleafPixmap,		new->fsel.Dir,
		XtNleafOpenPixmap,	new->fsel.DirOpen,
		NULL,0);
	
	new->fsel.form = XtVaCreateManagedWidget("form",
		viewportWidgetClass,new->fsel.paned,
		XtNallowVert,		True,
		XtNforceBars,		True,
		XtNuseBottom,		True,
		XtNuseRight,		True,
		NULL,0);
	
	new->fsel.list=XtVaCreateManagedWidget("scrolledList",
		listWidgetClass,new->fsel.form,
		XtNfont,		new->fsel.font,
		XtNlist,        	NULL,
		XtNnumberStrings,       0,
		XtNdefaultColumns,      1,
		XtNinternalHeight,	new->fsel.Margin,
		XtNinternalWidth,	new->fsel.Margin,
		XtNcolumnSpacing,	6,
		XtNrowSpacing,		0,
		XtNverticalList,	True,
		NULL,0);
	
	new->fsel.box1 = XtVaCreateManagedWidget("box",
		formWidgetClass,new->fsel.mainpane,
		XtNshowGrip,		False,
		XtNresizeToPreferred,	True,
		NULL,0);
	new->fsel.sel = XtVaCreateManagedWidget("label",
		labelWidgetClass,new->fsel.box1,
		XtNlabel,		"Selection :",
		XtNborderWidth,		0,
		XtNleft,		XawChainLeft,
		XtNright,		XawChainLeft,
		NULL,0);
	new->fsel.sel = XtVaCreateManagedWidget("text",
		asciiTextWidgetClass,new->fsel.box1,
		XtNfont,		new->fsel.font,
		XtNsensitive,		True,
		XtNeditType,		XawtextEdit,
		XtNfromHoriz,		new->fsel.sel,
		XtNleft,		XawChainLeft,
		XtNright,		XawChainRight,
		NULL,0);
	
	new->fsel.box2 = XtVaCreateManagedWidget("box2",
		boxWidgetClass,new->fsel.mainpane,
                XtNorientation,		XtorientHorizontal,
		XtNshowGrip,		False,
		XtNresizeToPreferred,	True,
		NULL,0);
	new->fsel.okbtn = XtVaCreateManagedWidget("ok",
		commandWidgetClass,new->fsel.box2,
		XtNlabel,		"  OK  ",
		NULL,0);
	new->fsel.cancelbtn = XtVaCreateManagedWidget("cancel",
		commandWidgetClass,new->fsel.box2,
		XtNlabel,		"Cancel",
		NULL,0);

	new->fsel.files=NULL;
	new->fsel.files_alloc=0;
	new->fsel.filecount=0;
	strcpy(new->fsel.SelectedFile,"");

	XtAddCallback(new->fsel.tree,XtNpathCallback,PathCallback,(XtPointer)new);
	XtAddCallback(new->fsel.list,XtNcallback,FileCallback,(XtPointer)new);
	XtAddCallback(new->fsel.okbtn,XtNcallback,OkCallback,(XtPointer)new);
	XtAddCallback(new->fsel.cancelbtn,XtNcallback,CancelCallback,(XtPointer)new);
}

static void
Realize(gw,valueMask,attrs)
Widget gw;
XtValueMask *valueMask;
XSetWindowAttributes *attrs;
{
TreeFSelWidget w;
Dimension height;

	w=(TreeFSelWidget)gw;
	XtCreateWindow(gw,InputOutput,(Visual *)CopyFromParent,
		*valueMask,attrs);
	InitTree(w);
	XtVaGetValues(w->fsel.box1,
		XtNheight,	&height,
		NULL);
	XawPanedSetMinMax(w->fsel.box1,(int)height,(int)height);
	XtVaGetValues(w->fsel.box2,
		XtNheight,	&height,
		NULL);
	XawPanedSetMinMax(w->fsel.box2,(int)height,(int)height);
	XtResizeWidget(w->fsel.mainpane,w->core.width,w->core.height,(Dimension)0);
}

static void
Resize(Widget current)
{
TreeFSelWidget w = (TreeFSelWidget)current;

	if (!XtIsRealized(current)) return;
	XtResizeWidget(w->fsel.mainpane,w->core.width,w->core.height,(Dimension)0);
}

static Boolean SetValues(current, request, new, args, nargs)
Widget current, request, new;
ArgList args;
Cardinal *nargs;
{
	return XtIsRealized(current);
}

static XtGeometryResult
GeometryManager(w,request,reply)
Widget w;
XtWidgetGeometry *request;
XtWidgetGeometry *reply;
{
	return(XtGeometryYes);
}

static void
Destroy(TreeFSelWidget w)
{
	return;
}



static void
ClearFiles(TreeFSelWidget w)
{
int i;

	for (i=0; i<w->fsel.filecount; i++) free(w->fsel.files[i]);
	w->fsel.filecount=0;
}

static int
AlphabetizeFiles(const void *item1,const void *item2)
{
	return strcmp(*(char **)item1,*(char **)item2);
}

int
RevAlphaItems(const void *item1,const void *item2)
{
	return -strcmp((*((MwListTreeItem **)item1))->text,
		      (*((MwListTreeItem **)item2))->text);
}

static void
SortFiles(TreeFSelWidget w)
{
	qsort(w->fsel.files,w->fsel.filecount,sizeof(char *),AlphabetizeFiles);
}

static char *
StoreFile(TreeFSelWidget w,char *name)
{
char *file;

	if (w->fsel.filecount+1>w->fsel.files_alloc) {
		w->fsel.files_alloc+=INITIAL_NUM_FILES;
		w->fsel.files=(char **)realloc((char *)w->fsel.files,
			w->fsel.files_alloc*sizeof(char *));
	}
	file=(char *)malloc(strlen(name)+1);
	strcpy(file,name);
	w->fsel.files[w->fsel.filecount++]=file;
	return file;
}

static void
ShowFiles(TreeFSelWidget w)
{
	if (w->fsel.filecount>0)
		XawListChange(w->fsel.list,w->fsel.files,w->fsel.filecount,0,True);
	else
		XawListChange(w->fsel.list,NULL,0,0,True);
}

static MwListTreeItem *
NewDir(TreeFSelWidget w,char *dir,MwListTreeItem *parent)
{
DirEntry entry;
Directory directory;
char *name;

	if (!DirectoryOpen(dir,&directory)) {
		fprintf(stderr,"Can't open directory '%s'\n",dir);
		return NULL;
	}
	strcpy(w->fsel.WorkingDir,DirectoryPath(&directory));
	ClearFiles(w);
		
	MwListTreeRefreshOff((Widget)w->fsel.tree);

	while (DirectoryReadNextEntry(&directory,&entry)) {
		name=DirEntryFileName(&entry);
		switch(DirEntryType(&entry)) {
		case F_TYPE_DIR:
			if (strcmp(name,".")!=0 && strcmp(name,"..")!=0) {
				if (!MwListTreeFindChildName((Widget)w->fsel.tree,parent,name))
					MwListTreeAdd((Widget)w->fsel.tree,parent,name);
			}
			break;
		default:
			StoreFile(w,name);
			break;
		}
/*		printf("<Type %5d,  File '%20s',  Mode %3d,  Size %d>\n",*/
/*		       DirEntryType(&entry),*/
/*		       DirEntryFileName(&entry),*/
/*		       DirEntryProt(&entry),*/
/*		       DirEntryFileSize(&entry));*/
	}
	strcpy(w->fsel.CurrentDir,DirectoryPath(&directory));
	DirectoryClose(&directory);
	XtVaSetValues(w->fsel.showdir,XtNlabel,w->fsel.CurrentDir,NULL);
#ifdef REVERSE_LIST
	MwListTreeUserOrderChildren((Widget)w->fsel.tree,parent,RevAlphaItems);
#else
	MwListTreeOrderChildren((Widget)w->fsel.tree,parent);
#endif
	return parent;
}

static void
InitTree(TreeFSelWidget w)
{
Directory directory;
MwListTreeItem *dummy,*item,*first;
char *path,*loc,name[256];

	dummy=NewDir(w,"/",NULL);
	first=MwListTreeFirstItem((Widget)w->fsel.tree);
	if (!DirectoryOpen(".",&directory)) {
		fprintf(stderr,"Can't open current directory\n");
		return;
	}
	strcpy(w->fsel.CurrentDir,"/");
	strcpy(w->fsel.WorkingDir,DirectoryPath(&directory));
	path=w->fsel.WorkingDir;
	while (*path && *path=='/') path++;
	while (*path) {
		strcpy(name,path);
		path=name;
		while (*path && *path!='/') path++;
		loc=path;
		if (*loc=='/') loc++;
		*path='\0';
		
		path=name;
/*		printf("path: %s\n",path);*/
		item=MwListTreeFindSiblingName((Widget)w->fsel.tree,first,path);
		if (item) {
			strcat(w->fsel.CurrentDir,path);
			strcat(w->fsel.CurrentDir,"/");
			NewDir(w,w->fsel.CurrentDir,item);
			item->open=True;
			first=item->firstchild;
		}
		
		path=loc;
	}
	strcpy(w->fsel.CurrentDir,DirectoryPath(&directory));
	DirectoryClose(&directory);
	XtVaSetValues(w->fsel.showdir,XtNlabel,w->fsel.CurrentDir,NULL);
	SortFiles(w);
	ShowFiles(w);
	MwListTreeRefreshOn((Widget)w->fsel.tree);
}

static void
GetFiles(TreeFSelWidget w,char *dir,MwListTreeItem *item)
{
DirEntry entry;
Directory directory;
char *name;

	if (!DirectoryOpen(dir,&directory)) {
		fprintf(stderr,"Can't open directory '%s'\n",dir);
		return;
	}
	strcpy(w->fsel.WorkingDir,DirectoryPath(&directory));
	ClearFiles(w);
		
	MwListTreeRefreshOff((Widget)w->fsel.tree);
	MwListTreeHighlightItem((Widget)w->fsel.tree,item);

	while (DirectoryReadNextEntry(&directory,&entry)) {
		name=DirEntryFileName(&entry);
		switch(DirEntryType(&entry)) {
		case F_TYPE_DIR:
			break;
		default:
			StoreFile(w,name);
			break;
		}
	}
	strcpy(w->fsel.CurrentDir,DirectoryPath(&directory));
	DirectoryClose(&directory);
	XtVaSetValues(w->fsel.showdir,XtNlabel,w->fsel.CurrentDir,NULL);
	SortFiles(w);
	ShowFiles(w);
	MwListTreeRefreshOn((Widget)w->fsel.tree);
}

static void
ChangeDir(TreeFSelWidget w,char *dir,MwListTreeItem *parent)
{
	MwListTreeItem *dummy;

	dummy=NewDir(w,dir,parent);
	SortFiles(w);
	ShowFiles(w);
	MwListTreeRefreshOn((Widget)w->fsel.tree);
}

static void
PathCallback(Widget tree,XtPointer client,XtPointer call)
{
MwListTreeReturnStruct *ret;
MwListTreeItem *parent;
int count;
char dir[MAXNAMLEN*2];
TreeFSelWidget w;

	ret=(MwListTreeReturnStruct *)call;
	w=(TreeFSelWidget)client;
	if (ret->item->open) {
		strcpy(dir,"/");
		strcat(dir,ret->path[0]->text);
		count=1;
		while (count<ret->count) {
			strcat(dir,"/");
			strcat(dir,ret->path[count]->text);
			count++;
		}
		parent=ret->path[count-1];

		ChangeDir(w,dir,ret->item);
	}
	else {
		strcpy(dir,"/");
		count=0;
		while (count<ret->count-1) {
			strcat(dir,ret->path[count]->text);
			strcat(dir,"/");
			count++;
		}
		GetFiles(w,dir,ret->item->parent);
	}
	free(call);
}

static void
FileCallback(Widget list,XtPointer client,XtPointer call)
{
TreeFSelWidget w;
XawListReturnStruct *ret;

	ret=(XawListReturnStruct *)call;
	w=(TreeFSelWidget)client;
	strcpy(w->fsel.SelectedFile,w->fsel.CurrentDir);
	strcat(w->fsel.SelectedFile,ret->string);
	XtVaSetValues(w->fsel.sel,XtNstring,w->fsel.SelectedFile,NULL);
}

static void
OkCallback(Widget list,XtPointer client,XtPointer call)
{
TreeFSelWidget w;
TreeFSelReturnStruct ret;

	w=(TreeFSelWidget)client;
	if (w->fsel.Callback) {
		ret.button=1;
		ret.pathname=w->fsel.SelectedFile;
		XtCallCallbacks((Widget)w,XtNcallback,(XtPointer)&ret);
	}
}

static void
CancelCallback(Widget list,XtPointer client,XtPointer call)
{
TreeFSelWidget w;
TreeFSelReturnStruct ret;

	w=(TreeFSelWidget)client;
	if (w->fsel.Callback) {
		ret.button=0;
		ret.pathname=w->fsel.SelectedFile;
		XtCallCallbacks((Widget)w,XtNcallback,(XtPointer)&ret);
	}
}
