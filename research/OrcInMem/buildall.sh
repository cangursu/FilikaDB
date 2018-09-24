#!/bin/sh


echo ............................................................................
echo Build All
echo ............................................................................

pushd .
set -e

pushd 3rdparty/gtest/
mkdir -p _build && cd _build
cmake ..
make 

popd


find . -type f -name "Makefile" -print0 | while read -d $'\0' file
do
    dir=$(dirname $file)
    echo
    echo Building  $dir
    echo
    make -C $dir -f ./Makefile all
done


#pushd ./pgext/
#sudo make install
#make installcheck
#popd

popd
