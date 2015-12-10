#!/bin/bash
ARRAY="image_volume"
FLAT_ARRAY="image_volume_flat"
INPUT_DIR=$1
SCHEMA=$(./gen_img_schema $INPUT_DIR 2> /dev/null)
FLAT_SCHEMA="<f0:double>[i0=0:*,2000000,0]"

TMPDIR=$(mktemp -d /tmp/ldtr.XXXXXXXXXX)

trap "rm -rf $TMPDIR" 15 3 2 1 0

mkfifo $TMPDIR/fifo

iquery -aq "remove($ARRAY)" 2>/dev/null
iquery -aq "remove($FLAT_ARRAY)" 2>/dev/null
iquery -aq "create array $FLAT_ARRAY $FLAT_SCHEMA"
iquery -aq "create array $ARRAY $SCHEMA"

./imgs2csv $INPUT_DIR > $TMPDIR/fifo &
iquery -naq "load($FLAT_ARRAY, '"$TMPDIR/fifo"', -2, 'csv')"
wait
