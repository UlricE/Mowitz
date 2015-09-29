SUBDIRS = Mowitz examples doc

EXTRA_DIST = README mowitz-config.sh

mowitz-config: mowitz-config.sh Makefile
	./mowitz-config.sh "-I$(includedir) @CFLAGS_CFG@" \
		"-L$(libdir) -lMowitz @LDFLAGS_CFG@" \
		"@VERSION@" > mowitz-config
	chmod 0755 mowitz-config

bin_SCRIPTS = mowitz-config

