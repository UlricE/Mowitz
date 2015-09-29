#ifndef MW_FORMATP_H
#define MW_FORMATP_H

/*
This file should be included by MwFormat.c and MwXFormat.c, nowhere
else. It declares structures that are private to those two modules.

In particular, it is an error for any application to include the file.
*/

/* A structure with the name seen by the user followed by four X font names
   in XLFD format and four PS font names. The four are for plain, italic,
   bold and bold italic. The x font names are of this format (example):
   -*-helvetica-medium-o-*--*-%d-%d-%d-*-*-*-*
   The three %d fields are for point size in decipoints, x resolution
   and y resolution. They are filled in by encode_font.
*/
typedef struct {
	char *name;
	int *afm[4];
	char *pfb[4];
	char *x_name[4];
	char *ps_name[4];
	char *t1_name[4];
	int iso8859_1;	/* should we try to do IsoLatin1 encoding? */
} MwFontname;

extern MwFontname MwFontnameTable[256];

extern int MwLookupFontname(char *);

extern void mw_init_format(void);
extern void MwInitColors(void);	/* in MwXFormat.c */
extern char *mowitz_data;

#endif	/* MW_FORMATP_H */
