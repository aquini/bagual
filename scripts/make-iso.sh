#!/bin/sh
GMKISO=$(which grub2-mkrescue 2>/dev/null)
XORISO=$(which xorriso 2>/dev/null)
OS_STRING=${1:-bagual}
IMGOUT=${OS_STRING}.iso
_ARCH=${2:-i386}
ARCHDIR="arch/$_ARCH"


function __mkiso_tree()
{
	_isodir="${1}/boot/grub"
	mkdir -p $_isodir
	if [ ! -d $_isodir ]; then
		exit 1
	fi
}

function __rmiso_tree()
{
	_isodir=${1}
	if [ -d $_isodir ]; then
		rm -rf $_isodir
	fi
}

function __check_deps()
{
	if [ ! -e $GMKISO ]; then
		echo "ERROR: missing dependency: grub2-mkrescue"
		echo "  issue 'yum install grub2-tools' to fix it"
		exit 1
	fi
	if [ ! -e $XORISO ]; then
		echo "ERROR: missing dependency: xorriso"
		echo "  issue 'yum install xorriso' to fix it"
		exit 1
	fi
}

function __create_grub_cfg()
{
cat > $1 << EOF
menuentry "$OS_STRING" {
	multiboot /boot/kernel
}
EOF
}

if [ -f $ARCHDIR/kernel ]; then
	ISODIR=$(mktemp -d --tmpdir=$HOME/tmp/)
	__check_deps
	__mkiso_tree $ISODIR
	__create_grub_cfg $ISODIR/boot/grub/grub.cfg
	cp $ARCHDIR/kernel $ISODIR/boot/ || exit 1
	$GMKISO -o $IMGOUT $ISODIR
	__rmiso_tree $ISODIR
else
	echo "ERROR: kernel binary not found!"
	exit 1
fi
