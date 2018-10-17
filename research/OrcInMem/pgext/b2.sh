#!/bin/sh

./clean.sh
./build.sh
rs=$?

if [ "$?" == "0" ]; then
    ./check.sh
fi

