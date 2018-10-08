#!/bin/sh


error_exit()
{
    echo ERROR EXIT....
    exit -1
}


echo ............................................................................
echo Clean All 
echo ............................................................................

res=0

if [ "$res" == "0" ]; then
    ./cleanall.sh
    res=$?
fi

if [ "$res" == "0" ]; then
    ./buildall.sh
    res=$?
fi

if [ "$res" == "0" ]; then
    ./testall.sh
    res=$?
fi



if [ "$res" == "0" ]; then
    echo $0 quited successfully
else
    echo ERROR quit
fi


