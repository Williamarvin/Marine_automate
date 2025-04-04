#!/bin/bash
#--------------------------------------------------
#   Script: GenlaunchShoresideDiff.sh
#   Author: M.Benjamin
#   LastEd: May 2024
#--------------------------------------------------
for ARGI; do
    CMD_ARGS+="${ARGI} "
done

FILE="launch_shoreside.sh"

GenLaunchShoreside.sh $CMD_ARGS --output=tmp_$FILE
diff $FILE tmp_$FILE
rm -f tmp_$FILE

exit 0
