

echo .... drop ext ....
psql -c "drop extension ext_orc15;"

echo .... stop db ....
pg_ctl -D ~/pgdata11 -l ~/pgdata11/log stop

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
    pg_ctl -D ~/pgdata11 -l ~/pgdata11/log start

    echo
    echo .... create ext ....
    echo
    psql -c "create extension ext_orc15;"
    rs=$?
fi

echo .... finish ....


if [ "$rs" = "0" ]; then
    echo .... unit test ....
    make installcheck
fi



if [ "$rs" = "0" ]; then
    echo .... test ....

    psql -c "drop extension ext_orc15 cascade;"
    psql -c "create extension ext_orc15 version '0.0.3';"
    psql -c "select orc_inmem_test('')";
fi

