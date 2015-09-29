
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/Xatom.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <Mowitz/Mowitz.h>
#include "MwApplicationShellP.h"

#undef TESTING
#ifdef TESTING
#define Printf printf
#else
#define Printf dnd_printf
static int dnd_printf(char *fmt, ...)
{
	return 0;
}
#endif

#define offset(field) XtOffsetOf(MwApplicationShellRec, mw_application.field)

static XtResource resources[] = {
	{
		XtNdragCallback,	/* name */
		XtCDragCallback,	/* class */
		XtRCallback,		/* type */
		sizeof(XtPointer),	/* size */
		offset(drag_callback),	/* offset */
		XtRCallback,		/* default_type */
		(XtPointer)0		/* default_addr */
	}, {
		XtNdropCallback,
		XtCDropCallback,
		XtRCallback,
		sizeof(XtPointer),
		offset(drop_callback),
		XtRCallback,
		(XtPointer)0
	}, {
		XtNdropX,
		XtCPosition,
		XtRPosition,
		sizeof(Position),
		offset(dropx),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNdropY,
		XtCPosition,
		XtRPosition,
		sizeof(Position),
		offset(dropy),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNdropData,
		XtCDropData,
		XtRPointer,
		sizeof(XtPointer),
		offset(drop_data),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNdropSize,
		XtCDropSize,
		XtRInt,
		sizeof(int),
		offset(drop_size),
		XtRImmediate,
		(XtPointer)0
	}, {
		XtNdndType,
		XtCDndType,
		XtRInt,
		sizeof(int),
		offset(dnd_type),
		XtRImmediate,
		(XtPointer)DndNotDnd
	}, {
		XtNxdndType,
		XtCXdndType,
		XtRAtom,
		sizeof(Atom),
		offset(xdnd_type),
		XtRImmediate,
		(XtPointer)None
	}, {
		XtNdropTypes,
		XtCDropTypes,
		XtRPointer,
		sizeof(XtPointer),
		offset(drop_types),
		XtRImmediate,
		(XtPointer)0
	}
};

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Realize(Widget, XtValueMask *, XSetWindowAttributes *);
static void Destroy(Widget);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);

#define superclass (&applicationShellClassRec)

MwApplicationShellClassRec mwApplicationShellClassRec = {
{
/* core */
	(WidgetClass)superclass,	/* superclass */
	"MwApplicationShell",		/* class_name */
	sizeof(MwApplicationShellRec),	/* size */
	NULL,				/* class_initialize */
	NULL,				/* class_part_initialize */
	FALSE,				/* Class inited */
	Initialize,			/* initialize */
	NULL,				/* initialize_hook */
	Realize,			/* realize */
	NULL,				/* actions */
	0,				/* num_actions */
	resources,			/* resources */
	XtNumber(resources),		/* resource_count */
	NULLQUARK,			/* xrm_class */
	FALSE,				/* compress_motion */
	TRUE,				/* compress_exposure */
	FALSE,				/* compress_enterleave */
	FALSE,				/* visible_interest */
	Destroy,			/* destroy */
	XtInheritResize,		/* resize */
	NULL,				/* expose */
	SetValues,			/* set_values */
	NULL,				/* set_values_hook */
	XtInheritSetValuesAlmost,	/* set_values_almost */
	NULL,				/* get_values_hook */
	NULL,				/* accept_focus */
	XtVersion,			/* intrinsics version */
	NULL,				/* callback offsets */
	XtInheritTranslations,		/* tm_table */
	NULL,				/* query_geometry */
	NULL,				/* display_accelerator */
	NULL,				/* extension */
},{
/* Composite */
	XtInheritGeometryManager,	/* geometry_manager */
	XtInheritChangeManaged,		/* change_managed */
	XtInheritInsertChild,		/* insert_child */
	XtInheritDeleteChild,		/* delete_child */
	NULL,				/* extension */
},{
/* Shell */
	NULL,				/* extension */
},{
/* WMShell */
	NULL,				/* extension */
},{
/* VendorShell */
	NULL,				/* extension */
},{
/* TopLevelShell */
	NULL,				/* extension */
},{
/* ApplicationShell */
	NULL,				/* extension */
},{
/* MwApplicationShell */
	NULL,				/* extension */
} };


WidgetClass mwApplicationShellWidgetClass = (WidgetClass)&mwApplicationShellClassRec;

/************************************************************************/

static Atom intern_atom(Widget w, char *n, Bool b)
{
	Atom a = XInternAtom(XtDisplay(w), n, b);
	Printf("XInternAtom(dpy, %s, %d) => %ld\n", n, (int)b, a);
	return a;
}

/* OffiX DND */
/* Based on code by Cesar Crusius */

/* We only accept the new protocol - the old one is broken and deprecated */
static int DndIsDropMessage(Widget w, XEvent *event)
{
	MwApplicationShellWidget aw = (MwApplicationShellWidget)w;
	if (event->xany.type == ClientMessage) {
Printf("ClientMessage, type = %s\n", XGetAtomName(XtDisplay(w), event->xclient.message_type));
		return (event->xclient.message_type == aw->mw_application.dnd_protocol);
	}
Printf("Event is not ClientMessage\n");
	return 0;
}

static long DndProtocolVersion(Widget w, XEvent *event)
{
	return DndIsDropMessage(w, event)?(event->xclient.data.l[4]):-1;
}

static int DndDataType(XEvent *event)
{
	int type = event->xclient.data.l[0];
	if (type >= DndEND) type = DndUnknown;
	return type;
}

static void DndGetData(Widget w, unsigned char **data, unsigned long *size)
{
	Display *dpy = XtDisplay(w);
	Window root = DefaultRootWindow(dpy);
	MwApplicationShellWidget aw = (MwApplicationShellWidget)w;

	Atom ActualType, ActualDndSelection;
	int ActualFormat;
	unsigned long RemainingBytes;

	ActualDndSelection = aw->mw_application.dnd_selection;
	XGetWindowProperty(dpy, root, ActualDndSelection,
			0L, 1000000L,
			FALSE, AnyPropertyType,
			&ActualType, &ActualFormat,
			size, &RemainingBytes,
			data);
}

static void offix_init(Widget w)
{
	MwApplicationShellWidget aw = (MwApplicationShellWidget)w;
	aw->mw_application.dnd_protocol = intern_atom(w, "_DND_PROTOCOL", FALSE);
	aw->mw_application.dnd_selection = intern_atom(w, "_DND_SELECTION", FALSE);
	aw->mw_application.wm_state = intern_atom(w, "WM_STATE", True);
}

/* OffiX-specific drop handler */
static void offix_handler(Widget w, XEvent *event)
{
	MwApplicationShellWidget aw = (MwApplicationShellWidget)w;
	unsigned char *data;
	unsigned long size;
	aw->mw_application.dnd_type = DndDataType(event);

	DndGetData(w, &data, &size);
	aw->mw_application.drop_data = data;
	aw->mw_application.drop_size = size;
	Printf("OffiX drop; protocol version %ld\n",
		DndProtocolVersion(w, event));
	XtCallCallbackList(w, aw->mw_application.drop_callback,
			(XtPointer)DndDrop);
}


/* XDND */
/* Based on code by Paul Sheer */

#define XDND_VERSION 3
 
 
/* XdndEnter */
#define XDND_THREE 3
#define XDND_ENTER_SOURCE_WIN(e)        ((e)->xclient.data.l[0])
#define XDND_ENTER_THREE_TYPES(e)       (((e)->xclient.data.l[1] & 0x1UL) == 0)
#define XDND_ENTER_THREE_TYPES_SET(e,b) (e)->xclient.data.l[1] = ((e)->xclient.data.l[1] & ~0x1UL) | (((b) == 0) ? 0 : 0x1UL)
#define XDND_ENTER_VERSION(e)           ((e)->xclient.data.l[1] >> 24)
#define XDND_ENTER_VERSION_SET(e,v)     (e)->xclient.data.l[1] = ((e)->xclient.data.l[1] & ~(0xFF << 24)) | ((v) << 24)
#define XDND_ENTER_TYPE(e,i)            ((e)->xclient.data.l[2 + i])    /* i => (0, 1, 2) */

/* XdndPosition */
#define XDND_POSITION_SOURCE_WIN(e)     ((e)->xclient.data.l[0])
#define XDND_POSITION_ROOT_X(e)         ((e)->xclient.data.l[2] >> 16)
#define XDND_POSITION_ROOT_Y(e)         ((e)->xclient.data.l[2] & 0xFFFFUL)
#define XDND_POSITION_ROOT_SET(e,x,y)   (e)->xclient.data.l[2]  = ((x) << 16) | ((y) & 0xFFFFUL)
#define XDND_POSITION_TIME(e)           ((e)->xclient.data.l[3])
#define XDND_POSITION_ACTION(e)         ((e)->xclient.data.l[4])

/* XdndStatus */
#define XDND_STATUS_TARGET_WIN(e)       ((e)->xclient.data.l[0])
#define XDND_STATUS_WILL_ACCEPT(e)      ((e)->xclient.data.l[1] & 0x1L)
#define XDND_STATUS_WILL_ACCEPT_SET(e,b) (e)->xclient.data.l[1] = ((e)->xclient.data.l[1] & ~0x1UL) | (((b) == 0) ? 0 : 0x1UL)
#define XDND_STATUS_WANT_POSITION(e)    ((e)->xclient.data.l[1] & 0x2UL)
#define XDND_STATUS_WANT_POSITION_SET(e,b) (e)->xclient.data.l[1] = ((e)->xclient.data.l[1] & ~0x2UL) | (((b) == 0) ? 0 : 0x2UL)
#define XDND_STATUS_RECT_X(e)           ((e)->xclient.data.l[2] >> 16)
#define XDND_STATUS_RECT_Y(e)           ((e)->xclient.data.l[2] & 0xFFFFL)
#define XDND_STATUS_RECT_WIDTH(e)       ((e)->xclient.data.l[3] >> 16)
#define XDND_STATUS_RECT_HEIGHT(e)      ((e)->xclient.data.l[3] & 0xFFFFL)
#define XDND_STATUS_RECT_SET(e,x,y,w,h) {(e)->xclient.data.l[2] = ((x) << 16) | ((y) & 0xFFFFUL); (e)->xclient.data.l[3] = ((w) << 16) | ((h) & 0xFFFFUL); }
#define XDND_STATUS_ACTION(e)           ((e)->xclient.data.l[4])

/* XdndLeave */
#define XDND_LEAVE_SOURCE_WIN(e)        ((e)->xclient.data.l[0])
 
/* XdndDrop */
#define XDND_DROP_SOURCE_WIN(e)         ((e)->xclient.data.l[0])
#define XDND_DROP_TIME(e)               ((e)->xclient.data.l[2])
 
/* XdndFinished */
#define XDND_FINISHED_TARGET_WIN(e)     ((e)->xclient.data.l[0])

static int XdndIsEnter(Widget w, XEvent *event)
{
	MwApplicationShellWidget aw = (MwApplicationShellWidget)w;
	return (event->xany.type == ClientMessage
		&& event->xclient.message_type == aw->mw_application.xdnd_enter);
}

/* get the protocol version, supported data types and the timestamp */
/* also pick a useful available data type for later */
/* we accept text/uri-list and text/plain */
static void xdnd_enter(Widget w, XEvent *event)
{
	Display *dpy = XtDisplay(w);
	MwApplicationShellWidget aw = (MwApplicationShellWidget)w;
	Atom *dt = aw->mw_application.drop_types;
	int i, j;
	char *n;
	unsigned char *data = NULL;
	Printf("xdnd_enter()\n");
	Printf("version = %d\n", (int)XDND_ENTER_VERSION(event));
	aw->mw_application.best_type = 0;
	if (!aw->mw_application.drop_types) return;
	if (XDND_ENTER_THREE_TYPES(event)) {
		for (i = 0; i < 3; i++) {
			long a = XDND_ENTER_TYPE(event, i);
			if (a == 0) continue;
			n = XGetAtomName(dpy, a);
			Printf("type %d = %ld (%s)\n", i, a, n);
			for (j = 0; dt[j]; j++) {
				if (a == dt[j]) {
					aw->mw_application.best_type = a;
					goto Done;	/* double break */
				}
			}
		}
	} else {
		Atom type, *typelist;
		int format;
		unsigned long count, remaining;
		Window source = XDND_ENTER_SOURCE_WIN(event);
		Printf("More than 3 types\n");
		XGetWindowProperty(dpy, source, aw->mw_application.xdnd_type_list,
				0, 0x8000000L, False, XA_ATOM,
				&type, &format, &count, &remaining, &data);
		if (type != XA_ATOM || format != 32 || count == 0 || !data) {
			if (data) XFree(data);
			Printf("XGetWindowProperty failed for XdndTypeList\n");
			return;
		}
		typelist = (Atom *)data;
		for (i = 0; i < count; i++) {
			Atom a = typelist[i];
			n = XGetAtomName(dpy, a);
			Printf("type %d = %ld (%s)\n", i, a, n);
			for (j = 0; dt[j]; j++) {
				if (a == dt[j]) {
					aw->mw_application.best_type = a;
					goto Done;	/* double break */
				}
			}
		}
	}
Done:
	if (data) XFree(data);
	if (aw->mw_application.best_type == 0) {
		Printf("No suitable data type found\n");
	} else {
		Printf("Will use data type '%s'\n",
			XGetAtomName(dpy, aw->mw_application.best_type));
	}
}

static int XdndIsPosition(Widget w, XEvent *event)
{
	MwApplicationShellWidget aw = (MwApplicationShellWidget)w;
	return (event->xany.type == ClientMessage
		&& event->xclient.message_type == aw->mw_application.xdnd_position);
}

static void xdnd_position(Widget wid, XEvent *event)
{
	Display *dpy = XtDisplay(wid);
	MwApplicationShellWidget aw = (MwApplicationShellWidget)wid;
	XEvent reply;
	int x = 0, y = 0, w = 0, h = 0;
	Window source = XDND_POSITION_SOURCE_WIN(event);
	DropPosition where;

	x = XDND_POSITION_ROOT_X(event);
	y = XDND_POSITION_ROOT_Y(event);
	Printf("xdnd_position()\n");
	Printf("position = (%d,%d)\n", x, y);
	Printf("time = %ld\n", XDND_POSITION_TIME(event));
	Printf("requested action = %ld\n", XDND_POSITION_ACTION(event));
	aw->mw_application.dropx = x;
	aw->mw_application.dropy = y;

	/* reply with a status message */
	memset(&reply, 0, sizeof reply);
	reply.xany.type = ClientMessage;
	reply.xany.display = dpy;
	reply.xclient.window = source;
	reply.xclient.message_type = aw->mw_application.xdnd_status;
	reply.xclient.format = 32;
	XDND_STATUS_TARGET_WIN(&reply) = XtWindow(wid);
	XDND_STATUS_WILL_ACCEPT_SET(&reply, (aw->mw_application.best_type != 0));
	XDND_STATUS_WANT_POSITION_SET(&reply, 1);
	XDND_STATUS_RECT_SET(&reply, x, y, w, h);
	XDND_STATUS_ACTION(&reply) = aw->mw_application.xdnd_action_copy;
	XSendEvent(dpy, source, 0, 0, &reply);
	where.x = x;
	where.y = y;
	XtCallCallbackList(wid, aw->mw_application.drag_callback,
		(XtPointer)&where);
}

static int XdndIsLeave(Widget w, XEvent *event)
{
	MwApplicationShellWidget aw = (MwApplicationShellWidget)w;
	return (event->xany.type == ClientMessage
		&& event->xclient.message_type == aw->mw_application.xdnd_leave);
}

static void xdnd_leave(Widget w, XEvent *event)
{
	MwApplicationShellWidget aw = (MwApplicationShellWidget)w;
	Printf("xdnd_leave()\n");
	aw->mw_application.best_type = 0;
}

/* get the data that was dropped */
static void xdnd_requestor_callback(Widget w, XtPointer client_data,
	Atom *selection, Atom *type, XtPointer value,
	unsigned long *length, int *format)
{
	MwApplicationShellWidget aw = (MwApplicationShellWidget)w;
	unsigned char *p = value;

	Printf("xdnd_requestor_callback()\n");

	if ((p == NULL) && (*length == 0)) {
		Printf("No selection or timed out\n");
		return;
	}

	MwFree(aw->mw_application.drop_data);
	p = MwMalloc((*length)+1);
	memcpy(p, value, *length);
	p[*length] = '\0';
	aw->mw_application.drop_data = p;
	aw->mw_application.xdnd_type = *type;
	XtFree(value);

	XtCallCallbackList(w, aw->mw_application.drop_callback,
		(XtPointer)XdndDrop);
}

static int XdndIsDrop(Widget w, XEvent *event)
{
	MwApplicationShellWidget aw = (MwApplicationShellWidget)w;
	return (event->xany.type == ClientMessage
		&& event->xclient.message_type == aw->mw_application.xdnd_drop);
}

static void xdnd_drop(Widget w, XEvent *event)
{
	MwApplicationShellWidget aw = (MwApplicationShellWidget)w;
	XEvent reply;
	Window source = XDND_POSITION_SOURCE_WIN(event);

	Printf("xdnd_drop()\n");
	XtGetSelectionValue(w, aw->mw_application.xdnd_selection,
		aw->mw_application.best_type,
		xdnd_requestor_callback, event, CurrentTime);

	/* now tell the source we are done */
	memset(&reply, 0, sizeof reply);
	reply.xany.type = ClientMessage;
	reply.xany.display = XtDisplay(w);
	reply.xclient.window = source;
	reply.xclient.message_type = aw->mw_application.xdnd_finished;
	reply.xclient.format = 32;
	XDND_FINISHED_TARGET_WIN(&reply) = XtWindow(w);
	XSendEvent(XtDisplay(w), source, 0, 0, &reply);
}

static void xdnd_init(Widget w)
{
	Display *dpy = XtDisplay(w);
	MwApplicationShellWidget aw = (MwApplicationShellWidget)w;
	aw->mw_application.xdnd_version = XDND_VERSION;
	aw->mw_application.xdnd_aware = intern_atom(w, "XdndAware", False);
	aw->mw_application.xdnd_selection = intern_atom(w, "XdndSelection", False);
	aw->mw_application.xdnd_enter = intern_atom(w, "XdndEnter", False);
	aw->mw_application.xdnd_leave = intern_atom(w, "XdndLeave", False);
	aw->mw_application.xdnd_position = intern_atom(w, "XdndPosition", False);
	aw->mw_application.xdnd_drop = intern_atom(w, "XdndDrop", False);
	aw->mw_application.xdnd_finished = intern_atom(w, "XdndFinished", False);
	aw->mw_application.xdnd_status = intern_atom(w, "XdndStatus", False);
	aw->mw_application.xdnd_action_copy = intern_atom(w, "XdndActionCopy", False);
	aw->mw_application.xdnd_action_move = intern_atom(w, "XdndActionMove", False);
	aw->mw_application.xdnd_action_link = intern_atom(w, "XdndActionLink", False);
	aw->mw_application.xdnd_action_private = intern_atom(w, "XdndActionPrivate", False);
	aw->mw_application.xdnd_type_list = intern_atom(w, "XdndTypeList", False);
	aw->mw_application.xdnd_action_list = intern_atom(w, "XdndActionList", False);
	aw->mw_application.xdnd_action_description = intern_atom(w, "XdndActionDescription", False);
	aw->mw_application.text_plain = intern_atom(w, "text/plain", False);
	aw->mw_application.text_urilist = intern_atom(w, "text/uri-list", False);

	XChangeProperty(dpy, XtWindow(w), aw->mw_application.xdnd_aware,
		XA_ATOM, 32, PropModeReplace,
		(unsigned char *)&(aw->mw_application.xdnd_version), 1);
}


/* Motif */
/* So far I've been unable to find useful info on the Motif protocol.
   What I have is either incomplete or on a too high level, i.e. uses
   the Motif library */

static void motif_init(Widget w)
{
	Printf("motif_init()\n");
}


/* Generic stuff for all known protocols */

/* Deal with client events for the toplevel widget */
static void drop_event(Widget w, XtPointer data, XEvent *event, Boolean *p)
{
	Printf("drop_event()\n");

	if (DndIsDropMessage(w, event)) {
		offix_handler(w, event);
		return;
	} else if (XdndIsEnter(w, event)) {
		xdnd_enter(w, event);
	} else if (XdndIsPosition(w, event)) {
		xdnd_position(w, event);
	} else if (XdndIsLeave(w, event)) {
		xdnd_leave(w, event);
	} else if (XdndIsDrop(w, event)) {
		xdnd_drop(w, event);
	}
}


/* Initialization */

/************************************************************************/

static void Initialize(Widget req, Widget new, ArgList args, Cardinal *nargs)
{
Printf("Initialize()\n");
}

static void Realize(Widget w, XtValueMask *m, XSetWindowAttributes *a)
{
	(*superclass->core_class.realize)(w, m, a);
Printf("Realize(%s)\n", XtName(w));
   	offix_init(w);
	xdnd_init(w);
	motif_init(w);
	XtAddEventHandler(w, NoEventMask, True, drop_event, NULL);
}

static void Destroy(Widget w)
{
Printf("Destroy()\n");
	;
}

static Boolean SetValues(Widget current, Widget request, Widget new,
			ArgList args, Cardinal *nargs)
{
	MwApplicationShellWidget oldw = (MwApplicationShellWidget)current;
	MwApplicationShellWidget neww = (MwApplicationShellWidget)new;

Printf("SetValues()\n");
	if (oldw->mw_application.drop_types != neww->mw_application.drop_types) {
		Atom *a = (Atom *)neww->mw_application.drop_types;
		Atom *b;
		int i;
		MwFree(oldw->mw_application.drop_types);
		for (i = 0; a[i]; i++);
		b = MwMalloc((i+1)*sizeof *b);
		for (i = 0; a[i]; i++) b[i] = a[i];
		b[i] = 0;
		neww->mw_application.drop_types = b;
	}
	return False;
}

