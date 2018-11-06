#!/bin/sh


echo ............................................................................
echo Build All 3rd Party
echo ............................................................................

pushd .
set -e


export AVX2_CFLAGS=-mavx2 
export SSSE3_CFLAGS=-mssse3 
export SSE41_CFLAGS=-msse4.1 
export SSE42_CFLAGS=-msse4.2 
export AVX_CFLAGS=-mavx 
export CFLAGS+=-fPIC


echo
echo Building  3rdParty/gtest/
echo
pushd ./gtest/
mkdir -p _build && cd _build
cmake ..
popd



find . -type f -name "Makefile" -print0 | while read -d $'\0' file
do
    dir=$(dirname $file)
    echo
    echo Building  $dir
    echo
    make -C $dir -f ./Makefile
    if [ "$?" != "0" ]; then
        echo ERROR : $dir
        exit -1
    fi

done


popd
