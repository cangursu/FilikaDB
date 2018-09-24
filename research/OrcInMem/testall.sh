#!/bin/sh

echo ............................................................................
echo Unit Test All
echo ............................................................................

find . -type f -name "gtestapp" -print0 | while read -d $'\0' file
do
    dir=$(dirname $file)
    echo
    echo Testing  $dir
    echo
    ./$file
    
    echo ************************************************ RESULT = $?
done
