/*
   Copyright (C) 1996-2002  Ulric Eriksson <ulric@siag.nu>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the Licence, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free
   Foundation, Inc., 59 Temple Place - Suite 330, Boston,
   MA 02111-1307, USA.
 */

#ifndef MW_TABLE_H
#define MW_TABLE_H

/****************************************************************
 *
 * Table widget
 *
 ****************************************************************/

/* Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 destroyCallback     Callback		Pointer		NULL
 height		     Height		Dimension	0
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 sensitive	     Sensitive		Boolean		True
 tableAdjHoriz	     TableAdjHoriz	Callback	NULL
 tableAdjVert	     TableAdjVert	Callback	NULL
 tableBackground     TableBackground	Pixel		XtDefaultBackground
 tableColWidth	     TableColWidth	Callback	NULL
 tableCursor	     TableCursor	Pixel		XtDefaultForeground
 tableData	     TableData		Callback	NULL
 tableDefaultHeight  TableDefaultHeight	Dimension	20
 tableDefaultWidth   TableDefaultWidth	Dimension	80
 tableForeground     TableForeground	Pixel		XtDefaultForeground
 tableFormat	     TableFormat	Callback	NULL
 tableGC	     TableGC		Callback	NULL
 tableGrid	     TableGrid		Pixel		XtDefaultForeground
 tableGridLines	     TableGridLines	Boolean		True
 tableHighlight	     TableHighlight	Pixel		XtDefaultForeground
 tableMaxCol	     TableMaxCol			1000
 tableMaxRow	     TableMaxRow			1000
 tablePointCol	     TablePointCol			0
 tablePointRow	     TablePointRow			0
 tableProtCol	     TableProtCol			1
 tableProtRow	     TableProtRow			1
 tableRedisplay	     TableRedisplay	Boolean		False
 tableRowHeight	     TableRowHeight	Callback	NULL
 tableSelectBottomRow TableSelectBottomRow		0
 tableSelectLeftCol  TableSelectLeftRow			0
 tableSelectRightCol TableSelectRightCol		0
 tableSelectTopRow   TableSelectTopRow			0
 tableText	     TableText		Callback	NULL
 tableTopCol	     TableTopCol			1
 tableTopRow	     TableTopRow			1
 tableType	     TableType		Callback	NULL
 tableVisibleCursor  TableVisibleCursor	Boolean		False
 width		     Width		Dimension	0
 x		     Position		Position	0
 y		     Position		Position	0

*/

/* define any special resource names here that are not in <X11/StringDefs.h> */

#define XtNtableMaxRow "tableMaxRow"
#define XtNtableMaxCol "tableMaxCol"
#define XtNtableProtRow "tableProtRow"
#define XtNtableProtCol "tableProtCol"
#define XtNtableTopRow "tableTopRow"
#define XtNtableTopCol "tableTopCol"
#define XtNtableSelectTopRow "tableSelectTopRow"
#define XtNtableSelectBottomRow "tableSelectBottomRow"
#define XtNtableSelectLeftCol "tableSelectLeftCol"
#define XtNtableSelectRightCol "tableSelectRightCol"
#define XtNtablePointRow "tablePointRow"
#define XtNtablePointCol "tablePointCol"
#define XtNtableDefaultWidth "tableDefaultWidth"
#define XtNtableDefaultHeight "tableDefaultHeight"
#define XtNtableColWidth "tableColWidth"
#define XtNtableRowHeight "tableRowHeight"
#define XtNtableType "tableType"
#define XtNtableText "tableText"
#define XtNtableFormat "tableFormat"
#define XtNtableData "tableData"
#define XtNtableRedisplay "tableRedisplay"
#define XtNtableGridLines "tableGridLines"
#define XtNtableVisibleCursor "tableVisibleCursor"
#define XtNtablePluginCoords "tablePluginCoords"
#define XtNtable3D "table3D"
#ifndef XtNzoom
#define XtNzoom "zoom"
#define XtCZoom "Zoom"
#endif

#define XtCTableMaxRow "TableMaxRow"
#define XtCTableMaxCol "TableMaxCol"
#define XtCTableProtRow "TableProtRow"
#define XtCTableProtCol "TableProtCol"
#define XtCTableTopRow "TableTopRow"
#define XtCTableTopCol "TableTopCol"
#define XtCTableSelectTopRow "TableSelectTopCol"
#define XtCTableSelectBottomRow "TableSelectBottomRow"
#define XtCTableSelectLeftCol "TableSelectLeftCol"
#define XtCTableSelectRightCol "TableSelectRightCol"
#define XtCTablePointRow "TablePointRow"
#define XtCTablePointCol "TablePointCol"
#define XtCTableDefaultWidth "TableDefaultWidth"
#define XtCTableDefaultHeight "TableDefaultHeight"
#define XtCTableColWidth "TableColWidth"
#define XtCTableRowHeight "TableRowHeight"
#define XtCTableType "TableType"
#define XtCTableText "TableText"
#define XtCTableFormat "TableFormat"
#define XtCTableData "TableData"
#define XtCTableRedisplay "TableRedisplay"
#define XtCTableGridLines "TableGridLines"
#define XtCTableVisibleCursor "TableVisibleCursor"
#define XtCTablePluginCoords "TablePluginCoords"
#define XtCTable3D "Table3D"

/* declare specific TableWidget class and instance datatypes */

typedef struct s_TableClassRec*	MwTableWidgetClass;
typedef struct s_TableRec*		MwTableWidget;

/* declare the class constant */

extern WidgetClass mwTableWidgetClass;

extern void MwTableCellToCoords(Widget, int, int, int *, int *);
extern void MwTableZoomedCellToCoords(Widget, int, int, int *, int *);
extern Pixmap MwTablePixmap(Widget);
extern void MwTableSetZoom(Widget, float);

#define MW_TABLE_TEXT 1
#define MW_TABLE_PIXMAP 2
#define MW_TABLE_EMBED 3

#endif /* MW_TABLE_H */

