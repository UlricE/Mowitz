#ifndef MwApplicationShellP_h
#define MwApplicationShellP_h

#include "MwApplicationShell.h"
#include <X11/ShellP.h>

typedef struct {
	XtPointer extension;
} MwApplicationShellClassPart;

typedef struct MwApplicationShellClassRec {
	CoreClassPart			core_class;
	CompositeClassPart		composite_class;
	ShellClassPart			shell_class;
	WMShellClassPart		wm_shell_class;
	VendorShellClassPart		vendor_shell_class;
	TopLevelShellClassPart		top_level_shell_class;
	ApplicationShellClassPart	application_shell_class;
	MwApplicationShellClassPart	mw_application_shell_class;
} MwApplicationShellClassRec;

extern MwApplicationShellClassRec mwApplicationShellClassRec;

typedef struct MwApplicationShellPart {
	/* public */
	XtCallbackList		drag_callback;
	XtCallbackList		drop_callback;
	Position		dropx, dropy;
	XtPointer		drop_data;
	int			drop_size;
	int			dnd_type;
	Atom			xdnd_type;
	XtPointer		drop_types;
	/* private for OffiX */
	Atom			dnd_protocol, dnd_selection, wm_state;
	/* private for XDND */
	long			xdnd_version;
	Atom			xdnd_aware, xdnd_selection, xdnd_enter,
				xdnd_leave, xdnd_position, xdnd_drop,
				xdnd_finished, xdnd_status, xdnd_action_copy,
				xdnd_action_move, xdnd_action_link,
				xdnd_action_private, xdnd_type_list,
				xdnd_action_list, xdnd_action_description,
				text_plain, text_urilist, best_type;
} MwApplicationShellPart;

typedef struct MwApplicationShellRec {
	CorePart		core;
	CompositePart		composite;
	ShellPart		shell;
	WMShellPart		wm_shell;
	VendorShellPart		vendor;
	TopLevelShellPart	topLevel;
	ApplicationShellPart	application;
	MwApplicationShellPart	mw_application;
} MwApplicationShellRec;

#endif /* MwApplicationShellP_h */
