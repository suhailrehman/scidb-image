#!/bin/bash
ARRAY="image_volume"
FLAT_ARRAY="image_volume_flat"
INPUT_DIR=$1
SCHEMA=$(./gen_img_schema $INPUT_DIR 2> /dev/null)
FLAT_SCHEMA="image_volume_flat<f0:double>[i0=0:*,2000000,0]"

TMPDIR=$(mktemp -d /tmp/ldtr.XXXXXXXXXX)

trap "rm -rf $TMPDIR" 15 3 2 1 0

mkfifo $TMPDIR/fifo

iquery -aq "remove($ARRAY)" 2>/dev/null
iquery -aq "remove($FLATARRAY)" 2>/dev/null
iquery -aq "create array $FLAT_ARRAY $FLAT_SCHEMA"

img2csv $INPUT_DIR > $TMPDIR/fifo &
iquery -naq "load($ARRAY, '"$TMPDIR/fifo"', -2, 'csv')"
wait
