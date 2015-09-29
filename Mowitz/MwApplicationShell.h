#ifndef MwApplicationShell_h
#define MwApplicationShell_h

#include <X11/Shell.h>

#ifndef XtNdragCallback
#define XtNdragCallback "dragCallback"
#define XtCDragCallback "DragCallback"
#endif
#ifndef XtNdropCallback
#define XtNdropCallback "dropCallback"
#define XtCDropCallback "DropCallback"
#endif
#ifndef XtNdropX
#define XtNdropX "dropX"
#define XtNdropY "dropY"
#endif
#ifndef XtNdropData
#define XtNdropData "dropData"
#define XtCDropData "DropData"
#endif
#ifndef XtNdropSize
#define XtNdropSize "dropSize"
#define XtCDropSize "DropSize"
#endif
#ifndef XtNdndType
#define XtNdndType "dndType"
#define XtCDndType "DndType"
#endif
#ifndef XtNxdndType
#define XtNxdndType "xdndType"
#define XtCXdndType "XdndType"
#endif
#ifndef XtNdropTypes
#define XtNdropTypes "dropTypes"
#define XtCDropTypes "DropTypes"
#endif

#ifndef DndNotDnd
#define DndNotDnd -1
#define DndUnknown 0
#define DndRawData 1
#define DndFile 2
#define DndFiles 3
#define DndText 4
#define DndDir 5
#define DndLink 6
#define DndExe 7
#define DndURL 8
#define DndMIME 9
#define DndEND 10
#endif

#ifndef DndDrop
#define DndDrop 1
#define XdndDrop 2
#define MotifDrop 3
#endif

typedef struct {
	Position x, y;
} DropPosition;

typedef struct MwApplicationShellClassRec *MwApplicationShellWidgetClass;
typedef struct MwApplicationShellRec *MwApplicationShellWidget;
extern WidgetClass mwApplicationShellWidgetClass;

#endif /* MwApplicationShell_h */
