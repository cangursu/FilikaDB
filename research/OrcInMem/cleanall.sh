#!/bin/sh

echo ............................................................................
echo Clean All 
echo ............................................................................


pushd .

find . -type d -name "_build" -print0 | while read -d $'\0' file
do
    echo file: file
    [ -d $file ] && rm -r -d $file
done

find . -type f -name "core.*" -print0 | while read -d $'\0' file
do
    [ -f $file ] && rm -d $file
done

find . -type d -name "nbproject" -print0 | while read -d $'\0' file
do
    echo file: $file
    [ -d $file ] && rm -d -r $file
done

find . -name "Makefile" -print0 | while read -d $'\0' file
do
    dir=$(dirname $file)
    make -C $dir -f ./Makefile clean
done


popd
