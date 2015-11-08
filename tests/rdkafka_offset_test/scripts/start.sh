#/bin/sh
dir_name=`pwd`
export LD_LIBRARY_PATH=/usr/local/lib
./bin/rdkafka_test -flagfile ./conf/rdkafka.conf

