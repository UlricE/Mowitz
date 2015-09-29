/*
    Copyright (C) 1997-2002  Ulric Eriksson <ulric@siag.nu>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston,
    MA 02111-1307, USA.
*/

/*
 * fonts.c
 *
 * Support for figuring out richtext metrics and displaying rich text.
 * Includes support for T1lib.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Intrinsic.h>
#include <X11/cursorfont.h>
#ifdef HAVE_LIB_T1X
#include <t1lib.h>
#include <t1libx.h>
#endif

#include <Mowitz/Mowitz.h>
#include <Mowitz/MwFormatP.h>

#define has_t1_font(i) (font_table[(i)].id != -1)

/* this structure holds the XLFD name including point size and resolution,
   the Postscript name and the corresponding X font structure */
typedef struct {
	int name;		/* index into fontname table */
	char *x_name;		/* encoded X font name */
	char *ps_name;		/* encoded PS font name */
	char *t1_name;		/* encoded T1 font name */
	int *afm;		/* font metrics */
	XFontStruct *fs;	/* X11 font structure */
	int id;			/* T1 font id */
	int size;		/* in decipoints */
	char bold, italic;
} siag_font;

/* this structure holds all the format properties of a single character,
   including the XLFD name, the Postscript name and foreground and
   background colours. The last field is for the special cell format
   properties used by Siag. They will be 0 for any other application
*/
typedef struct {
        int font;       /* index into font table */
        char uline, strike;
        int fg, bg;     /* index into colour table */
        int style;
        long siagfmt;   /* old-style Siag attributes */
} mw_format;

static mw_format mw_format_table[256];

static Display *dpy;

/* Here, the red, green and blue fields are present despite that
   identical fields exist in the XColor structure. This is to allow
   non-X ports.
*/
typedef struct {
        char *name;
        unsigned short red, green, blue;
        char need_init;
        XColor color;
} MwColor;

static MwColor MwColorTable[1000];
static siag_font font_table[256];
static int ncolor, nfont;

static Display *t1_dpy;
static Screen *t1_scr;
static int t1_scrn;
static int t1_nfont;
static char **t1_fontname;
static int t1_mode;

static int lookup_font(char *, int, int, int);
static int t1_getfontid(char *);

static int format_is_initialized = 0;

static void check_init(void)
{
	if (!format_is_initialized) {
		fprintf(stderr, "MwInitFormat not called\n");
		exit(1);
	}
}

static int compar(const void *p, const void *q)
{
	return strcmp(*(const char **)p, *(const char **)q);
}

/* ---
returns a null-terminated, sorted list of all known colours.
Caller must free
*/

char **MwColorList(int *n)
{
	int i;
	char **list;

	mw_init_format();

	list = (char **)MwMalloc((ncolor+1)*sizeof(char *));
	for (i = 0; i < ncolor; i++)
		list[i] = MwStrdup(MwColorTable[i].name);
	list[i] = NULL;
	qsort(list, i, sizeof(char *), compar);
	*n = i;
	return list;
}

/* ---
The index is into the font_table. The index must have come from
lookup_font, so we know that the font is already loaded.
*/

#define font_struct(i) (font_table[(i)].fs)

static int font_ascent(int index)
{
	if (has_t1_font(index)) {
#ifdef HAVE_LIB_T1X
		GLYPH *glyph = T1_SetChar(font_table[index].id, 'd',
				font_table[index].size/10.0, NULL);
		if (glyph) return glyph->metrics.ascent;
#endif
		return font_table[index].size/10;	/* BOGUS */
	} else {
		return font_struct(index)->max_bounds.ascent;
	}
}

int MwFontDescent(int index)
{
	check_init();
	if (has_t1_font(index)) {
#ifdef HAVE_LIB_T1X
		GLYPH *glyph = T1_SetChar(font_table[index].id, 'p',
				font_table[index].size/10.0, NULL);
		if (glyph) return -glyph->metrics.descent;
#endif
		return font_table[index].size/10;	/* BOGUS */
	} else {
		return font_struct(index)->max_bounds.descent;
	}
}

int MwFontHeight(int index)
{
	check_init();
#ifdef HAVE_LIB_T1X
	if (has_t1_font(index)) {
		BBox bbox = T1_GetFontBBox(font_table[index].id);
		return (bbox.ury-bbox.lly)*font_table[index].size/10000;
	}
#endif
	return font_ascent(index)+MwFontDescent(index);
}

int MwFontWidth(int index, char *s)
{
	check_init();
	if (has_t1_font(index)) {
#ifdef HAVE_LIB_T1X
		return T1_GetStringWidth(font_table[index].id,
			s, strlen(s), 0, FALSE)*font_table[index].size/10000;
#else
		return font_table[index].size/20;
#endif
	} else {
		return XTextWidth(font_struct(index), s, strlen(s));
	}
}

/* ---
index to MwColorTable. The index must come from lookup_color,
so we know that the colour has already been allocated
*/

unsigned long MwGetColor(int index)
{
	check_init();
	return MwColorTable[index].color.pixel;
}


/* ---
Take an index into the format table (i.e. a new-style format code) and
decode it into the components specified by the mask.
*/

void MwDecodeFormat(int n, long mask, MwFmt *fmt)
{
	int font;

	mw_init_format();
	font = mw_format_table[n].font;
	if (mask & MW_FMT_FAMILY) {
		fmt->family = MwFontnameTable[font_table[font].name].name;
		if (!fmt->family) fmt->family = "Helvetica";
	}
	if (mask & MW_FMT_SIZE)
		fmt->size = font_table[font].size;
	if (mask & MW_FMT_BOLD)
		fmt->bold = font_table[font].bold;
	if (mask & MW_FMT_ITALIC)
		fmt->italic = font_table[font].italic;
	if (mask & MW_FMT_ULINE)
		fmt->uline = mw_format_table[n].uline;
	if (mask & MW_FMT_STRIKE)
		fmt->strike = mw_format_table[n].strike;
	if (mask & MW_FMT_FG) {
		fmt->fg = MwColorTable[mw_format_table[n].fg].name;
		if (!fmt->fg) fmt->fg = "black";
	}
	if (mask & MW_FMT_BG) {
		fmt->bg = MwColorTable[mw_format_table[n].bg].name;
		if (!fmt->bg) fmt->bg = "white";
	}
	if (mask & MW_FMT_BORDERS)
		fmt->borders = mw_format_table[n].siagfmt & MW_BORDER_MASK;
	if (mask & MW_FMT_VADJ)
		fmt->vadj = mw_format_table[n].siagfmt & MW_VADJ_MASK;
	if (mask & MW_FMT_HADJ)
		fmt->hadj = mw_format_table[n].siagfmt & MW_HADJ_MASK;
	if (mask & MW_FMT_STYLE)
		fmt->style = mw_format_table[n].style;
}

/* ---
returns the index of a format structure with all the requested
   properties, or -1 for failure. It is assumed that the font has been
   allocated by calling lookup_font and the colors by calling
   lookup_color. lookup_format doesn't allocate any X resources.

   981106: Added "style", which is a bit tricky because we have no
   idea in this code what a style is or does: it varies between
   applications. The application encodes the style in to an integer
   and we don't care what it really means.
*/

int lookup_format(int font, int uline, int strike, int fgcolor, int bgcolor,
                int style, int siagfmt)
{
        int i;

        for (i = 0; i < MwFormatCount; i++) {
                if (mw_format_table[i].font == font &&
                        mw_format_table[i].uline == uline &&
                        mw_format_table[i].strike == strike &&
                        mw_format_table[i].fg == fgcolor &&
                        mw_format_table[i].bg == bgcolor &&
                        mw_format_table[i].style == style &&
                        mw_format_table[i].siagfmt == siagfmt)
                                return i;
        }
        MwFormatCount++;
        mw_format_table[i].font = font;
        mw_format_table[i].uline = uline;
        mw_format_table[i].strike = strike;
        mw_format_table[i].fg = fgcolor;
        mw_format_table[i].bg = bgcolor;
        mw_format_table[i].style = style;
        mw_format_table[i].siagfmt = siagfmt;
        return i;
}

/* ---
Encode the components from fmt specified by the mask into a new-style
format code, allocating colours and fonts as necessary.
*/

int MwEncodeFormat(long mask, MwFmt *fmt)
{
	char *family = "Helvetica";
	int fg = 0, bg = 0;
	int size = 120;
	int bold = 0, italic = 0, uline = 0, strike = 0;
	int siagfmt = 0;
	int style = 0;

	check_init();
	if (mask & MW_FMT_FAMILY) {
		family = MwLookupFontAlias(fmt->family);
		if (!family) family = "Helvetica";
	}
	if (mask & MW_FMT_SIZE)
		size = fmt->size;
	if (mask & MW_FMT_BOLD)
		bold = fmt->bold;
	if (mask & MW_FMT_ITALIC)
		italic = fmt->italic;
	if (mask & MW_FMT_ULINE)
		uline = fmt->uline;
	if (mask & MW_FMT_STRIKE)
		strike = fmt->strike;
	if (mask & MW_FMT_FG) {
		fg = MwLookupColor(fmt->fg);
		if (fg == -1) fg = 0;	/* black */
	}
	if (mask & MW_FMT_BG) {
		bg = MwLookupColor(fmt->bg);
		if (bg == -1) bg = 7;	/* white */
	}
	if (mask & MW_FMT_BORDERS)
		siagfmt |= fmt->borders;
	if (mask & MW_FMT_VADJ)
		siagfmt |= fmt->vadj;
	if (mask & MW_FMT_HADJ)
		siagfmt |= fmt->hadj;
	if (mask & MW_FMT_STYLE)
		style = fmt->style;
	return lookup_format(lookup_font(family, size, bold, italic),
				uline, strike, fg, bg, style, siagfmt);
}

/* ---
Here is the whole song and dance to get a format id for Siag versions >= 3.1.

Start by deciding on font family, point size, weight and slant.
Get an index into the font table by calling lookup_font with
the family name as argument along with the size, weight and slant.
The font is automatically loaded, if necessary.

Next pick foreground and background colours. The colours are loaded
by calling lookup_color with the colour name as argument. An index into
the colour table is returned.

Finally we are ready to call lookup_format. Decide if the character
should be underlined. If the application is Siag, also combine the
cell attributes. Pass the font index, the colour index, the underlining
flag and the Siag attributes to lookup_format. An index into the
format table is returned, and it is this index we can use to replace
the old style formats.

Yes, this is more complex than the old stuff. But it is also in fact
easier to extract individual pieces of the formats, since they are
normal C structures. To my eyes, font_table[format_table[fmt].font].size
is much clearer than (fmt & SIZE_MASK)>>SIZE_SHIFT. And it is less
prone to errors. And it leaves room for future expansion, for example
for overstrike and double underline, or bidirectional printing.
--- */

/* ---
stores a colour name along with the red, green and blue values,
   or overwrites an existing colour with the same name.
   The colour is not allocated until it is looked up in lookup_color.
*/

static int register_color(char *name,
	unsigned short red, unsigned short green, unsigned short blue)
{
	int i;

	for (i = 0; i < ncolor; i++)
		if (!MwStrcasecmp(name, MwColorTable[i].name)) break;
	if (i == ncolor) {
		ncolor++;
		MwColorTable[i].name = MwStrdup(name);
	}
	MwColorTable[i].red = red;
	MwColorTable[i].green = green;
	MwColorTable[i].blue = blue;
	MwColorTable[i].need_init = 1;
	return i;
}


/* ---
returns the index of a structure with the requested color, or -1
   for failure. Pixel value in MwColorTable[i].color.pixel is allocated

020128: Accept names of the form #rrggbb or other forms understood by X,
	even if they are not in rgb.txt. The new names are added at the
	end of the color table.
*/

int MwLookupColor(char *name)
{
	int i;

	mw_init_format();
	if (!name) {
		abort();
	}
	if (!dpy) return 0;
	for (i = 0; i < ncolor; i++) {
		if (!MwStrcasecmp(name, MwColorTable[i].name))
			break;
	}

	if (i == ncolor) {
		XColor color;
		MwAllocNamedColor(dpy, name, &color);
		i = register_color(name, color.red, color.green, color.blue);
	}

	if (i == ncolor) return -1;	/* fail */
	if (MwColorTable[i].need_init) {
		MwColorTable[i].color.red = MwColorTable[i].red;
		MwColorTable[i].color.green = MwColorTable[i].green;
		MwColorTable[i].color.blue = MwColorTable[i].blue;
		MwColorTable[i].color.flags = DoRed|DoGreen|DoBlue;
		MwAllocColor(dpy, None, &MwColorTable[i].color);
		MwColorTable[i].need_init = 0;
	}
	return i;
}

/* ---
for the given font with the given properties, encode X, T1 and PS strings.
example (result in x_name will vary depending on resolution):

	font = index of font with name "Helvetica"
	size = 12
	bold = 0
	italic = 1;

	x_name = "-adobe-helvetica-medium-o-*--*-120-75-75-*-*-iso8859-1"
	t1_name = "Helvetica-Oblique"
	ps_name = "Helvetica-Oblique"

t1_name may be NULL. The caller must then use the X font.
*/

static void encode_font(int font, int size, int bold, int italic, float zoom,
		char *x_name, char **t1_name, char **ps_name, int **afm)
{
	char *x_fmt;
	int i = 0;
	int res_x, res_y;	/* screen resolution */
	res_x = res_y = 72*zoom;

	if (bold) i += 2;
	if (italic) i += 1;

	x_fmt = MwFontnameTable[font].x_name[i];
	if (!x_fmt) x_fmt = MwFontnameTable[0].x_name[0];
	sprintf(x_name, x_fmt, size, res_x, res_y);
	*t1_name = MwFontnameTable[font].t1_name[i];
	*ps_name = MwFontnameTable[font].ps_name[i];
	*afm = MwFontnameTable[font].afm[i];
}


static struct {
	char *name;
	XFontStruct *fs;
} *loaded_fonts = NULL;

static XFontStruct *load_cached_font(char *result, char *family, int size,
		int bold, int italic, float zoom)
{
	char *t1_name, *ps_name;
	int i, *afm;
	XFontStruct *fs;

	encode_font(MwLookupFontname(family),
		size, bold, italic, zoom, result, &t1_name, &ps_name, &afm);
	if (loaded_fonts == NULL) {
		loaded_fonts = MwMalloc(1*sizeof *loaded_fonts);
		loaded_fonts[0].name = NULL;
	}
	for (i = 0; loaded_fonts[i].name; i++) {
		if (!strcmp(loaded_fonts[i].name, result))
			return loaded_fonts[i].fs;
	}
	loaded_fonts = MwRealloc(loaded_fonts, (i+2)*sizeof(*loaded_fonts));
	loaded_fonts[i].name = MwStrdup(result);
	fs = XLoadQueryFont(dpy, result);
	if (fs == NULL) {
		fs = XLoadQueryFont(dpy, "fixed");
	}
	loaded_fonts[i++].fs = fs;
	loaded_fonts[i].name = 0;
	return fs;
}

/* ---
return index into font table or -1 for failure. Load font if necessary.
This function is wasteful in that it always loads an X font, even if
a T1 font is found and will be used.
*/

static int lookup_font(char *fname, int size, int bold, int italic)
{
	int i, name, *afm;
	char x_name[1000], *t1_name, *ps_name;

	name = MwLookupFontname(fname);
	if (name == -1) name = 0;	/* fallback value */

	for (i = 0; i < nfont; i++) {
		if (font_table[i].name == name &&
			font_table[i].size == size &&
			font_table[i].bold == bold &&
			font_table[i].italic == italic)
				return i;	/* already loaded */
	}
	nfont++;
	font_table[i].name = name;
	font_table[i].size = size;
	font_table[i].bold = bold;
	font_table[i].italic = italic;
	encode_font(name, size, bold, italic, 1.0,
		x_name, &t1_name, &ps_name, &afm);
	font_table[i].x_name = MwStrdup(x_name);
	if (ps_name) font_table[i].ps_name = MwStrdup(ps_name);
	else font_table[i].ps_name = NULL;
	if (t1_name) font_table[i].t1_name = MwStrdup(t1_name);
	else font_table[i].t1_name = NULL;
	font_table[i].afm = afm;
	font_table[i].fs = XLoadQueryFont(dpy, x_name);
	font_table[i].id = t1_getfontid(t1_name);
	if (font_table[i].fs == NULL) {
		font_table[i].fs =
			XLoadQueryFont(dpy, "*helvetica-medium-r*12*");
	}
	if (font_table[i].fs == NULL) {
		fprintf(stderr, "Panic: can't load any fonts!\n");
		exit(EXIT_FAILURE);
	}
	return i;
}

static int tmpncolor;
static struct colors {
        int r, g, b;  
        char *n;
} *tmpcolor;
           
static void scan_name(char *from, char *to)
{          
        int c = ' ';
        while (*from) {
                if (isspace(c) && islower(*from)) {
                        *to++ = c = toupper(*from++);
                } else if (isupper(*from)) {
                        if (!isspace(c)) *to++ = ' ';
                        *to++ = c = *from++;
                } else {
                        *to++ = c = *from++;
                }
        }
        *to = '\0';
}

static void insert_color(int r, int g, int b, char *n)
{
        int i;
        char n2[1000];
        scan_name(n, n2);
        for (i = 0; i < tmpncolor; i++) {
                if (!strcmp(n2, tmpcolor[i].n)) break;
        }
        if (i == tmpncolor) {
                tmpncolor++;
                tmpcolor = MwRealloc(tmpcolor, tmpncolor*sizeof *tmpcolor);
        } else {
                MwFree(tmpcolor[i].n);
        }
        tmpcolor[i].r = r;
        tmpcolor[i].g = g;
        tmpcolor[i].b = b;
        tmpcolor[i].n = MwMalloc(strlen(n2)+1);
        strcpy(tmpcolor[i].n, n2);
}           

static int ccompar(const void *p, const void *q)
{           
        const struct colors *c1 = p;
        const struct colors *c2 = q;
        return strcmp(c1->n, c2->n);
}

static int sc(int n)
{               
        return n*65535/255;
}               

void MwInitColors(void)
{
        char fn[1000], s[1000], n[1000];
        FILE *fp;
        int r, g, b, i;

        sprintf(fn, "%s/rgb.txt", mowitz_data);
        fp = fopen(fn, "r");
        if (fp == NULL) {
                fprintf(stderr, "Can't read %s\n", fn);
                return;
        }

        while (fgets(s, sizeof s, fp)) {
                if (s[0] != '!') {      
                        i = sscanf(s, "%d %d %d %[^\n]", &r, &g, &b, n);
                        if (i == 4) {
                                insert_color(r, g, b, n);
                        }
                }
        }
        fclose(fp);
        qsort(tmpcolor, tmpncolor, sizeof *tmpcolor, ccompar);
        for (i = 0; i < tmpncolor; i++) {
                register_color(tmpcolor[i].n, sc(tmpcolor[i].r),
                        sc(tmpcolor[i].g), sc(tmpcolor[i].b));
        }
}


#if 0	/* unused */
/* ---
fmt is an index into MwFontnameTable
*/

static char *family2name(int fmt)
{
	return MwFontnameTable[fmt].name;
}

/* ---
fmt is index into font_table
*/

static char *size2name(int fmt)
{
	static char b[80];

	sprintf(b, "%d", font_table[fmt].size/10);
	return b;
}

/* ---
fmt is index into MwColorTable
*/

static char *color2name(int fmt)
{
	return MwColorTable[fmt].name;
}
#endif	/* unused */

/* ---
Postscript font name
*/

static char *ps_fontname(int fmt)
{
	char *p = font_table[mw_format_table[fmt].font].ps_name;
	if (p == NULL) p = font_table[mw_format_table[fmt].font].t1_name;
	if (p == NULL) p = font_table[0].ps_name;
	return p;
}

/* ---
Postscript font size
*/

static int ps_fontsize(int fmt)
{
	return font_table[mw_format_table[fmt].font].size / 10;
}

static int lastfont;

/* This translates ps fonts in .pfb format to ASCII ps files. */
    
/* 2000-10-26 Snarfed from Groff by Ulric */

/* Binary bytes per output line. */
#define BYTES_PER_LINE (64/2)
#define HEX_DIGITS "0123456789abcdef"

#define error(p) { fprintf(stderr, "%s\n", p); return 1; }

static int pfb2ps(FILE *fpin, FILE *fpout)
{   
        if (fpin == NULL || fpout == NULL) error("no file");
        for (;;) {
                int type, c, i;
                long n;        
       
                c = getc(fpin);
                if (c != 0x80) {
                        /* first byte of packet not 0x80, treat as ascii */
                        do {                                               
                                putc(c, fpout);
                                c = getc(fpin);
                        } while (c != EOF);
                        return 0;
                }
                type = getc(fpin);
                if (type == 3) break;
                if (type != 1 && type != 2) error("bad packet type");
                n = 0;
                for (i = 0; i < 4; i++) {
                        c = getc(fpin);
                        if (c == EOF) error("end of file in packet header");
                        n |= (long) c << (i << 3);
                }
                if (n < 0) error("negative packet length");
                if (type == 1) {
                        while (--n >= 0) {
                                c = getc(fpin);
                                if (c == EOF)
                                        error("end of file in text packet");
                                if (c == '\r') c = '\n';
                                putc(c, fpout);
                        }
                        if (c != '\n') putc('\n', fpout);
                } else {
                        int count = 0;
                        while (--n >= 0) {
                                c = getc(fpin);
                                if (c == EOF)
                                        error("end of file in binary packet");
                                if (count >= BYTES_PER_LINE) {
                                        putc('\n', fpout);
                                        count = 0;
                                }
                                count++;
                                putc(HEX_DIGITS[(c >> 4) & 0xf], fpout);
                                putc(HEX_DIGITS[c & 0xf], fpout);
                        }
                        putc('\n', fpout);
                }
        }
        return 0;
}

/* ---
Define Latin-1 fonts

Improved: only makes the fonts that are actually used.

This is a mess, but here is a strategy I believe in. Go through the entire
format table, looking for font, italic and bold information. For each
format, call a function that figures out whether to use ps or t1 fonts,
encodes it, uploads the font if necessary and adds it to a list of "done"
fonts. Fonts that have already been done are not repeated even if they are
found again in the format table.

Because of the way the font system works (the same font may be used for
regular and bold, for example), it is necessary to encode the fonts
before checking if it is already done.
*/

static struct done {
	char *name;
	struct done *next;
} *done_fonts;

static void ps_makefont(FILE *fp, int id, char *ps_name, int pfab,
			char *enc, char *pfb)
{
	struct done *df;

	for (df = done_fonts; df; df = df->next)
		if (!strcmp(df->name, ps_name)) return;

	df = MwMalloc(sizeof(struct done));
	df->name = MwStrdup(ps_name);
	df->next = done_fonts;
	done_fonts = df;

	if (pfab == 0 && pfb != NULL) {
		FILE *fpin;
		char b[1024];

		sprintf(b, "%s/fonts/%s", mowitz_data, pfb);
		fpin = fopen(b, "r");
		if (fpin) {
			pfb2ps(fpin, fp);
			fclose(fpin);
		}
	}

#ifdef HAVE_LIB_T1X
	if (pfab) {	/* dump font into ps file */
#if 1
		FILE *fpin;
		char b[1024];

		if (id == -1) return;
		sprintf(b, "%s/fonts/%s", mowitz_data,
			T1_GetFontFileName(id));
		fpin = fopen(b, "r");
		if (fpin) {
			pfb2ps(fpin, fp);
			fclose(fpin);
		}
#else
		char b[1024], cmd[10000];
		FILE *ffp;
		if (id == -1) return;
		sprintf(cmd, "%s/readpfa %s %s", mowitz_data,
			T1_GetFileSearchPath(T1_PFAB_PATH),
			T1_GetFontFileName(id));
		ffp = popen(cmd, "r");
		if (ffp == NULL) {
			fprintf(stderr, "Couldn't read from %s\n", cmd);
			return;
		}
		while (fgets(b, sizeof b, ffp))
			fputs(b, fp);
		pclose(ffp);
		/* and presto, we have a font */
#endif
	}
#endif
	if (!enc) return;	/* don't bother */

	fprintf(fp, "/%s findfont\n", ps_name);

	fprintf(fp, "dup length dict begin\n");
	fprintf(fp, "  {1 index /FID ne {def} {pop pop} ifelse} forall\n");
	fprintf(fp, "  /Encoding %sEncoding def\n", enc);
	fprintf(fp, "  currentdict\n");
	fprintf(fp, "end\n");
	fprintf(fp, "/%s-%s exch definefont pop\n", ps_name, enc);
}

void MwPsMakeFonts(FILE *fp)
{
	int i, j;
	struct done *df;

	check_init();
	done_fonts = NULL;

	for (i = 0; i < MwFormatCount; i++) {
		char *ps_name, *enc;
		int fi = mw_format_table[i].font;	/* index to font_table */
		int id = font_table[fi].id;
		int bold = font_table[fi].bold;
		int italic = font_table[fi].italic;
		int name = font_table[fi].name;
		j = 0;
		if (bold) j += 2;
		if (italic) j += 1;

		if (MwFontnameTable[name].iso8859_1) enc = "ISOLatin1";
		else enc = NULL;

		ps_name = MwFontnameTable[name].ps_name[j];
		if (ps_name) {
			ps_makefont(fp, id, ps_name, 0, enc,
				MwFontnameTable[name].pfb[j]);
		} else {
			ps_name = MwFontnameTable[name].t1_name[j];
			if (!ps_name) continue;		/* give up */
			ps_makefont(fp, id, ps_name, 1, enc,
				MwFontnameTable[name].pfb[j]);
		}
	}
	while (done_fonts) {
		df = done_fonts->next;
		MwFree(done_fonts->name);
		MwFree(done_fonts);
		done_fonts = df;
	}
}

static MwColor lastcolor;

void MwPsSetColor(FILE *fp, int red, int green, int blue)
{
	check_init();
	if (red == lastcolor.red
	    && green == lastcolor.green
	    && blue == lastcolor.blue) return;
	lastcolor.red = red;
	lastcolor.green = green;
	lastcolor.blue = blue;

	if (red == -1) return;		/* starter mark */

	fprintf(fp, "%f %f %f setrgbcolor\n",
		(float)red / 65535.0,
		(float)green / 65535.0,
		(float)blue / 65535.0);
}

/* ---
Set the font
*/

void MwPsSetFont(FILE *fp, int newfont)
{
	int name;
	MwColor newcolor;

	check_init();
	/* always set color */
	if (newfont == -1) {
		MwPsSetColor(fp, 0, 0, 0);
	} else {
		newcolor = MwColorTable[mw_format_table[newfont].fg];
		MwPsSetColor(fp, newcolor.red, newcolor.green, newcolor.blue);
	}

	if (newfont == lastfont) {
		return;
	}
	lastfont = newfont;
	if (newfont == -1) {
		return;	/* starter mark */
	}

	name = font_table[mw_format_table[newfont].font].name;

	fprintf(fp, "/%s%s findfont\n",
		ps_fontname(newfont),
		MwFontnameTable[name].iso8859_1?"-ISOLatin1":"");
	fprintf(fp, "%d scalefont\n", ps_fontsize(newfont));
	fprintf(fp, "setfont\n");
}


/* ---
   Code to handle a combination of X and T1 fonts. Currently pretty
   suboptimal: makes several passes over strings, doesn't use kerning
   or ligatures.

   000425: added zoom.
--- */

/* X code borrowed from Richchar.c */

float MwRcWidth(MwRichchar c)
{
	int w, index, n, x, *a;

	check_init();
	index = mw_format_table[c.fmt].font;
	n = font_table[index].name;		/* index into MwFontnameTable */
	x = (font_table[index].bold?2:0)+(font_table[index].italic?1:0);
	a = MwFontnameTable[n].afm[x];	/* widths */

	if (a) {
		w = a[c.c];
		if (w == 0) {
			/*printf("0 for %d (%c)\n", c.c, c.c);*/
			w = a[0];
		}
		return (float)w*font_table[index].size/10000;
	} else {
		;
	}

	if (has_t1_font(index)) {
#ifdef HAVE_LIB_T1X
		return (float)T1_GetCharWidth(font_table[index].id, c.c)
			*font_table[index].size/10000;
#else
		return (float)font_table[index].size/10;	/* BOGUS */
#endif
	} else {
		w = XTextWidth(font_struct(index), &c.c, 1);
	}
	return w;
}

float MwRcStrwidth(MwRichchar *p, int len)
{
	int i;
	float w = 0;

	check_init();
	if (len == -1) len = MwRcStrlen(p);

	for (i = 0; i < len; i++) {
		w += MwRcWidth(p[i]);
	}
	return w;
}

static int rc_height(MwRichchar c)
{
	int index = mw_format_table[c.fmt].font;

	return MwFontHeight(index);
}

int MwRcStrheight(MwRichchar *p, int len)
{
	int i, h = 0;

	check_init();
	if (len == -1) len = MwRcStrlen(p);

	for (i = 0; i < len; i++) {
		int h1 = rc_height(p[i]);
		if (h1 > h) h = h1;
	}
	return h;
}


/*
x and y are the unzoomed coordinates, i.e. if x=10 and zoom=2.0,
the text will be drawn at x=20.
*/
static int rc_draw(Drawable d, GC gc, int xo, int yo,
		int x, int y, MwRichchar c, int opaque, float zoom)
{
	static int format_index = -1;
	static MwFmt fmt;
	static Font font;
	static float oldzoom = -1;
	XFontStruct *font_struct;
	char xfont[1000];
	int index, color_index;
	int vadj, width;
	unsigned long color;

	if (c.fmt != format_index || zoom != oldzoom) {
		format_index = c.fmt;
		oldzoom = zoom;
		MwDecodeFormat(format_index,
			MW_FMT_FAMILY | MW_FMT_SIZE | MW_FMT_BOLD | MW_FMT_ITALIC
			 | MW_FMT_ULINE | MW_FMT_FG | MW_FMT_BG | MW_FMT_VADJ
			 | MW_FMT_HADJ, &fmt);
		font_struct = load_cached_font(xfont, fmt.family,
				fmt.size, fmt.bold, fmt.italic, zoom);
		font = font_struct->fid;
		color_index = mw_format_table[c.fmt].fg;
		color = MwGetColor(color_index);
		XSetForeground(t1_dpy, gc, color);

	}
	index = mw_format_table[c.fmt].font;
	switch (mw_format_table[c.fmt].siagfmt & MW_VADJ_MASK) {
	case MW_VADJ_TOP:
		vadj = -6;
		break;
	case MW_VADJ_BOTTOM:
		vadj = 6;
		break;
	default:
		vadj = 0;
	}

	if (has_t1_font(index)) {
#ifdef HAVE_LIB_T1X
		T1_SetCharX(d, gc, t1_mode, xo+zoom*x, yo+zoom*(y+vadj),
			font_table[index].id, c.c,
			zoom*font_table[index].size/10.0, NULL);
if (opaque) fprintf(stderr, "Warning: can't draw opaque\n");
#endif
	} else {
		XSetFont(t1_dpy, gc, font);
		if (opaque) XDrawImageString(t1_dpy, d, gc,
				xo+zoom*x, yo+zoom*(y+vadj), &c.c, 1);
		else XDrawString(t1_dpy, d, gc,
				xo+zoom*x, yo+zoom*(y+vadj), &c.c, 1);
	}
	if (mw_format_table[c.fmt].uline) {
		width = MwRcWidth(c);
		XDrawLine(t1_dpy, d, gc,
			xo+zoom*x, yo+zoom*(y+1),
			xo+zoom*(x+width), yo+zoom*(y+1));
	}
	if (mw_format_table[c.fmt].strike) {
		width = MwRcWidth(c);
		XDrawLine(t1_dpy, d, gc,
			xo+zoom*x, yo+zoom*(y-4),
			xo+zoom*(x+width), yo+zoom*(y-4));
	}
	return 0;
}


int MwRcStrdraw(Drawable d, GC gc, int xo, int yo,
		int x, int y, MwRichchar *p, int len, float zoom)
{
	int i;
	float x0 = x;

	check_init();
	if (len == -1) len = MwRcStrlen(p);

	for (i = 0; i < len; i++) {
		rc_draw(d, gc, xo, yo, x0, y, p[i], 0, zoom);
		x0 += MwRcWidth(p[i]);
	}
	return 0;
}

static int t1_getfontid(char *name)
{
	int i;

	if (!name) return -1;
	for (i = 0; i < t1_nfont; i++) {
		if (t1_fontname[i] && !strcmp(t1_fontname[i], name))
			return i;
	}
	return -1;
}

#if 0	/* unused */
static char *t1_getfontname(int id)
{
	if (id < 0 || id >= t1_nfont) return NULL;
	return t1_fontname[id];
}
#endif	/* unused */

int MwT1Init(Display *dpy)
{
#ifdef HAVE_LIB_T1X
	int i;
	char **enc;
#endif

	t1_dpy = dpy;
	t1_scr = DefaultScreenOfDisplay(t1_dpy);
	t1_scrn = DefaultScreen(t1_dpy);
	t1_mode = 0;

#ifdef HAVE_LIB_T1X
	if (!getenv("T1LIB_CONFIG")) {
		char dir[1024];
		sprintf(dir, "T1LIB_CONFIG=%s/t1lib.config", mowitz_data);
		putenv(dir);
	}

	T1_SetBitmapPad(16);

        if ((T1_InitLib(NO_LOGFILE) == NULL)) {
                fprintf(stderr, "Initialization of t1lib failed\n");
                return -1;
        }

        t1_nfont = T1_Get_no_fonts();

        T1_SetX11Params(t1_dpy, DefaultVisual(t1_dpy, t1_scrn),
                DefaultDepth(t1_dpy, t1_scrn),
                DefaultColormap(t1_dpy, t1_scrn));

        enc = T1_LoadEncoding("IsoLatin1.enc");
        if (enc == NULL) {
                fprintf(stderr, "Couldn't load encoding\n");
                exit(EXIT_FAILURE);
        } else {
                T1_SetDefaultEncoding(enc);
        }

        t1_fontname = MwMalloc(t1_nfont*sizeof *t1_fontname);
        for (i = 0; i < t1_nfont; i++) {
		char *p;
		T1_LoadFont(i);
		p = T1_GetFontName(i);
		T1_DeleteFont(i);
		if (p == NULL) t1_fontname[i] = NULL;
                else t1_fontname[i] = MwStrdup(p);
        }
#endif	/* HAVE_LIB_T1X */
        return 0;
}

#if 0	/* unused */
static int t1_exit(void)
{
#ifdef HAVE_LIB_T1X
        T1_CloseLib();
#endif
	return 0;
}
#endif	/* unused */

/* ---
Initialise the X format code.
*/

void MwInitFormat(Display *d)
{
	if (!format_is_initialized) {
		format_is_initialized = 1;
		dpy = d;
		mw_init_format();
		MwT1Init(dpy);
	}
}

