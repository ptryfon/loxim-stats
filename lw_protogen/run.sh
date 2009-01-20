#!/bin/sh

#tune it to your needs
export XERCESC_NLS_HOME=/usr/share/xerces-c/msg
XML=loxim2.xml
TARGET=../src/Protocol
#needs to be ralative!!!
SKEL=skel
echo "Cleaning target directory"
TO_REMOVE=`find $TARGET | grep -v svn | grep -E "(\.(h|cpp)|Makefile.am)$" `
echo "About to remove the following files"
for a in $TO_REMOVE ; do
	echo rm $a
done
echo "Do it? (ctrl-c to abort)"
read
for a in $TO_REMOVE ; do
	rm $a
done
TO_COPY=`find $SKEL -type f | grep -v svn | grep -v $SKEL\$`
SKEL_LEN=${#SKEL}
echo "About to copy the following files"
for a in $TO_COPY ; do
	CUT_NAME=`echo $a| cut -c$(( $SKEL_LEN + 2 ))-`
	echo cp $a $TARGET/$CUT_NAME
done
echo "Do it? (ctrl-c to abort)"
read
for a in $TO_COPY ; do
	CUT_NAME=`echo $a| cut -c$(( $SKEL_LEN + 2 ))-`
	if [ "`echo -n $a | grep "\.am$"`" != "" ] ; then 
		echo "## DO NOT MODIFY, this file is auto generated using lw_protogen" > $TARGET/$CUT_NAME
	fi
	if [ "`echo -n $a | grep -E "\.(cpp|h)$"`" != "" ] ; then echo
		echo "/* DO NOT MODIFY, this file is auto generated using lw_protogen*/" > $TARGET/$CUT_NAME
	fi
	cat $a >> $TARGET/$CUT_NAME
done
echo "Launching: ./lw_protogen $XML $TARGET"
if ./lw_protogen $XML $TARGET ; then
	echo 'Success!'
else
	echo 'Failed :('
fi

