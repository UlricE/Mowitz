#ifndef MW_XUTILS_H
#define MW_XUTILS_H

extern Pixmap MwLoadPixmap(Display *, Pixel, const char *);
extern void MwFreeColors(void);
extern void MwAllocColor(Display *, Colormap, XColor *);
extern void MwAllocNamedColor(Display *, char *, XColor *);
extern void MwQueryColor(Display *, Colormap, XColor *);
extern void MwCenter(Widget);
extern String MwLabelGet(Widget);
extern void MwLabelSet(Widget, String);
extern Boolean MwStateGet(Widget);
extern void MwStateSet(Widget, Boolean, int, int);
extern Dimension MwWidthGet(Widget);
extern Dimension MwHeightGet(Widget);
extern int MwXErrorHandler(Display *, XErrorEvent *);
extern void MwSetIcon(Widget, char **);
extern void MwHighlightInit(Widget);

#endif	/* MW_XUTILS_H */
