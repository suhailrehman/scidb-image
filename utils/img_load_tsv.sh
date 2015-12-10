#!/bin/bash
ARRAY="image_volume"
FLAT_ARRAY="image_volume_flat"
INPUT_DIR=$1
SCHEMA=$(./gen_img_schema $INPUT_DIR 2> /dev/null)
FLAT_SCHEMA="<i0: int64, i1: int64, i2: int64, i3: int64, f0:double>[i=0:*,2000000,0]"

TMPDIR=$(mktemp -d /tmp/ldtr.XXXXXXXXXX)

trap "rm -rf $TMPDIR" 15 3 2 1 0

mkfifo $TMPDIR/fifo

iquery -aq "remove($ARRAY)" 2>/dev/null
iquery -aq "remove($FLAT_ARRAY)" 2>/dev/null
#iquery -aq "create array $FLAT_ARRAY $FLAT_SCHEMA"
iquery -aq "create array $ARRAY $SCHEMA"

./imgs2csv $INPUT_DIR > $TMPDIR/fifo &
iquery -naq "store(parse(split('"$TMPDIR/fifo"'),'num_attributes=5'),image_volume_flat))"
wait

iquery -naq "store(redimension(image_volume_flat,image_volume),image_volume)"
