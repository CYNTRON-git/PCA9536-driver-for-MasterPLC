#!/bin/bash

case "$1" in
    help)
        echo "makedrv.sh platform [wtc]"
        echo " *  platform - compiling platform"
        
        echo "Supported platforms:"
        echo " *  linux-x86"
        echo " *  linux-x64"
        echo " *  linux-armv7hf"
        echo " *  linux-armv8"
        echo " *  abak-k2"
        echo " *  abak-k3"
        echo " *  baget"
        echo " *  baikal-m"
        echo " *  bolid"
        echo " *  bolid-m3000t-v2"
        echo " *  fimatic-c"
        echo " *  kvantor"
        echo " *  nls-con-rsb"
        echo " *  oni-plc-w"
        echo " *  osatec-x64"
        echo " *  plc210"
        echo " *  titan"
        echo " *  titan2000"
        echo " *  wirenboard6"
        echo " *  wirenboard7"
        exit 0;
        ;;
    linux-armv7hf | nls-con-rsb)
        export PATH=/opt/arm-mpssoft-linux-gnueabihf/bin:$PATH
        export CXX=arm-mpssoft-linux-gnueabihf-g++
        export CC=arm-mpssoft-linux-gnueabihf-gcc
        export LDFLAGS=-latomic
        export CXXFLAGS+=-Wno-psabi -Wno-narrowing
        ;;
    optilogic)
        export PATH=/opt/arm-mpssoft-linux-gnueabihf/bin:$PATH
        export CXX=arm-mpssoft-linux-gnueabihf-g++
        export CC=arm-mpssoft-linux-gnueabihf-gcc
        export LDFLAGS=-latomic
        export CFLAGS+=-mcpu=cortex-a5+nosimd
        export CXXFLAGS+=-mcpu=cortex-a5+nosimd -Wno-psabi -Wno-narrowing
        ;;
    kvantor | oni-plc-w | wirenboard6 | wirenboard7)
        export PATH=/opt/arm-mpssoft-linux-gnueabihf/bin:$PATH
        export CXX=arm-mpssoft-linux-gnueabihf-g++
        export CC=arm-mpssoft-linux-gnueabihf-gcc
        export LDFLAGS=-latomic
        export CFLAGS+=-mcpu=cortex-a7
        export CXXFLAGS+=-mcpu=cortex-a7 -Wno-psabi -Wno-narrowing
        ;;
    abak-k2 | abak-k3)
        export PATH=/opt/arm-mpssoft-linux-gnueabihf/bin:$PATH
        export CXX=arm-mpssoft-linux-gnueabihf-g++
        export CC=arm-mpssoft-linux-gnueabihf-gcc
        export LDFLAGS=-latomic
        export CFLAGS+=-march=armv7-a+vfpv3+simd
        export CXXFLAGS+=-march=armv7-a+vfpv3+simd
        ;;
    fimatic-c | plc210)
        export PATH=/opt/arm-mpssoft-linux-gnueabihf/bin:$PATH
        export CXX=arm-mpssoft-linux-gnueabihf-g++
        export CC=arm-mpssoft-linux-gnueabihf-gcc
        export LDFLAGS=-latomic
        export CFLAGS+=-mcpu=cortex-a8
        export CXXFLAGS+=-mcpu=cortex-a8 -Wno-psabi -Wno-narrowing
        ;;
    bolid)
        export PATH=/opt/arm-mpssoft-linux-gnueabihf/bin:$PATH
        export CXX=arm-mpssoft-linux-gnueabihf-g++
        export CC=arm-mpssoft-linux-gnueabihf-gcc
        export LDFLAGS=-latomic
        export CFLAGS+=-mtune=cortex-a9
        export CXXFLAGS+=-mtune=cortex-a9 -Wno-psabi -Wno-narrowing
        ;;
    bolid-m3000t-v2)
        export PATH=/opt/arm-mpssoft-linux-gnueabihf/bin:$PATH
        export CXX=arm-mpssoft-linux-gnueabihf-g++
        export CC=arm-mpssoft-linux-gnueabihf-gcc
        export LDFLAGS=-latomic
        export CFLAGS+=-mcpu=cortex-a9
        export CXXFLAGS+=-mcpu=cortex-a9 -Wno-psabi -Wno-narrowing
        ;;

    linux-armv8)
        export PATH=/opt/aarch64-mpssoft-linux-gnu/bin:$PATH
        export CXX=aarch64-mpssoft-linux-gnu-g++
        export CC=aarch64-mpssoft-linux-gnu-gcc
        export CXXFLAGS+=-Wno-psabi -Wno-narrowing
        ;;
    titan)
        export PATH=/opt/aarch64-mpssoft-linux-gnu/bin:$PATH
        export CXX=aarch64-mpssoft-linux-gnu-g++
        export CC=aarch64-mpssoft-linux-gnu-gcc
        export CFLAGS+=-mcpu=cortex-a53
        export CXXFLAGS+=-mcpu=cortex-a53 -Wno-psabi -Wno-narrowing
        ;;
    titan2000)
        export PATH=/opt/aarch64-mpssoft-linux-gnu/bin:$PATH
        export CXX=aarch64-mpssoft-linux-gnu-g++
        export CC=aarch64-mpssoft-linux-gnu-gcc
        export CFLAGS+=-mcpu=cortex-a55
        export CXXFLAGS+=-mcpu=cortex-a55 -Wno-psabi -Wno-narrowing
        ;;
    baikal-m)
        export PATH=/opt/aarch64-mpssoft-linux-gnu/bin:$PATH
        export CXX=aarch64-mpssoft-linux-gnu-g++
        export CC=aarch64-mpssoft-linux-gnu-gcc
        export CFLAGS+=-mcpu=cortex-a57
        export CXXFLAGS+=-mcpu=cortex-a57 -Wno-psabi -Wno-narrowing
        ;;

    linux-x64 | osatec-x64)
        export PATH=/opt/x86_64-mpssoft-linux-gnu/bin:$PATH
        export CXX=x86_64-mpssoft-linux-gnu-g++
        export CC=x86_64-mpssoft-linux-gnu-gcc
        export CFLAGS+=-m64 -Wno-deprecated-declarations -Wno-unused-function -Wno-unused-parameter -Wno-narrowing
        export CXXFLAGS+=-m64 -Wno-deprecated-declarations -Wno-unused-function -Wno-unused-parameter
        ;;
    linux-x86) 
        export PATH=/opt/i686-mpssoft-linux-gnu/bin:$PATH
        export CXX=i686-mpssoft-linux-gnu-g++
        export CC=i686-mpssoft-linux-gnu-gcc
        ;;
    baget)
        export PATH=/opt/mips-mpssoft-linux-gnu/bin:$PATH
        export CXX=mips-mpssoft-linux-gnu-g++
        export CC=mips-mpssoft-linux-gnu-gcc
        LDFLAGS+=-latomic
        ;;
        
    *)
        echo "Incorrect platform name $1. Use argument help to display supporeted platforms"
        exit 1
esac

export DEPs=(mplc_lib_so/masterplc.so
    mplc_lib_so/mplc_archive.so 
    mplc_lib_so/mplcshare.so 
    mplc_lib_so/opcua.so 
    mplc_lib_so/liblua.so 
    mplc_lib_so/mplc_events.so)

mkdir ./out
for dep in "${DEPs[@]}"; do
    if [ ! -f "$dep" ]; then
        echo "$dep not found in ./mplc_lib_so/."
        echo "Hint: Copy ${DEPs[*]} from /opt/mplc4/ to ./mplc_lib_so/"
        if [ -d "/opt/mplc4" ]; then
            echo "Found mplc4 on this machine. Do you want to copy libs (y/n)?"
            read yn
            if [ "$yn" == "y" ];then
                mkdir ./mplc_lib_so 
                cp /opt/mplc4/masterplc.so ./mplc_lib_so/
                cp /opt/mplc4/mplcshare.so ./mplc_lib_so/
                cp /opt/mplc4/mplc_archive.so ./mplc_lib_so/
                cp /opt/mplc4/mplc_events.so ./mplc_lib_so/
                cp /opt/mplc4/opcua.so ./mplc_lib_so/
                cp /opt/mplc4/liblua.so ./mplc_lib_so/
                break
            else
                exit 0
            fi
        fi
    fi
done

export MPLC_LIB_DIR=./out
export MPLCLIBS=$(cd $MPLC_LIB_DIR && pwd)

function Log(){
    echo $1 | tee -a log.txt
}

echo Start "$(date)" | tee log.txt
Log "PWD=$(pwd)"
Log "CMD=$0"
Log "PATH=$PATH"
Log "CXX=$CXX"
Log "CXX=$CC"
Log "DEPs=$DEPs"
Log "MPLC_LIB_DIR=$MPLC_LIB_DIR"
Log "MPLCLIBS=$MPLCLIBS"

make clear | tee -a log.txt
make NAME="$NAME" | tee -a log.txt

Log "End $(date)"