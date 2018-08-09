#!/bin/sh


pushd .

find . -name "Makefile" -print0 | while read -d $'\0' file
do
    dir=$(dirname $file)
    make -C $dir -f ./Makefile clean
done

find -type d  -name nbproject | xargs rm -rv



popd