#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#ifdef MOTIF
#include <Xm/ScrolledW.h>
#else
#include <X11/Xaw/Viewport.h>
#endif
#include <Mowitz/MwListTree.h>

Widget	toplevel,paned,form,tree,list;
XtAppContext app_con;
char *files[256];


void
HighlightCallback(w,client,call)
Widget w;
XtPointer client;
XtPointer call;
{
MwListTreeMultiReturnStruct *ret;
MwListTreeItem *item;
int i;

	ret=(MwListTreeMultiReturnStruct *)call;
	printf("HIGHLIGHT: count=%d\n",ret->count);
	for (i=0; i<ret->count; i++) {
	  item=ret->items[i];
	  printf("%s",item->text);
	  while (item->parent) {
	    item=item->parent;
	    printf("<--%s",item->text);
	  }
	  printf("\n");
	}
}

void
ActivateCallback(w,client,call)
Widget w;
XtPointer client;
XtPointer call;
{
MwListTreeActivateStruct *ret;
MwListTreeMultiReturnStruct ret2;
int count;

	ret=(MwListTreeActivateStruct *)call;
	printf("ACTIVATE: item=%s count=%d\n",ret->item->text,ret->count);
	count=0;
	while (count<ret->count) {
		printf(" path: %s\n",ret->path[count]->text);
		count++;
	}
  	MwListTreeSetHighlighted(w,ret->path,ret->count,True);
  	MwListTreeGetHighlighted(w,&ret2);
  	MwListTreeSetHighlighted(w,ret2.items,ret2.count,True);
}

static void Init(Widget tree)
{
int i,j,k,l;
char test[64];
MwListTreeItem *level1,*level2,*level3,*level4;

	for (i=0; i<10; i++) {
		sprintf(test,"%c Level #1, entry #%d",
			(char)('Z'-i),i);
		level1=MwListTreeAdd(tree,NULL,test);
		for (j=0; j<i; j++) {
			sprintf(test,"%c Level #2, entry #%d",
				(char)('Z'-j),j);
			level2=MwListTreeAdd(tree,level1,test);
			for (k=0; k<i; k++) {
				sprintf(test,"%c Level #3, entry #%d",
					(char)('Z'-k),k);
				level3=MwListTreeAdd(tree,level2,test);
				for (l=0; l<i; l++) {
					sprintf(test,"%c Level #4, entry #%d",
						(char)('Z'-l),l);
					level4=MwListTreeAdd(tree,level3,test);
				}
			}
		}
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
#ifdef USE_XPM
Pixmap open,closed;
Pixel bg;
XpmColorSymbol transparent;
XpmAttributes attrib;
#endif

	toplevel = XtAppInitialize(&app_con,"MwListTreeDemo",NULL,0,
				&argc,argv,NULL,NULL,0);

#ifdef MOTIF
	form=XtVaCreateManagedWidget("form",xmScrolledWindowWidgetClass,toplevel,
		XmNscrollingPolicy,		XmAUTOMATIC,
		XmNvisualPolicy,		XmCONSTANT,
		XmNscrollBarDisplayPolicy,	XmSTATIC,
/*		XmNscrollBarDisplayPolicy,	XmAS_NEEDED,*/
		NULL);
#else
	form = XtVaCreateManagedWidget("form",viewportWidgetClass,toplevel,
		XtNallowVert,	True,
		XtNallowHoriz,	True,
/*		XtNforceBars,	True,*/
                XtNuseBottom,   True,
		NULL,0);
#endif

#ifdef USE_XPM
        XtVaGetValues(form,
                XtNbackground,  &bg,
                NULL);
        transparent.name=NULL;
        transparent.value="#FFFFFFFFFFFF";
        transparent.pixel=bg;
        attrib.colorsymbols=&transparent;
        attrib.valuemask=XpmColorSymbols;
        attrib.numsymbols=1;
	XpmCreatePixmapFromData(XtDisplay(form),
		RootWindowOfScreen(XtScreen(form)),
		folderopen_xpm,&open,NULL,&attrib);
	XpmCreatePixmapFromData(XtDisplay(form),
		RootWindowOfScreen(XtScreen(form)),
		folder_xpm,&closed,NULL,&attrib);
#endif
	tree = XtVaCreateManagedWidget("form",listtreeWidgetClass,form,
		XtNfont,	XLoadQueryFont(XtDisplay(form),"-adobe-helvetica-medium-r-normal--*-100-*"),
		XtNheight,      	(Dimension)500,
		XtNwidth,		(Dimension)150,
#ifdef MOTIF
		XtNborderWidth,		(Dimension)0,
#endif
#ifdef USE_XPM
		XtNbranchPixmap,	closed,
		XtNbranchOpenPixmap,	open,
		XtNleafPixmap,		closed,
		XtNleafOpenPixmap,	open,
#else
/*		XtNbranchPixmap,	NULL,*/
/*		XtNbranchOpenPixmap,	NULL,*/
/*		XtNleafPixmap,		NULL,*/
/*		XtNleafOpenPixmap,	NULL,*/
/*		XtNindent,		(int)5,*/
#endif
		NULL,0);

	Init(tree);
	XtAddCallback(tree,XtNhighlightCallback, HighlightCallback, (XtPointer) NULL);
	XtAddCallback(tree,XtNactivateCallback, ActivateCallback, (XtPointer) NULL);

	XtRealizeWidget(toplevel);

	XtAppMainLoop(app_con);
	return 0;
}
