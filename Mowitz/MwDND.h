/*
Copyright (C) 1996 César Crusius

This file is part of the DND Library.  This library is free
software; you can redistribute it and/or modify it under the terms of
the GNU Library General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your
option) any later version.  This library is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU Library General Public License for more details.
You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef __DragAndDropH__
#define __DragAndDropH__ 1L

/* The standard DND types are defined here */
#include "MwDNDTypes.h"

/* Xt stuff is defined here */
#include <X11/Intrinsic.h>

void MwDndInitialize(Widget shell);

void MwDndRegisterRootDrop(XtEventHandler handler);
void MwDndRegisterIconDrop(XtEventHandler handler);
void MwDndRegisterOtherDrop(XtEventHandler handler);

void
MwDndRegisterDropWidget(Widget widget,
		      XtEventHandler handler,
		      XtPointer data);
void
MwDndRegisterDragWidget(Widget widget,
		      XtEventHandler handler,
		      XtPointer data);

int
MwDndHandleDragging(Widget widget,
		  XEvent* event);

void MwDndMultipleShells(void);
void MwDndAddShell(Widget widget);

void MwDndSetData(int Type,unsigned char *Data,unsigned long Size);
void MwDndGetData(unsigned char **Data,unsigned long *Size);
		
int		MwDndIsIcon(Widget widget);
int		MwDndDataType(XEvent *event);
unsigned int	MwDndDragButtons(XEvent *event);
Widget		MwDndSourceWidget(XEvent *event);
Window          MwDndSourceWindow(XEvent *event);

void
MwDndDropCoordinates(Widget widget,
		   XEvent *event,
		   int *x,
		   int *y);
void            
MwDndDropRootCoordinates(XEvent *event,
		       int *x,
		       int *y);

long
MwDndProtocolVersion(XEvent *event);

int
MwDndIsDropMessage(XEvent *event);

void
MwDndChangeCursor(int Type,
		int width,int height,
		char *image,char *mask,
		int hot_x,int hot_y);
#endif


