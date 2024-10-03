#!/bin/bash -e
#--------------------------------------------------------------
#   Script: launch_shoreside.sh                                    
#  Mission: alpha_heron
#   Author: Michael Benjamin  
#   LastEd: June 2021     
#--------------------------------------------------------------  
#  Part 1: Set global variables
#--------------------------------------------------------------
ME=`basename "$0"`
TIME_WARP=1
JUST_MAKE="no"
VERBOSE=""
CONFIRM="yes"
AUTO_LAUNCHED="no"
LAUNCH_GUI="yes"
CMD_ARGS=""

IP_ADDR="localhost"
MOOS_PORT="9000"
PSHARE_PORT="9200"

REGION="pavlab"

FORCE_IP=""

FORMATION_MARGIN="14"
FORMATION_HEADING="150"
FORMATION_DISTANCE="35"
TURN_RADIUS="4" #NB: also change for vehicles until we have a better way

MISSION_NAME=""
BATCH_NAME=""
SIM="FALSE"


#--------------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#--------------------------------------------------------------
for ARGI; do
    CMD_ARGS+="${ARGI} "
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME: [OPTIONS] [time_warp]                       " 
	echo "                                                 "
	echo "Options:                                         "
    echo "  --help, -h                                     "
    echo "    Display this help message                    "
	echo "  --just_make, -j                                " 
	echo "    Just make target files. Do not launch.       "
    echo "  --verbose, -v                                  "
    echo "    Increase verbosity                           "
	echo "  --noconfirm, -nc                               " 
	echo "    No confirmation before launching             "
    echo "  --auto, -a                                     "
    echo "     Auto-launched by a script.                  "
    echo "     Will not launch uMAC as the final step.     "
    echo "  --nogui, -n                                    "
    echo "     Headless mode - no pMarineViewer etc        "
	echo "                                                 "
	echo "  --ip=<localhost>                               " 
	echo "    Force pHostInfo to use this IP Address       "
	echo "  --mport=<9000>                                 "
	echo "    Port number of this vehicle's MOOSDB port    "
	echo "  --pshare=<9200>                                " 
	echo "    Port number of this vehicle's pShare port    "
	echo "                                                 "
	echo "  --forest, -f      Set region to be Forest Lake " 
    echo "                                                 "
    echo "-------------------------------------------------"
    echo "  --fm=<value>     Formation Margin              "
    echo "  --fh=<value>     Formation Heading             "
    echo "  --fd=<value>     Formation Distance            "
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE="yes"
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	VERBOSE="yes"
    elif [ "${ARGI}" = "--noconfirm" -o "${ARGI}" = "-nc" ]; then
	CONFIRM="no"
    elif [ "${ARGI}" = "--auto" -o "${ARGI}" = "-a" ]; then
        AUTO_LAUNCHED="yes"
    elif [ "${ARGI}" = "--nogui" -o "${ARGI}" = "-n" ]; then
        LAUNCH_GUI="no"
    elif [ "${ARGI:0:5}" = "--ip=" ]; then
        IP_ADDR="${ARGI#--ip=*}"
	FORCE_IP="yes"
    elif [ "${ARGI:0:7}" = "--mport" ]; then
	MOOS_PORT="${ARGI#--mport=*}"
    elif [ "${ARGI:0:9}" = "--pshare=" ]; then
        PSHARE_PORT="${ARGI#--pshare=*}"

    elif [ "${ARGI}" = "--forest" -o "${ARGI}" = "-f" ]; then
        REGION="forest_lake"
    elif [ "${ARGI:0:5}" = "--fm=" ]; then
        FORMATION_MARGIN="${ARGI#--fm=*}"
    elif [ "${ARGI:0:5}" = "--fh=" ]; then
        FORMATION_HEADING="${ARGI#--fh=*}"
    elif [ "${ARGI:0:5}" = "--fd=" ]; then
        FORMATION_DISTANCE="${ARGI#--fd=*}"
    elif [ "${ARGI:0:5}" = "--tr=" ]; then
        TURN_RADIUS="${ARGI#--tr=*}"
    elif [ "${ARGI:0:15}" = "--mission_name=" ]; then
        MISSION_NAME="${ARGI#--mission_name=*}"
    elif [ "${ARGI:0:8}" = "--batch=" ]; then
        BATCH_NAME="${ARGI#--batch=*}"

    elif [ "${ARGI}" = "--sim" -o "${ARGI}" = "-s" ]; then
        SIM="TRUE"

    else 
	echo "$ME: Bad Arg: $ARGI. Exit Code 1."
	exit 1
    fi
done

echo "SIM = " $SIM
echo "MISSION_NAME = " $MISSION_NAME

#---------------------------------------------------------------
#  Part 3: If verbose, show vars and confirm before launching
#---------------------------------------------------------------
if [ "${VERBOSE}" = "yes" -o "${CONFIRM}" = "yes" ]; then 
    echo "$ME"
    echo "CMD_ARGS =      [${CMD_ARGS}]      "
    echo "TIME_WARP =     [${TIME_WARP}]     "
    echo "AUTO_LAUNCHED = [${AUTO_LAUNCHED}] "
    echo "IP_ADDR =       [${IP_ADDR}]       "
    echo "MOOS_PORT =     [${MOOS_PORT}]     "
    echo "PSHARE_PORT =   [${PSHARE_PORT}]   "
    echo "---------------------------------- "
    echo "REGION =        [${REGION}]        "
    echo "---------------------------------- "
    echo "FORMATION_MARGIN =  [${FORMATION_MARGIN}]  "
    echo "FORMATION_HEADING = [${FORMATION_HEADING}] "
    echo "FORMATION_DISTANCE = [${FORMATION_DISTANCE}]"
    echo "TURN_RADIUS =       [${TURN_RADIUS}]      "
    echo "---------------------------------- "
    echo "SIMULATION = [${SIM}]"
    echo -n "Hit any key to continue with launching"
    read ANSWER
fi


#--------------------------------------------------------------
#  Part 4: Create the .moos and .bhv files using nsplug
#--------------------------------------------------------------
nsplug meta_shoreside.moos targ_shoreside.moos -i -f WARP=$TIME_WARP  \
       IP_ADDR=$IP_ADDR       PSHARE_PORT=$PSHARE_PORT                \
       MOOS_PORT=$MOOS_PORT   REGION=$REGION  FORCE_IP=$FORCE_IP      \
       FORMATION_MARGIN=$FORMATION_MARGIN                             \
       FORMATION_HEADING=$FORMATION_HEADING                           \
       FORMATION_DISTANCE=$FORMATION_DISTANCE                         \
       TURN_RADIUS=$TURN_RADIUS                                       \
       SIM=$SIM                                                       \
       MISSION_NAME=$MISSION_NAME                                     \
       BATCH_NAME=$BATCH_NAME                                         \
       LAUNCH_GUI=$LAUNCH_GUI

if [ ${JUST_MAKE} = "yes" ]; then
    echo "Files assembled; nothing launched; exiting per request."
    exit 0
fi

#--------------------------------------------------------------
#  Part 5: Launch the processes
#--------------------------------------------------------------
echo "Launching Shoreside MOOS Community. WARP="$TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &
echo "Done Launching Shoreside Community"

#---------------------------------------------------------------
#  Part 6: If launched from script, we're done, exit now
#---------------------------------------------------------------
if [ "${AUTO_LAUNCHED}" = "yes" ]; then
    exit 0
fi

#---------------------------------------------------------------
# Part 7: Launch uMAC until the mission is quit
#---------------------------------------------------------------
uMAC targ_shoreside.moos
kill -- -$$

