#!/bin/sh

cat << EOF
#!/bin/sh

usage()
{
	echo "usage: \$0 [--cflags] [--libs] [--version]"
	exit 0
}

cflags=no
libs=no
version=no

test "\$1" || usage

while test "\$1"; do
	case "\$1" in
	--cflags )
		cflags=yes
		;;
	--libs )
		libs=yes
		;;
	--version )
		version=yes
		;;
	* )
		usage
		;;
	esac
	shift
done

test "\$cflags" = yes && cat << FOO
$1
FOO

test "\$libs" = yes && cat << FOO
$2
FOO

test "\$version" = yes && cat << FOO
$3
FOO

EOF
