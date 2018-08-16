#!/bin/sh


pushd .

set -e

find . -name "Makefile" -print0 | while read -d $'\0' file
do
    dir=$(dirname $file)
    echo
    echo Building  $dir
    echo
    make -C $dir -f ./Makefile  clean all

done

popd
