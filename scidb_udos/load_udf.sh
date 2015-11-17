#!/bin/bash

~/scidb/scidbtrunk/run.py stop
cp ~/scidb/scidb-image/scidb_udos/libexample_udos.so ~/scidb/scidbtrunk/stage/install/lib/scidb/plugins/
~/scidb/scidbtrunk/run.py start
iquery -aq "load_library('example_udos')"

