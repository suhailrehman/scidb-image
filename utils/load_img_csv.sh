#!/bin/bash
ARRAY="image_volume"
INPUT_DIR=$1
SCHEMA=$(./gen_img_schema $INPUT_DIR 2> /dev/null)
TMPDIR=$(mktemp -d /tmp/ldtr.XXXXXXXXXX)

trap "rm -rf $TMPDIR" 15 3 2 1 0

mkfifo $TMPDIR/fifo

iquery -aq "remove($ARRAY)" 2>/dev/null
iquery -aq "create array $ARRAY $SCHEMA"

img2csv $INPUT_DIR > $TMPDIR/fifo &
iquery -naq "load($ARRAY, '"$TMPDIR/fifo"', -2, 'csv')"
wait
