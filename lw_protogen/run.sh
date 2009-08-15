#!/bin/sh
if [ ! -x ./lw_protogen ] ; then 
	echo "First type make to compile lw_protogen and ensure that you have xerces-c 3.0"
	exit 1
fi
#tune it to your needs
export XERCESC_NLS_HOME=/usr/share/xerces-c/msg
XML=loxim2.xml
TARGET=../src/Protocol
#needs to be ralative!!!
SKEL=skel
echo "Cleaning target directory"
TO_REMOVE=`find $TARGET | grep -v svn | grep -E "(\.(h|cpp)|CMakeLists\.txt)$" `
echo "About to remove the following files"
for a in $TO_REMOVE ; do
	echo rm $a
done
echo "Do it? (ctrl-c to abort)"
read a
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
read a
for a in $TO_COPY ; do
	CUT_NAME=`echo $a| cut -c$(( $SKEL_LEN + 2 ))-`
	if [ "`echo -n $a | grep "CMakeLists.txt$"`" != "" ] ; then 
		echo "## DO NOT MODIFY, this file is auto generated using lw_protogen" > $TARGET/$CUT_NAME
	fi
	if [ "`echo -n $a | grep -E "\.(cpp|h)$"`" != "" ] ; then echo
		echo "/* DO NOT MODIFY, this file is auto generated using lw_protogen*/" > $TARGET/$CUT_NAME
	fi
	cat $a >> $TARGET/$CUT_NAME
done
echo "Launching: ./lw_protogen $XML $TARGET"
export LD_LIBRARY_PATH=/usr/local/lib
if ./lw_protogen $XML $TARGET ; then
	echo 'Success!'
	echo "Remember to run cmake in the project's directory an reconfigure the project"
else
	echo 'Failed :('
fi

