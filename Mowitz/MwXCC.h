/* $Id: XCC.h,v 1.6 1995/08/04 17:46:53 cwikla Exp $ */
/*
 * Copyright 1994,1995 John L. Cwikla
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appears in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of John L. Cwikla or
 * Wolfram Research, Inc not be used in advertising or publicity
 * pertaining to distribution of the software without specific, written
 * prior permission.  John L. Cwikla and Wolfram Research, Inc make no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * John L. Cwikla and Wolfram Research, Inc disclaim all warranties with
 * regard to this software, including all implied warranties of
 * merchantability and fitness, in no event shall John L. Cwikla or
 * Wolfram Research, Inc be liable for any special, indirect or
 * consequential damages or any damages whatsoever resulting from loss of
 * use, data or profits, whether in an action of contract, negligence or
 * other tortious action, arising out of or in connection with the use or
 * performance of this software.
 *
 * Author:
 *  John L. Cwikla
 *  X Programmer
 *  Wolfram Research Inc.
 *
 *  cwikla@wri.com
*/

#ifndef MW_XCC_h
#define MW_XCC_h

typedef struct Mw_XColorContext *MwXCC;


/* Put Mw in front of everything */
extern MwXCC MwXCCCreate(Display *_dpy, Visual *_visual, int _usePrivateColormap, 
						int _useStdCmaps, Atom _stdColormapAtom, Colormap *_colormap);
extern MwXCC MwXCCMonoCreate(Display *_dpy, Visual *_visual, Colormap _colormap);
extern void MwXCCFree(MwXCC _xcc);

extern unsigned long MwXCCGetPixel(MwXCC _xcc, unsigned int _red, unsigned int _green, unsigned int _blue);
extern int MwXCCGetNumColors(MwXCC _xcc);

extern Colormap MwXCCGetColormap(MwXCC _xcc);
extern Visual *MwXCCGetVisual(MwXCC _xcc);
extern XVisualInfo *MwXCCGetVisualInfo(MwXCC _xcc);
extern Display *MwXCCGetDisplay(MwXCC _xcc);
extern int MwXCCQueryColor(MwXCC _xcc, XColor *_color);
extern int MwXCCQueryColors(MwXCC _xcc, XColor *_colors, int _numColors);


#endif /* MW_XCC_H */
