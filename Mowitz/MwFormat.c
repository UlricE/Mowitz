
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>

/* does not include Mowitz.h, because we want no references to X */
#include <Mowitz/MwUtils.h>
#include <Mowitz/MwFormat.h>
#include <Mowitz/MwFormatP.h>

/* ---
richchar.c

A "rich char" is a normal 8-bit character plus format information
   such as font, colour, size. This module contains functions to handle
   them much like the standard string library does.
--- */

#include <sys/types.h>
#include <unistd.h>

int MwFormatCount;

static struct fa_struct {
        char *alias;
        char *name;
} fontalias[256];

int MwFontAliasCount = 0;

static char *glyph_names[256];
MwFontname MwFontnameTable[256];
static int mw_nfontname;
char *mowitz_data;	/* datadir */

static struct {
	char *name;
	int mask;
} attrnames[] = {
	{"family", MW_FMT_FAMILY},
	{"size", MW_FMT_SIZE},
	{"fg", MW_FMT_FG},
	{"bg", MW_FMT_BG},
	{"bold", MW_FMT_BOLD},
	{"italic", MW_FMT_ITALIC},
	{"uline", MW_FMT_ULINE},
	{"strike", MW_FMT_STRIKE},
	{"hadj", MW_FMT_HADJ},
	{"vadj", MW_FMT_VADJ},
	{"borders", MW_FMT_BORDERS},
	{"style", MW_FMT_STYLE},
	{NULL, 0}
};

static int compar(const void *p, const void *q)
{
        return strcmp(*(const char **)p, *(const char **)q);
}

/* ---
returns a null-terminated, sorted list of all known fonts.
Caller must free
*/

char **MwFontList(int *n)
{
        int i, j;
        char **list;

	mw_init_format();

	list = (char **)MwMalloc((MwFontAliasCount+mw_nfontname+1)*sizeof(char *))
;
        for (i = 0; i < MwFontAliasCount; i++)
                list[i] = MwStrdup(fontalias[i].alias);
        for (j = 0; j < mw_nfontname; j++)
                list[i+j] = MwStrdup(MwFontnameTable[j].name);
        list[i+j] = NULL;
        qsort(list, i+j, sizeof(char *), compar);
        *n = i+j;
        return list;
}

/* ---
returns the index of a font with the specified name, or -1 for failure
*/

int MwLookupFontname(char *name)
{  
        int i;

        mw_init_format();
        for (i = 0; i < mw_nfontname; i++) {
                if (!MwStrcasecmp(name, MwFontnameTable[i].name))
                        return i;
        }
        return -1;
}

/* ---
Return an old-style attribute mask from its name.
*/

int MwFmtAttrToMask(char *attr)
{
	int i;

	mw_init_format();

	for (i = 0; attrnames[i].name; i++) {
		if (!MwStrcasecmp(attr, attrnames[i].name))
			return attrnames[i].mask;
	}
	return 0;
}

#if 0
/* ---
Return an attribute name from its old-style mask.
*/

static char *fmt_mask2attr(int mask)
{
	int i;

	for (i = 0; attrnames[i].name; i++) {
		if (mask == attrnames[i].mask)
			return attrnames[i].name;
	}
	return NULL;
}
#endif

/* ---
Loading and saving formats.

This code is shared between all programs that use the new formats.
That way there will be less duplication of code.

This requires new tags to store the formats. The new tag is

.ft N

where N is a number. This is followed by additional lines of
the format

name value

where name is the name of an attribute and
value is the value of the attribute. The beauty of this is
that it can be extended with new attributes without breaking
compatibility. Old versions of the program will simply not
understand new attributes and will therefore ignore them.
A format specification is terminated by the word "end" alone
on a line.

Example:

.ft 0
family Times
size 10
uline n
fg red
end
.ft 1
family Courier
size 12
italic y
end

It is not necessary to specify all attributes. The ones that are
not specified will use defaults compiled into the program.

Compatibility: Old files contain no .ft tags. For those, a function
will be used that translates from the old bitmap fields to new
structures. Another function translates in the other direction,
making it possible to store in a format old versions of Siag
can open.
--- */

/* ---
Convert an old-style bitmapped format to the new style index.
*/

int MwFmtOldToNew(long oldfmt)
{
	MwFmt fmt;

	mw_init_format();

	switch (oldfmt & MW_SIZE_MASK) {
	case MW_SIZE_8: fmt.size = 80; break;
	case MW_SIZE_10: fmt.size = 100; break;
	case MW_SIZE_12: fmt.size = 120; break;
	case MW_SIZE_14: fmt.size = 140; break;
	case MW_SIZE_18: fmt.size = 180; break;
	case MW_SIZE_24: fmt.size = 240; break;
	case MW_SIZE_20: fmt.size = 200; break;
	default: fmt.size = 300; break;
	}
	fmt.italic = ((oldfmt & MW_ITALIC) != 0);
	fmt.bold = ((oldfmt & MW_BOLD) != 0);
	fmt.uline = ((oldfmt & MW_ULINE) != 0);
	fmt.strike = 0;		/* strikethrough didn't exist then */
	switch (oldfmt & MW_FONT_MASK) {
	case MW_COURIER: fmt.family = "Courier"; break;
	case MW_HELVETICA: fmt.family = "Helvetica"; break;
	case MW_NEW_CENTURY: fmt.family = "New Century Schoolbook"; break;
	default: fmt.family = "Times"; break;
	}
	switch (oldfmt & MW_COLOR_MASK) {
	case MW_COLOR_0: fmt.fg = "black"; break;
	case MW_COLOR_1: fmt.fg = "red"; break;
	case MW_COLOR_2: fmt.fg = "green"; break;
	case MW_COLOR_3: fmt.fg = "blue"; break;
	case MW_COLOR_4: fmt.fg = "yellow"; break;
	case MW_COLOR_5: fmt.fg = "magenta"; break;
	case MW_COLOR_6: fmt.fg = "cyan"; break;
	default: fmt.fg = "white"; break;
	}
	fmt.bg = "white";
	fmt.borders = oldfmt & MW_BORDER_MASK;
	fmt.vadj = oldfmt & MW_VADJ_MASK;
	fmt.hadj = oldfmt & MW_HADJ_MASK;
	/* this works because the old styles are registered first */
	fmt.style = (oldfmt & MW_FMT_MASK) >> MW_FMT_SHIFT;
	return MwEncodeFormat(~0, &fmt);
}

/* ---
Convert a new-style format index to the old bitmapped format.
this function could try harder
*/

long MwFmtNewToOld(int newfmt)
{
	mw_init_format();

	return MW_SIZE_10 | MW_HELVETICA;
}

/* the default format */
static MwFmt dfmt = { "Helvetica", 120, 0, 0, 0, 0, "black", "white",
			0, MW_VADJ_CENTER, MW_HADJ_LEFT, 0};

/* ---
the formats are not split up into fonts, colors and rest, but saved
   in a single level. The restructuring is done when the formats are read.
   Attributes: font, size, bold, italic, uline, strike, fg, bg, siagfmt
*/

void MwSaveFormats(FILE *fp, int i)
{
	MwFmt fmt;

	mw_init_format();

	MwDecodeFormat(i, ~0, &fmt);
	fprintf(fp, ".ft %d\n", i);
	if (MwStrcasecmp(fmt.family, dfmt.family))
		fprintf(fp, "font %s\n", fmt.family);
	if (fmt.size != dfmt.size)
		fprintf(fp, "size %d\n", fmt.size);
	if (fmt.bold != dfmt.bold)
		fprintf(fp, "bold %d\n", fmt.bold);
	if (fmt.italic != dfmt.italic)
		fprintf(fp, "italic %d\n", fmt.italic);
	if (fmt.uline != dfmt.uline)
		fprintf(fp, "uline %d\n", fmt.uline);
	if (fmt.strike != dfmt.strike)
		fprintf(fp, "strike %d\n", fmt.strike);
	if (MwStrcasecmp(fmt.fg, dfmt.fg))
		fprintf(fp, "fg %s\n", fmt.fg);
	if (MwStrcasecmp(fmt.bg, dfmt.bg))
		fprintf(fp, "bg %s\n", fmt.bg);
	if (fmt.borders != dfmt.borders)
		fprintf(fp, "borders %d\n", fmt.borders);
	if (fmt.vadj != dfmt.vadj)
		fprintf(fp, "vadj %d\n", fmt.vadj);
	if (fmt.hadj != dfmt.hadj)
		fprintf(fp, "hadj %d\n", fmt.hadj);
	if (fmt.style != dfmt.style)
		fprintf(fp, "style %d\n", fmt.style);
	fprintf(fp, "end\n");
}

/* ---
When this function gets to borrow the file pointer, we are about to
   read the first attribute of a font. We keep on reading until eof
   or "end". The value returned is the index into the real format table.
*/

int MwLoadFormats(FILE *fp)
{
	char b[1000];
	char font[256], fg[256], bg[256];
	MwFmt fmt = dfmt;

	mw_init_format();

	while (fgets(b, sizeof b, fp) && strncmp(b, "end", 3)) {
		MwChomp(b);
		if (!strncmp(b, "font ", 5)) {
			strcpy(font, b+5);
			fmt.family = font;
		} else if (!strncmp(b, "size ", 5)) {
			fmt.size = strtol(b+5, NULL, 10);
		} else if (!strncmp(b, "bold ", 5)) {
			fmt.bold = strtol(b+5, NULL, 10);
		} else if (!strncmp(b, "italic ", 7)) {
			fmt.italic = strtol(b+7, NULL, 10);
		} else if (!strncmp(b, "uline ", 6)) {
			fmt.uline = strtol(b+6, NULL, 10);
		} else if (!strncmp(b, "strike ", 6)) {
			fmt.strike = strtol(b+7, NULL, 10);
		} else if (!strncmp(b, "fg ", 3)) {
			strcpy(fg, b+3);
			fmt.fg = fg;
		} else if (!strncmp(b, "bg ", 3)) {
			strcpy(bg, b+3);
			fmt.bg = bg;
		} else if (!strncmp(b, "borders ", 8)) {
			fmt.borders = strtol(b+8, NULL, 10);
		} else if (!strncmp(b, "vadj ", 5)) {
			fmt.vadj = strtol(b+5, NULL, 10);
		} else if (!strncmp(b, "hadj ", 5)) {
			fmt.hadj = strtol(b+5, NULL, 10);
		} else if (!strncmp(b, "style ", 6)) {
			fmt.style = strtol(b+6, NULL, 10);
		}
		/* ignore anything we don't grok */
	}
	return MwEncodeFormat(~0, &fmt);
}

static void load_encoding(char *fn)
{
	int state = 0;
	char b[256];
	FILE *fp = fopen(fn, "r");
	int i, n = 0;
	if (fp == NULL) return;

	while (fgets(b, sizeof b, fp)) {
		if (!strncmp(b, "Encoding=", 9)) {
                        state = 1;
                } else if (state) {
                        for (i = 0; b[i] && !isspace(b[i]); i++);
                        b[i] = '\0';
			if (n >= 256) {
				fprintf(stderr, "Too many glyphs\n");
				break;
			}
                        glyph_names[n++] = MwStrdup(b);
                }
        }
        fclose(fp);
}

static int find_glyph(char *n)
{
	int i;
 
        for (i = 0; i < 256; i++)
                if (glyph_names[i] && !strcmp(glyph_names[i], n)) return i;
        return -1;
}

static int *load_afm(char *q)
{
	char fn[1024];
	int state = 0, w, g;
	char b[256], n[256], *p;
	int *widths = MwCalloc(512, sizeof *widths);
	FILE *fp;

	if (q[0] == '/') strcpy(fn, q);
	else sprintf(fn, "%s/fonts/%s", mowitz_data, q);

	fp = fopen(fn, "r");
	if (fp == NULL) return NULL;

	while (fgets(b, sizeof b, fp)) {
                if (!strncmp(b, "StartCharMetrics", 16)) {
                        state = 1;
                } else if (!strncmp(b, "EndCharMetrics", 14)) {
                        state = 0;
                } else if (state) {
                        p = strstr(b, "WX ");
                        if (!p) continue;
                        sscanf(p, "WX %d", &w);
                        p = strstr(b, "C ");
                        if (p && sscanf(p, "C %d", &g) == 1 && g >= 0)
                                widths[g] = w;
                        p = strstr(b, "N ");
                        if (!p) continue;
                        sscanf(p, "N %s", n);
                        g = find_glyph(n);
                        if (g == -1) continue;
                        widths[g] = w;
                }
        }
	fclose(fp);
	return widths;
}

static void grow_fontname_table(char *name)
{
	int i;

	MwFontnameTable[mw_nfontname].name = MwStrdup(name);
	for (i = 0; i < 4; i++) {
		MwFontnameTable[mw_nfontname].afm[i] = NULL;
		MwFontnameTable[mw_nfontname].pfb[i] = NULL;
		MwFontnameTable[mw_nfontname].x_name[i] = NULL;
		MwFontnameTable[mw_nfontname].ps_name[i] = NULL;
		MwFontnameTable[mw_nfontname].t1_name[i] = NULL;
	}
	mw_nfontname++;
}

/* ---
X, T1 and PS fonts are registered separately because they are device
dependent on different devices. This is slightly lame because they will
probably be called from the same Scheme wrapper function, and they are
in the same C translation unit. Never mind.
*/

static int register_afm(char *name, char *a, char *ai, char *ab, char *abi)
{
	int i;

	for (i = 0; i < mw_nfontname; i++) {
		if (!MwStrcasecmp(name, MwFontnameTable[i].name)) break;
	}
	if (i == mw_nfontname) grow_fontname_table(name);
	MwFontnameTable[i].afm[0] = load_afm(a);
	MwFontnameTable[i].afm[1] = load_afm(ai);
	MwFontnameTable[i].afm[2] = load_afm(ab);
	MwFontnameTable[i].afm[3] = load_afm(abi);
	return i;
}

static int register_x_font(char *name, char *x, char *xi, char *xb, char *xbi)
{
	int i;

	for (i = 0; i < mw_nfontname; i++) {
		if (!MwStrcasecmp(name, MwFontnameTable[i].name)) break;
	}
	if (i == mw_nfontname) grow_fontname_table(name);
	MwFontnameTable[i].x_name[0] = MwStrdup(x);
	MwFontnameTable[i].x_name[1] = MwStrdup(xi);
	MwFontnameTable[i].x_name[2] = MwStrdup(xb);
	MwFontnameTable[i].x_name[3] = MwStrdup(xbi);
	return i;
}

static void register_encoding(char *name)
{
	int i;

	for (i = 0; i < mw_nfontname; i++) {
		if (!MwStrcasecmp(name, MwFontnameTable[i].name)) break;
	}
	if (i < mw_nfontname) MwFontnameTable[i].iso8859_1 = 0;
}

/* ---
As above but for Postscript.
*/

static int register_ps_font(char *name, char *ps, char *psi, char *psb, char *psbi)
{
	int i;

	for (i = 0; i < mw_nfontname; i++) {
		if (!MwStrcasecmp(name, MwFontnameTable[i].name)) break;
	}
	if (i == mw_nfontname) grow_fontname_table(name);
	MwFontnameTable[i].ps_name[0] = MwStrdup(ps);
	MwFontnameTable[i].ps_name[1] = MwStrdup(psi);
	MwFontnameTable[i].ps_name[2] = MwStrdup(psb);
	MwFontnameTable[i].ps_name[3] = MwStrdup(psbi);
	/* and now, a piece written in Hackish */
	MwFontnameTable[i].iso8859_1 = strcmp(ps, psi)?1:0;
	return i;
}

/* ---
As above but for pfb files.
*/

static int register_pfb(char *name, char *ps, char *psi, char *psb, char *psbi)
{
	int i;

	for (i = 0; i < mw_nfontname; i++) {
		if (!MwStrcasecmp(name, MwFontnameTable[i].name)) break;
	}
	if (i == mw_nfontname) grow_fontname_table(name);
	MwFontnameTable[i].pfb[0] = MwStrdup(ps);
	MwFontnameTable[i].pfb[1] = MwStrdup(psi);
	MwFontnameTable[i].pfb[2] = MwStrdup(psb);
	MwFontnameTable[i].pfb[3] = MwStrdup(psbi);
	MwFontnameTable[i].iso8859_1 = 0;
	return i;
}

/* ---
As above but for Type 1 (T1lib).
*/

static int register_t1_font(char *name, char *ps, char *psi, char *psb, char *psbi)
{
	int i;

	for (i = 0; i < mw_nfontname; i++) {
		if (!MwStrcasecmp(name, MwFontnameTable[i].name)) break;
	}
	if (i == mw_nfontname) grow_fontname_table(name);
	MwFontnameTable[i].t1_name[0] = MwStrdup(ps);
	MwFontnameTable[i].t1_name[1] = MwStrdup(psi);
	MwFontnameTable[i].t1_name[2] = MwStrdup(psb);
	MwFontnameTable[i].t1_name[3] = MwStrdup(psbi);
	/* and now, a piece written in Hackish */
	MwFontnameTable[i].iso8859_1 = strcmp(ps, psi)?1:0;
	return i;
}

/* ---
handle mapping between e.g. Arial and Helvetica

Store the name of a font we don't have and the name of a replacement
*/

int MwFontAlias(char *alias, char *name)
{
	int i;

	mw_init_format();

	for (i = 0; i < MwFontAliasCount; i++) {
		if (!MwStrcasecmp(alias, fontalias[i].alias)) break;
	}
	if (i == MwFontAliasCount) {
		MwFontAliasCount++;
		fontalias[i].alias = MwStrdup(alias);
	}
	fontalias[i].name = MwStrdup(name);
	return i;
}

/* ---
If there is a font with exactly the alias name, return that. Otherwise
if there is an alias, return the name of the real font. Otherwise NULL.
*/

char *MwLookupFontAlias(char *alias)
{                                   
        int i;

	mw_init_format();

        if (alias == NULL) return NULL;
        if (MwLookupFontname(alias) >= 0) return alias;
        for (i = 0; i < MwFontAliasCount; i++) {
                if (!MwStrcasecmp(alias, fontalias[i].alias))
                        return fontalias[i].name;
        }
        return NULL;
}


static void MwInitFonts(void)
{
	char fn[1000], s[1000];
	char n[1000], nr[1000], ni[1000], nb[1000], nbi[1000];
	int i;
	FILE *fp;

	sprintf(fn, "%s/IsoLatin1.enc", mowitz_data);
	load_encoding(fn);

	sprintf(fn, "%s/fonts.txt", mowitz_data);
	fp = fopen(fn, "r");
	if (fp == NULL) {
		fprintf(stderr, "Can't read %s\n", fn);
		return;
	}

	n[0] = '\0';
	while (fgets(s, sizeof s, fp)) {
		if (s[0] == '[') {
			i = sscanf(s, "[%[^]]", n);
			if (i != 1) {
				fprintf(stderr, "Error in %s\n", s);
			}
		} else if (!strncmp(s, "AFM=", 4)) {
			i = sscanf(s, "AFM=%[^|]|%[^|]|%[^|]|%[^\n]",
				nr, ni, nb, nbi);
			if (i != 4) {
				fprintf(stderr, "Error in %s\n", s);
			} else {
				register_afm(n, nr, ni, nb, nbi);
			}
		} else if (!strncmp(s, "PFB=", 4)) {
			i = sscanf(s, "PFB=%[^|]|%[^|]|%[^|]|%[^\n]",
				nr, ni, nb, nbi);
			if (i != 4) {
				fprintf(stderr, "Error in %s\n", s);
			} else {
				register_pfb(n, nr, ni, nb, nbi);
			}
		} else if (!strncmp(s, "X=", 2)) {
			i = sscanf(s, "X=%[^|]|%[^|]|%[^|]|%[^\n]",
				nr, ni, nb, nbi);
			if (i != 4) {
				fprintf(stderr, "Error in %s\n", s);
			} else {
				register_x_font(n, nr, ni, nb, nbi);
			}
		} else if (!strncmp(s, "PS=", 3)) {
			i = sscanf(s, "PS=%[^|]|%[^|]|%[^|]|%[^\n]",
				nr, ni, nb, nbi);
			if (i != 4) {
				fprintf(stderr, "Error in %s\n", s);
			} else {
				register_ps_font(n, nr, ni, nb, nbi);
			}
		} else if (!strncmp(s, "T1=", 3)) {
			i = sscanf(s, "T1=%[^|]|%[^|]|%[^|]|%[^\n]",
				nr, ni, nb, nbi);
			if (i != 4) {
				fprintf(stderr, "Error in %s\n", s);
			} else {
				register_t1_font(n, nr, ni, nb, nbi);
			}
		} else if (!strncmp(s, "ALIAS=", 6)) {
			i = sscanf(s, "ALIAS=%[^\n]", nr);
			if (i != 1) {
				fprintf(stderr, "Error in %s\n", s);
			} else {
				MwFontAlias(nr, n);
			}
		} else if (!strncmp(s, "ENC=", 4)) {
			/* tell us it's not Latin1 */
			register_encoding(n);
		}
	}
	fclose(fp);
}

void mw_init_format(void)
{
	static int done = 0;
	if (!done) {
		mowitz_data = getenv("MOWITZ_DATA");
		if (mowitz_data == NULL) mowitz_data = MOWITZ_DATA;
		done = 1;
		MwInitColors();
		MwInitFonts();
	}
}

/*#include "Fontstuff/richchar.c"*/

/* ---
calculates the number of chars in p
*/

size_t MwRcStrlen(MwRichchar *p)
{
	size_t i = 0;
	if (p == NULL) return i;
	while (p[i].c) i++;
	return i;
}

/* ---
copies from to to
*/

MwRichchar *MwRcStrcpy(MwRichchar *to, MwRichchar *from)
{
	size_t i = 0;
	if (!to) return NULL;
	if (!from) to[0].c = '\0';
	else for (i = 0; from[i].c; i++)
		to[i] = from[i];
	to[i].c = '\0';
	return to;
}

MwRichchar *MwRcStrncpy(MwRichchar *to, MwRichchar *from, size_t n)
{
	size_t i = 0;
	if (!to) return NULL;
	if (!from && n) to[0].c = '\0';
	else for (i = 0; i < n && from[i].c; i++)
		to[i] = from[i];
	if (i < n) to[i].c = '\0';
	return to;
}

/* ---
appends from after to
*/

static MwRichchar *rc_strcat(MwRichchar *to, MwRichchar *from)
{
	return MwRcStrcpy(to+MwRcStrlen(to), from);
}

/* ---
allocate a new copy of p
*/

MwRichchar *MwRcStrdup(MwRichchar *p)
{
	MwRichchar *q = (MwRichchar *)MwMalloc((MwRcStrlen(p)+1)
						*sizeof(MwRichchar));
	return MwRcStrcpy(q, p);
}

/* ---
allocate a new copy of p with q inserted at where
*/

MwRichchar *MwRcStrins(MwRichchar *p, MwRichchar *q, size_t where)
{
	size_t i = 0;

	MwRichchar *s = (MwRichchar *)MwMalloc((MwRcStrlen(p)+MwRcStrlen(q)+1)
						*sizeof(MwRichchar));
	for (i = 0; p && p[i].c && i < where; i++)
		s[i] = p[i];
	MwRcStrcpy(s+i, q);
	if (p) rc_strcat(s+i, p+i);
	return s;
}

/* ---
allocate a copy of "plain" string into rich char format
*/

MwRichchar *MwRcMakerich(unsigned char *p, int fmt)
{
	size_t i;
	MwRichchar *s = (MwRichchar *)MwMalloc(((p?strlen((char *)p):0)+1)
					*sizeof(MwRichchar));
	for (i = 0; p && p[i]; i++) {
		s[i].c = p[i];
		s[i].fmt = fmt;
	}
	s[i].c = '\0';
	return s;
}

/* ---
allocate a plain copy of a rich string
*/

unsigned char *MwRcMakeplain(MwRichchar *p)
{
	size_t i;

	unsigned char *s = (unsigned char *)MwMalloc((MwRcStrlen(p)+1)*
						sizeof(unsigned char));
	for (i = 0; p && p[i].c; i++) {
		s[i] = p[i].c;
	}
	s[i] = '\0';
	return s;
}

