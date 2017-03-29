#!/bin/sh
#***********************************************
# File Name  : mk
# Description: make file
# Make By    :lqf/200309129@163.com
# Date Time  :2011/06/15 
#***********************************************

UsAge="UsAge:\"./mk.sh\" or \"./mk.sh x64\" or \"./mk.sh clean\""
X64=$1
OSNAME=`uname`
#############################################
#业务处理文件路径
LIBDIR=""

#生成程序的名称
PROGRAMNAME="Humble"

#############################################
#Humble文件夹
HumbleDir="HBase HBase/event2 HBase/rijndael HBase/RSAEuro HBase/uchardet HBase/uchardet/LangModels HBase/zlib HBase/redisrand"
HumbleDir=$HumbleDir" HBase/lpeg HBase/lua5.3 HBase/luabridge HBase/luacjson HBase/mysql HBase/luasql HBase/pbc HBase/luaserialize HBase/luasocket HBase/lsnapshot"
HumbleDir=$HumbleDir" Humble"

#main函数所在文件夹
MAINDIR="Humble"

#main函数所在文件
MAINFILE="Humble.cpp"

#结果存放路径
RSTPATH="Bin"
if [ "$X64" = "x64" ]
then
    RSTPATH="BinX64"
fi

#附加包含库
INCLUDELIB="-lrt -ldl -lpthread"
INCLUDELIB=$INCLUDELIB" -levent_core -levent_extra -levent_pthreads -lmysqlclient"

#中间库文件名
LIBNAME="HumbleLib"

#不参与编译的文件,MAINFILE+其他
EXCEPTL=$MAINFILE" "

MAKEFILEPATH=`pwd`
LIBPATH="-L$MAKEFILEPATH/$RSTPATH"
ARCH="ar -rv"
INCLUDEPATH=""
OBJFILE=""
CFLAGS="-Wall -march=native"
CFLAGS=$CFLAGS" -Os -Wno-strict-aliasing"
if [ "$X64" = "x64" ]
then
    CFLAGS=$CFLAGS" -m64"
fi

CC="gcc"
GCC="g++"


LIBDIR=$HumbleDir" "$LIBDIR

GetIncludePath()
{
    for EachSub in $LIBDIR
    do
        INCLUDEPATH=$INCLUDEPATH" -I$MAKEFILEPATH/$EachSub"
    done
    
    echo ---------------------Dir---------------------------
    echo $LIBDIR

    echo ---------------------Include Dir---------------------------
    echo $INCLUDEPATH
}

IsExcePTL()
{
    for EachExec in $EXCEPTL
    do
        if [ "$EachExec" = "$1" ]
        then
            return 1
        fi
    done

    return 0
}

Make()
{
    for EachSub in $LIBDIR
    do
        echo ---------------------$EachSub--------------------------- 
        cd $EachSub
        if [ "$?" != "0" ]
        then
            exit 1
        fi

        SourceFile=`ls *.cpp 2>/dev/null`
        for EachFile in $SourceFile
        do
            IsExcePTL $EachFile
            if [ "$?" = "0" ]
            then
                echo "$GCC $CFLAGS -c $EachFile"
                $GCC $CFLAGS -c $EachFile $INCLUDEPATH
                if [ "$?" != "0" ]
                then
                    echo "---------------------Error---------------------"
                    exit 1
                fi
            fi
        done

        SourceFile=`ls *.c 2>/dev/null`
        for EachFile in $SourceFile
        do
            IsExcePTL $EachFile
            if [ "$?" = "0" ]
            then
                echo "$CC $CFLAGS -c $EachFile"
                $CC $CFLAGS -c $EachFile $INCLUDEPATH
                if [ "$?" != "0" ]
                then
                    echo "---------------------Error---------------------"
                    exit 1
                fi
            fi
        done
        
	RstFile=`ls *.o 2>/dev/null`
	for EachFile in $RstFile
        do
            if [ -f "$MAKEFILEPATH/$EachFile" ]
            then
                rm -rf $MAKEFILEPATH/$EachFile
            fi
            mv $EachFile $MAKEFILEPATH>/dev/null
        done
        cd $MAKEFILEPATH
    done

    echo ---------------------Make .a file--------------------------- 
    cd $MAKEFILEPATH
    OBJFILE=`ls *.o 2>/dev/null`
    echo "$ARCH lib$LIBNAME.a `echo $OBJFILE`"
    $ARCH lib$LIBNAME.a `echo $OBJFILE`
    rm -rf *.o
    mv lib$LIBNAME.a $RSTPATH
}

Clean()
{
    rm -rf *.o
    for EachSub in $LIBDIR
    do
	    rm -rf $MAKEFILEPATH/$EachSub/*.o
    done

    cd $RSTPATH    
    echo "start rm lib$LIBNAME.a"
    rm -rf lib$LIBNAME.a

    echo "start rm $PROGRAMNAME"
    rm -rf $PROGRAMNAME
    cd $MAKEFILEPATH
}

while [ 1 = 1 ]
do
    if [ $# -eq 1 ]
    then
        if [ "$1" = "clean" ]
        then
            Clean
            exit 0
        fi
        
        if [ "$1" = "x64" ]
        then
            break
        fi
        
        echo "$UsAge"
        exit 1
    elif [ $# -gt 1 ]
    then
        echo "$UsAge"
        exit 1
    else
        break
    fi
done

GetIncludePath
Make

#编译mian函数
if [ "$PROGRAMNAME" = "$SPACE" ]
then
    exit 0
fi

echo ---------------------cp lib---------------------------
cd $RSTPATH
if [ -f "libevent_pthreads.a" ]
then
    rm -rf libevent_pthreads.a
fi

if [ -f "libevent_extra.a" ]
then
    rm -rf libevent_extra.a
fi

if [ -f "libevent_core.a" ]
then
    rm -rf libevent_core.a
fi

alias cp="cp -f"

cp libevent_pthreads_$OSNAME.a libevent_pthreads.a
cp libevent_extra_$OSNAME.a libevent_extra.a
cp libevent_core_$OSNAME.a libevent_core.a

cd $MAKEFILEPATH/$MAINDIR

echo ---------------------Make program file---------------------------
echo "Include lib is:$INCLUDELIB"
echo "$GCC $CFLAGS -o $PROGRAMNAME $MAINFILE $LIBPATH $LIBAPP -l$LIBNAME $INCLUDELIB"
$GCC $CFLAGS -o $PROGRAMNAME $MAINFILE $LIBPATH $LIBAPP -l$LIBNAME $INCLUDELIB $INCLUDEPATH
if [ "$?" != "0" ]
then
    echo "---------------------Error---------------------"
    exit 1
fi

mv $PROGRAMNAME $MAKEFILEPATH/$RSTPATH
cd $MAKEFILEPATH

exit 0
