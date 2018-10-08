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
    res=$?

    if [ "$res" != "0" ]; then
        echo ERROR : Test $dir  failed ....
        exit $res;
    fi
done

res=$?

if [ "$res" == "0" ]; then
    echo All tests executed successfully
fi

exit $res
