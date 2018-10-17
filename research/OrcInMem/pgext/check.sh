#!/bin/sh

echo .... unit test ....
make installcheck
rs=$?

if [ "$rs" != "0" ]; then
    echo .... Test Failed ....
fi

exit $rs