

echo .... drop ext ....
psql -c "drop extension orcinmem;"

echo .... stop db ....
pg_ctl -D ~/pgdata -l ~/pgdata/log stop

echo
echo .... build ....
echo

make clean && sudo make uninstall && make && sudo make install

if [ "$?" != "0" ]; then
    echo
    echo .... build error ....
else
    echo
    echo .... start db ....
    echo
    pg_ctl -D ~/pgdata -l ~/pgdata/log start

    echo
    echo .... create ext ....
    echo
    psql -c "create extension orcinmem;"
    rs=$?
fi

echo .... finish ....


if [ "$rs" = "0" ]; then
    echo .... unit test ....
    make installcheck
fi



if [ "$rs" = "0" ]; then
    echo .... test ....

#    psql -c "drop extension orcinmem cascade;"
#    psql -c "create extension orcinmem version '0.0.3';"
#    psql -c "select orc_inmem_test('')";
fi

