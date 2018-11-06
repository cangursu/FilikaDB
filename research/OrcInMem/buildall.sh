#!/bin/sh


echo ............................................................................
echo Build All
echo ............................................................................

pushd .
set -e

res=0
pushd ./3rdParty
./buildall.sh
res=$?
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


#pushd ./pgext/
#sudo make install
#make installcheck


popd
