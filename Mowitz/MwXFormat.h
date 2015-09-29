#ifndef MW_XFORMAT_H
#define MW_XFORMAT_H

extern int MwFontHeight(int);
extern int MwFontWidth(int, char *);
extern void MwInitFormat(Display *);

extern int MwFontDescent(int);
extern int MwLookupColor(char *);
extern unsigned long MwGetColor(int);
extern int MwRcStrdraw(Drawable, GC, int, int, int, int, MwRichchar *, int, float);

#endif	/* MW_XFORMAT_H */
