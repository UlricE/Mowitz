#ifndef MW_FILESEL_P_H
#define MW_FILESEL_P_H

#include "MwFilesel.h"
#include "MwRudegridP.h"

typedef struct {XtPointer extension;} MwFileselClassPart;

typedef struct MwFileselClassRec {
	CoreClassPart		core_class;
	CompositeClassPart	composite_class;
	ConstraintClassPart	constraint_class;
	MwRudegridClassPart	rudegrid_class;
	MwFileselClassPart	filesel_class;
} MwFileselClassRec;

extern MwFileselClassRec mwFileselClassRec;

typedef struct MwFileselPart {
	/* public */
	String format;
	String dir_label;
	int ch_ext;
	XtCallbackList cancel_callbacks, select_callbacks;

	/* private */
	Widget topframe, bottomframe, filelabel, textframe,
		fileframe, formatlabel, formatbutton,
		dirlabel, dirbutton, dirframe;
	Widget topbox, bottombox, filetext, filelabel2, fileviewport,
		filelist, dirlabel2, dirviewport, dirlist;
	Widget dirmenu, formatmenu, extra[10];
	Widget okbutton, cancelbutton, findbutton, helpbutton;
#if 0
	int status;
#endif
	char **files, **dirs;
	int nfiles, ndirs;
	int nentry, nextra;
	struct {
		Widget w;
		char *d;
	} dir_entry[20];
} MwFileselPart;

typedef struct MwFileselRec {
	CorePart		core;
	CompositePart		composite;
	ConstraintPart		constraint;
	MwRudegridPart		rudegrid;
	MwFileselPart		filesel;
} MwFileselRec;

typedef struct {XtPointer extension;} MwFileselConstraintsPart;

typedef struct MwFileselConstraintsRec {
	MwRudegridConstraintsPart	rudegrid;
	MwFileselConstraintsPart	filesel;
} MwFileselConstraintsRec, *MwFileselConstraints;

#endif /* MW_FILESEL_P_H */

