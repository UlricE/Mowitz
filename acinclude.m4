AC_DEFUN(AC_CHECK_MOWITZ,
[
AC_CHECK_LIB(m, ceil)
dnl We want to use gmatch where fnmatch is not available
AC_CHECK_FUNC(fnmatch, AC_DEFINE(HAVE_FNMATCH),
	[AC_CHECK_LIB(gen, gmatch)
	AC_CHECK_FUNC(gmatch, AC_DEFINE(HAVE_GMATCH))])

AC_CHECK_LIB(X11, main, X_LIBS="-lX11 $X_LIBS", , $X_LIBS)
AC_CHECK_LIB(Xt, main, X_LIBS="-lXt $X_LIBS", , $X_LIBS)
AC_CHECK_LIB(Xext, main, X_LIBS="-lXext $X_LIBS", , $X_LIBS)
AC_CHECK_LIB(Xmu, main, X_LIBS="-lXmu $X_LIBS", , $X_LIBS)
AC_CHECK_LIB(Xpm, main, X_LIBS="-lXpm $X_LIBS",
	[echo "Can't find Xpm library"
	exit 1],
	$X_LIBS)
AC_CHECK_LIB(X11, XCreateIC, AC_DEFINE(HAVE_XCREATEIC), , $X_LIBS $x_libs)
AC_ARG_ENABLE(debugging,
	[  --enable-debugging      enable debugging],
	[ if test "$withval" != "no"; then
		CFLAGS="$CFLAGS -g"
	fi ])
AC_ARG_ENABLE(profiling,
	[  --enable-profiling      enable profiling],
	[ if test "$withval" != "no"; then
		CFLAGS="$CFLAGS -pg"
	fi ])
AC_ARG_WITH(xaw3d,
	[  --with-xaw3d            choose Xaw3d compatible library (default neXtaw)],
	[ case $withval in
	yes | no )
		XAWLIB=neXtaw
		;;
	* )
		XAWLIB=$withval
		;;
	esac ])

if test "$XAWLIB" = ""; then
	XAWLIB="neXtaw"
fi
AC_CHECK_LIB($XAWLIB, main, X_LIBS="-l$XAWLIB $X_LIBS",
[echo "$XAWLIB library not found"
echo "Read INSTALL for guidance"
exit 1],
$X_LIBS)
if test "$GCC" = "yes"; then
	CFLAGS="-Wall $CFLAGS"
fi
dnl Look for T1lib including its X part
AC_ARG_WITH(t1lib,
	[  --with-t1lib            enable t1lib],
	[ if test "$withval" != "no"; then
		AC_CHECK_LIB(t1, main, LIBT1="-lt1")
		if test "$LIBT1" != ""; then
			AC_DEFINE(HAVE_LIB_T1)
			LDFLAGS_CFG="$LIBT1 $LDFLAGS_CFG"
			LDFLAGS="$LIBT1 $LDFLAGS"
		fi
		AC_CHECK_LIB(t1x, main, LIBT1X="-lt1x", , $LIBT1 $X_LIBS -lX11)
		if test "$LIBT1X" != ""; then
			AC_DEFINE(HAVE_LIB_T1X)
			LDFLAGS_CFG="$LIBT1X $LDFLAGS_CFG"
			LDFLAGS="$LIBT1X $LDFLAGS"
		fi
	fi ])


dnl Set NARROWPROTO
case "$host_os" in
freebsd*|gnu*|irix5*|irix6*|linux-gnu*|netbsd*|openbsd*)
	CFLAGS="-DNARROWPROTO=1 $CFLAGS"
	CFLAGS_CFG="-DNARROWPROTO=1 $CFLAGS_CFG"
esac

AC_SUBST(CFLAGS_CFG)
AC_SUBST(LDFLAGS_CFG)
])
