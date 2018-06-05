

echo .... drop ext ....
psql -c "drop extension ext_orc15;"

echo .... stop db ....
pg_ctl -D ~/pgdata -l ~/pgdata/logfile stop

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
    pg_ctl -D ~/pgdata -l ~/pgdata/logfile start

    echo
    echo .... create ext ....
    echo
    psql -c "create extension ext_orc15;"
    rs=$?
fi

echo .... finish ....

if [ "$rs" = "0" ]; then
    echo .... test ....
    psql -c "select ext_orc_mem_test('')";
fi

