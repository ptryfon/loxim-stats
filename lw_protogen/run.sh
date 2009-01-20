#!/bin/sh

#tune it to your needs
export XERCESC_NLS_HOME=/usr/share/xerces-c/msg
XML=loxim2.xml
TARGET=../src/Protocol
SKEL=skel
echo "Cleaning target directory"
#rm -rf $TARGET
cp -Rf $SKEL $TARGET
echo "Launching: ./lw_protogen $XML $TARGET"
if ./lw_protogen $XML $TARGET ; then
	echo 'Success!'
else
	echo 'Failed :('
fi

