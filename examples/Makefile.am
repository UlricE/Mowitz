INCLUDES = -I.. -DNeedFunctionPrototypes -DUSE_XPM $(X_CFLAGS)

#LDADD = ../Mowitz/.libs/libMowitz.a $(X_LIBS) $(X_PRE_LIBS) $(X_EXTRA_LIBS)
LDADD = -L../Mowitz/ -lMowitz $(X_LIBS) $(X_PRE_LIBS) $(X_EXTRA_LIBS)

noinst_PROGRAMS = allwidgets listtree listtree2 rulerdemo

allwidgets_SOURCES = allwidgets.c
# html_SOURCES = html.c
listtree_SOURCES = listtree.c
listtree2_SOURCES = listtree2.c TreeFSel.c TreeFSel.h TreeFSelP.h Directory.c Directory.h
rulerdemo_SOURCES = rulerdemo.c
