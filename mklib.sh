#!/bin/sh
#***********************************************
# File Name  : mklib
# Description: 编译所需库
# Make By    :lqf/200309129@163.com
# Date Time  :2011/06/15 
#***********************************************

UsAge="UsAge:\"./mklib.sh\" or \"./mklib.sh x64\""

X64=$1
OSName=`uname`
LibPath=`pwd`
LibPath=$LibPath/
RstPath="Bin"
echo current path $LibPath
if [ "$X64" = "x64" ]
then
    RstPath="BinX64"
fi

DepLib=DependLib

#编译---------------------------包信息---------------------------
libevent_tar=libevent-2.1.5-beta.tar.gz
libevent_path=libevent-2.1.5-beta

#编译-----------------libevent---------------------------
cd $LibPath$DepLib

rm -rf $libevent_path
if [ "$OSName" = "SunOS" ]
then
    gzcat $libevent_tar | tar xvf -
else
    tar -xvzf $libevent_tar
fi
cd $LibPath$DepLib/$libevent_path

if [ "$X64" = "x64" ]
then
    ./configure --enable-shared=no --enable-static=yes CFLAGS=-m64
else
    ./configure --enable-shared=no --enable-static=yes
fi

make

if [ -f "$LibPath$RstPath/libevent_core_$OSName.a" ]
then
    rm -rf $LibPath$RstPath/libevent_core_$OSName.a
fi
if [ -f "$LibPath$RstPath/libevent_extra_$OSName.a" ]
then
    rm -rf $LibPath$RstPath/libevent_extra_$OSName.a
fi
if [ -f "$LibPath$RstPath/libevent_pthreads_$OSName.a" ]
then
    rm -rf $LibPath$RstPath/libevent_pthreads_$OSName.a
fi

mv $LibPath$DepLib/$libevent_path/.libs/libevent_core.a $LibPath$RstPath/libevent_core_$OSName.a
mv $LibPath$DepLib/$libevent_path/.libs/libevent_extra.a $LibPath$RstPath/libevent_extra_$OSName.a
mv $LibPath$DepLib/$libevent_path/.libs/libevent_pthreads.a $LibPath$RstPath/libevent_pthreads_$OSName.a
