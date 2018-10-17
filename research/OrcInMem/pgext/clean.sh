#!/bin/sh


until pg_isready -q
do
    echo .... start db ....
    pg_ctl -D ~/pgdata -l ~/pgdata/log start
    sleep 2
done


echo .... drop ext. ....
psql -c "drop extension orcinmem;"


echo .... stop db ....
pg_ctl -D ~/pgdata -l ~/pgdata/log stop


make clean && sudo make uninstall

if [ "$?" != "0" ]; then
    echo
    echo .... build error ....
else
    echo
    echo .... start db ....
    echo
    pg_ctl -D ~/pgdata -l ~/pgdata/log start
    rs=$?
fi

exit $rs