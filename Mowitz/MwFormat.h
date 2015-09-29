#ifndef MW_FORMAT_H
#define MW_FORMAT_H

#define MW_FMT_FAMILY (1<<0)
#define MW_FMT_SIZE (1<<1)
#define MW_FMT_BOLD (1<<2)
#define MW_FMT_ITALIC (1<<3)
#define MW_FMT_ULINE (1<<4)
#define MW_FMT_FG (1<<5)
#define MW_FMT_BG (1<<6)
#define MW_FMT_BORDERS (1<<7)
#define MW_FMT_VADJ (1<<8)
#define MW_FMT_HADJ (1<<9)
#define MW_FMT_STYLE (1<<10)
#define MW_FMT_STRIKE (1<<11)

#include <stdio.h>

typedef struct MwFmt {
        char *family;
        int size;
        int bold, italic, uline, strike;
        char *fg, *bg;
        int borders;
        int vadj, hadj;
        int style;
} MwFmt;

extern void MwDecodeFormat(int, long, MwFmt *);
extern int MwEncodeFormat(long, MwFmt *);

#define MW_BORDER_MASK (MW_BORDER_LEFT | MW_BORDER_RIGHT | MW_BORDER_TOP | MW_BORDER_BOTTOM)
#define MW_BORDER_LEFT (256)
#define MW_BORDER_RIGHT (512)
#define MW_BORDER_TOP (1024)
#define MW_BORDER_BOTTOM (2048)

#define MW_HADJ_MASK (4096 | 8192)
#define MW_HADJ_LEFT (0)
#define MW_HADJ_FULL (4096)
#define MW_HADJ_CENTER (8192)
#define MW_HADJ_RIGHT (4096+8192)

#define MW_VADJ_MASK (16384 | 32768)
#define MW_VADJ_TOP (16384)
#define MW_VADJ_CENTER (0)
#define MW_VADJ_BOTTOM (16384+32768)

/* First available bit is 65536 = 0x10000 = (1 << 16) */
#define MW_FMT_SHIFT (16)
#define MW_FMT_MASK (15<<MW_FMT_SHIFT)

/* not used in PW */
#define MW_FMT_DEFAULT (0<<MW_FMT_SHIFT)
#define MW_FMT_INVISIBLE (1<<MW_FMT_SHIFT)
#define MW_FMT_INTEGER (2<<MW_FMT_SHIFT)
#define MW_FMT_SCIENTIFIC (3<<MW_FMT_SHIFT)
#define MW_FMT_FIXED (4<<MW_FMT_SHIFT)
#define MW_FMT_DATE (5<<MW_FMT_SHIFT)
#define MW_FMT_TIME (6<<MW_FMT_SHIFT)
#define MW_FMT_TIMEDIFF (7<<MW_FMT_SHIFT)
#define MW_FMT_PERCENT (8<<MW_FMT_SHIFT)
#define MW_FMT_HEX (9<<MW_FMT_SHIFT)
#define MW_FMT_CURRENCY (10<<MW_FMT_SHIFT)
#define MW_FMT_USER1 (11<<MW_FMT_SHIFT)
#define MW_FMT_USER2 (12<<MW_FMT_SHIFT)
#define MW_FMT_USER3 (13<<MW_FMT_SHIFT)
#define MW_FMT_USER4 (14<<MW_FMT_SHIFT)
#define MW_FMT_USER5 (15<<MW_FMT_SHIFT)

/* only used in PW but does no harm in SIAG */
#define MW_STY_MASK 15
#define MW_STY_DEFAULT 0
#define MW_STY_HEADER1 1
#define MW_STY_HEADER2 2
#define MW_STY_HEADER3 3
#define MW_STY_HEADER4 4
#define MW_STY_HEADER5 5
#define MW_STY_HEADER6 6
#define MW_STY_ADDRESS 7
#define MW_STY_TITLE 8
#define MW_STY_ABSTRACT 9
#define MW_STY_PREFORMAT 10
#define MW_STY_USER1 11
#define MW_STY_USER2 12
#define MW_STY_USER3 13
#define MW_STY_USER4 14
#define MW_STY_USER5 15
#define MW_STY_EMBED MW_STY_USER5

/* one character */
typedef struct MwRichchar {
        unsigned char c;                /* a single character */
        unsigned int fmt;              /* same as in Siag */
} MwRichchar;

extern int MwFmtAttrToMask(char *);

extern int MwFormatCount;
extern int MwFontAliasCount;

extern char *MwLookupFontAlias(char *);
#if 0
extern void MwInitColors(void);
extern void MwInitFonts(void);
#endif
extern int MwFmtOldToNew(long);
extern long MwFmtNewToOld(int);
extern void MwSaveFormats(FILE *, int);
extern int MwLoadFormats(FILE *);
extern char **MwFontList(int *);
extern char **MwColorList(int *);
extern void MwPsMakeFonts(FILE *);
extern void MwPsSetColor(FILE *, int, int, int);
extern void MwPsSetFont(FILE *, int);
extern int MwRegisterColor(char *,
	unsigned short, unsigned short, unsigned short);

extern size_t MwRcStrlen(MwRichchar *);
extern MwRichchar *MwRcStrcpy(MwRichchar *, MwRichchar *);
extern MwRichchar *MwRcStrncpy(MwRichchar *, MwRichchar *, size_t);
extern MwRichchar *MwRcStrdup(MwRichchar *);
extern MwRichchar *MwRcStrins(MwRichchar *, MwRichchar *, size_t);
extern MwRichchar *MwRcMakerich(unsigned char *, int);
extern unsigned char *MwRcMakeplain(MwRichchar *);
extern float MwRcWidth(MwRichchar);
extern float MwRcStrwidth(MwRichchar *, int);
extern int MwRcStrheight(MwRichchar *, int);

/*#include "../common/Fontstuff/oldfonts.h"*/
/* This stuff is used by Siag Office for backwards compatibility. */

/* Fonts */
#define MW_SIZE_MASK (7)
#define MW_SIZE_8 (0)
#define MW_SIZE_10 (1)
#define MW_SIZE_12 (2)
#define MW_SIZE_14 (3)
#define MW_SIZE_18 (4)
#define MW_SIZE_24 (5)
#define MW_SIZE_20 (6)
#define MW_SIZE_30 (7)

#define MW_ITALIC (8)
#define MW_BOLD (16)

#define MW_FONT_SHIFT (5)
#define MW_FONT_MASK (96)
#define MW_COURIER (0)
#define MW_HELVETICA (32)
#define MW_NEW_CENTURY (64)
#define MW_TIMES (96)


/* black, red, green, blue, yellow, purple, cyan, white */
/* First available bit is 0x100000 = (1 << 20) */
#define MW_COLOR_SHIFT (20)
#define MW_COLOR_MASK (7<<MW_COLOR_SHIFT)
#define MW_COLOR_0 (0<<MW_COLOR_SHIFT)
#define MW_COLOR_1 (1<<MW_COLOR_SHIFT)
#define MW_COLOR_2 (2<<MW_COLOR_SHIFT)
#define MW_COLOR_3 (3<<MW_COLOR_SHIFT)
#define MW_COLOR_4 (4<<MW_COLOR_SHIFT)
#define MW_COLOR_5 (5<<MW_COLOR_SHIFT)
#define MW_COLOR_6 (6<<MW_COLOR_SHIFT)
#define MW_COLOR_7 (7<<MW_COLOR_SHIFT)

#define MW_ULINE (1<<23)

#endif	/* MW_FORMAT_H */
