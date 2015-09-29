#ifndef	MwGcs_h

extern	GC	AllocFgGC( Widget w, Pixel fg, Font font) ;
extern	GC	AllocBackgroundGC( Widget w, Font font) ;
extern	GC	AllocGreyGC( Widget w, Pixel fg, Font, int, int ) ;
extern	GC	AllocTopShadowGC( Widget w, int contrast, int ) ;
extern	GC	AllocBotShadowGC( Widget w, int contrast, int ) ;
extern	GC	AllocArmGC( Widget w, int contrast, int) ;
extern	Pixel	AllocShadowPixel(Widget, int scale) ;
extern	Pixmap	GetGrey50(Widget, Pixmap *);
extern	Pixel	AllocGreyPixel(Widget, Pixel fg, Pixel bg, int contrast) ;
extern	void	Draw3dBox(Widget w, int  x, int y, int wid, int hgt, int s,
			GC topgc, GC botgc, GC tophalf, GC bothalf) ;

#define	MwGcs_h
#endif
