#!/bin/bash

# If you want to pass arguments to this script, you can use $1, $2, etc.
# Example: ./your_script.sh arg1 arg2

IP_ADDR=172.28.195.205
FSEAT_IP=172.28.195.205
VNAME="Swimmer"


for ARGI; do
    CMD_ARGS+=" ${ARGI}"
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
        echo "help section"
        exit 0

    elif [ "${ARGI}" = "--ip" -o "${ARGI}" = "-ip" ]; then
        echo $IP_ADDR
    
    elif [ "${ARGI}" = "--fseat" -o "${ARGI}" = "-f" ]; then
        echo $FSEAT_IP
    
    elif [ "${ARGI}" = "--name" -o "${ARGI}" = "-n" ]; then
        echo $VNAME

    else
	    echo "$ME: Bad Arg:[$ARGI]. Exit Code 1."
        exit 1
    
    fi 
done