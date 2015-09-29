
SUBDIRS = cursor fonts pixmaps

lib_LTLIBRARIES = libMowitz.la

libMowitz_la_SOURCES = \
	MwApplicationShell.c \
	MwCanvas.c \
	MwCheck.c MwCheckME.c \
	MwCombo.c MwDialog.c MwFilesel.c \
	MwFrame.c MwGcs.c MwHandle.c \
	MwImage.c \
	MwListTree.c MwNotebook.c \
	MwRudegrid.c MwRuler.c MwSpinner.c \
	MwSlider.c MwVSlider.c \
	MwTabs.c MwTabstop.c MwTabbing.c MwTextField.c MwTooltip.c \
	MwBase.c MwBaseComp.c MwBaseConst.c MwBaseME.c MwButton.c \
	MwLineME.c MwLabelME.c MwMBButton.c \
	MwMenu.c MwMenuBar.c MwMenuButton.c \
	MwNws.c \
	MwPopText.c MwRow.c MwSButton.c MwSubME.c MwTraverse.c \
	MwAnimator.c MwRichtext.c MwTable.c \
	MwDND.c MwUtils.c MwXutils.c \
	MwFormat.c MwXFormat.c \
	MwXCC.c

mowitzincludedir=$(includedir)/Mowitz
mowitzinclude_HEADERS = Mowitz.h \
	MwApplicationShell.h MwApplicationShellP.h \
	MwCanvas.h MwCanvasP.h \
	MwCheck.h MwCheckP.h \
	MwCheckME.h MwCheckMEP.h \
	MwCombo.h MwComboP.h \
	MwDialog.h MwFilesel.h MwFileselP.h \
	MwFrame.h MwFrameP.h MwGcs.h \
	MwHandle.h MwHandleP.h \
	MwImage.h MwImageP.h \
	MwListTree.h MwListTreeP.h MwNotebook.h MwNotebookP.h \
	MwRudegrid.h MwRudegridP.h MwRuler.h MwRulerP.h \
	MwSpinner.h MwSpinnerP.h \
	MwSlider.h MwSliderP.h MwVSlider.h MwVSliderP.h \
	MwTabs.h MwTabsP.h MwTabstop.h MwTabstopP.h \
	MwTabbing.h MwTabbingP.h MwTextField.h MwTextFieldP.h \
	MwTooltip.h MwTooltipP.h \
	MwBase.h MwBaseP.h MwBaseComp.h MwBaseCompP.h \
	MwBaseConst.h MwBaseConstP.h \
	MwBaseME.h MwBaseMEP.h MwButton.h MwButtonP.h \
	MwLineME.h MwLineMEP.h MwLabelME.h MwLabelMEP.h \
	MwMBButton.h MwMBButtonP.h \
	MwMenu.h MwMenuP.h MwMenuBar.h MwMenuBarP.h \
	MwMenuButton.h MwMenuButtonP.h \
	MwNws.h \
	MwPopText.h MwPopTextP.h MwRow.h MwRowP.h \
	MwAnimator.h MwAnimatorP.h MwAnimatorTypes.h \
	MwRichtext.h MwRichtextP.h MwTable.h MwTableP.h \
	MwSButton.h MwSButtonP.h \
	MwSubME.h MwSubMEP.h MwTraverse.h \
	MwDND.h MwDNDTypes.h MwUtils.h MwXutils.h \
	MwFormat.h MwXFormat.h MwFormatP.h \
	MwXCC.h MwXCCP.h

pkgdata_DATA = FontDataBase IsoLatin1.enc IsoLatin2.enc \
	fonts.txt rgb.txt t1lib.config
pkgdata_SCRIPTS = any2xpm readpfa mkt1cfg

t1lib.config: mkt1cfg
	$(srcdir)/mkt1cfg $(pkgdatadir) > t1lib.config

libMowitz_la_LDFLAGS = -version-info 3:1:3

INCLUDES = $(DEBUG) @X_CFLAGS@ \
	-DNeedFunctionPrototypes -I.. \
	-DDEFAULT_PIXPATH=\"$(xpmdir)\" -DMOWITZ_DATA=\"$(pkgdatadir)\"

EXTRA_DIST = $(pkgdata_SCRIPTS) $(pkgdata_DATA)
