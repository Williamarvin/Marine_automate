#!/bin/bash -e
#-------------------------------------------------------------- 
#   Script: launch_vehicle.sh                                    
#  Mission: ufld_alpha
#   Author: Michael Benjamin  (Blake Cole)
#     Date: April 2021 (26 June 2021)
#--------------------------------------------------------------
#  Part 1: Declare global var defaults
#--------------------------------------------------------------
ME=`basename "$0"`
TIME_WARP=1
JUST_MAKE="no"
VERBOSE="no"
CONFIRM="yes"
AUTO_LAUNCHED="no"
CMD_ARGS=""

IP_ADDR="localhost"
MOOS_PORT="9001"
PSHARE_PORT="9201"

SHORE_IP="192.168.1.220"
SHORE_PSHARE="9200"

INDEX="1"

REGION="pavlab"
VNAME="jing"
VTYPE="kayak"
START_POS="0,0"
LOITER_POS="x=50,y=-50"


#--------------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#--------------------------------------------------------------
for ARGI; do
    CMD_ARGS+=" ${ARGI}"
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME [OPTIONS] [time_warp]                        "
	echo "                                                 " 
	echo "Options:                                         "
	echo "  --help, -h                                     " 
	echo "    Print this help message and exit             "
	echo "  --just_make, -j                                " 
	echo "    Just make targ files, but do not launch      "
	echo "  --verbose, -v                                  " 
	echo "    Verbose output, confirm before launching     "
	echo "  --noconfirm, -nc                               " 
	echo "    No confirmation before launching             "
        echo "  --auto, -a                                     "
        echo "     Auto-launched by a script.                  "
        echo "     Will not launch uMAC as the final step.     "
	echo "                                                 "
	echo "  --ip=<localhost>                               " 
	echo "    Force pHostInfo to use this IP Address       "
	echo "  --mport=<9001>                                 "
	echo "    Port number of this vehicle's MOOSDB port    "
	echo "  --pshare=<9201>                                " 
	echo "    Port number of this vehicle's pShare port    "
	echo "                                                 "
	echo "  --shore=<localhost>                            " 
	echo "    IP address location of shoreside             "
	echo "  --vname=<abe>                                  " 
	echo "    Name of the vehicle being launched           " 
	echo "  --index=<1>                                    " 
	echo "    Index for setting MOOSDB and pShare ports    "
	echo "                                                 "
	echo "  --start=<X,Y>     (default is 0,0)             " 
	echo "    Start position chosen by script launching    "
	echo "    this script (to ensure separation)           "
	echo "                                                 "
	echo "  --evan,  -E  : Evan vehicle.                   "
	echo "  --felix, -F  : Felix vehicle.                  "
	echo "  --gus,   -G  : Gus vehicle.                    "
	echo "  --hal,   -H  : Hal vehicle.                    "
	echo "  --ida,   -I  : Ida vehicle.                    "
	echo "  --jing,  -J  : Jing vehicle.                   "
	echo "  --kirk,  -K  : Kirk vehicle.                   "
	echo "  --luke,  -L  : Luke vehicle.                   "
	echo "  --m200       : Use iM200 interface             "
	echo "  --m300       : Use iM300 interface             "
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

    elif [ "${ARGI:0:5}" = "--ip=" ]; then
        IP_ADDR="${ARGI#--ip=*}"
    elif [ "${ARGI:0:7}" = "--mport" ]; then
	MOOS_PORT="${ARGI#--mport=*}"
    elif [ "${ARGI:0:9}" = "--pshare=" ]; then
        PSHARE_PORT="${ARGI#--pshare=*}"

    elif [ "${ARGI:0:8}" = "--shore=" ]; then
        SHORE_IP="${ARGI#--shore=*}"
    elif [ "${ARGI:0:8}" = "--vname=" ]; then
        VNAME="${ARGI#--vname=*}"
    elif [ "${ARGI:0:8}" = "--index=" ]; then
        INDEX="${ARGI#--index=*}"

    elif [ "${ARGI:0:8}" = "--start=" ]; then
        START_POS="${ARGI#--start=*}"
	
    elif [ "${ARGI}" = "--evan" -o "${ARGI}" = "-E" ]; then
        VNAME="evan"
    elif [ "${ARGI}" = "--felix" -o "${ARGI}" = "-F" ]; then
        VNAME="felix"
    elif [ "${ARGI}" = "--gus" -o "${ARGI}" = "-G" ]; then
        VNAME="gus"
    elif [ "${ARGI}" = "--hal" -o "${ARGI}" = "-H" ]; then
        VNAME="hal"
    elif [ "${ARGI}" = "--ida" -o "${ARGI}" = "-I" ]; then
        VNAME="ida"
    elif [ "${ARGI}" = "--jing" -o "${ARGI}" = "-J" ]; then
        VNAME="jing"
    elif [ "${ARGI}" = "--kirk" -o "${ARGI}" = "-K" ]; then
        VNAME="kirk"
    elif [ "${ARGI}" = "--luke" -o "${ARGI}" = "-L" ]; then
        VNAME="luke"

    elif [ "${ARGI}" = "--sim" -o "${ARGI}" = "-s" ]; then
        XMODE="SIM"
        echo "Simulation mode ON."

    elif [ "${ARGI}" = "--m300" -o "${ARGI}" = "-3" ]; then
	XMODE="M300"
    elif [ "${ARGI}" = "--m200" -o "${ARGI}" = "-2" ]; then
	XMODE="M200"
    else
	echo "$ME: Bad Arg:[$ARGI]. Exit Code 1."
	exit 1
    fi
done

#--------------------------------------------------------------
#  Part 3: Check for VNAME. Use INDEX for Other Settings
#--------------------------------------------------------------
if [ -z $VNAME ]; then
    if [ "${XMODE}" = "SIM" ]; then
	VNAME="abe"
    else
	echo "No in-water vehicle selected. Exit Code 2."
	exit 2
    fi
fi

if [ "${VNAME}" = "evan" ]; then
    INDEX=5
elif [ "${VNAME}" = "felix" ]; then
    INDEX=6
elif [ "${VNAME}" = "gus" ]; then
    INDEX=7
elif [ "${VNAME}" = "hal" ]; then
    INDEX=8
elif [ "${VNAME}" = "ida" ]; then
    INDEX=9
elif [ "${VNAME}" = "jing" ]; then
    INDEX=10
elif [ "${VNAME}" = "kirk" ]; then
    INDEX=11
elif [ "${VNAME}" = "luke" ]; then
    INDEX=12
fi

MOOS_PORT=`expr $INDEX + 9000`
PSHARE_PORT=`expr $INDEX + 9200`
FSEAT_IP="192.168.$INDEX.1"
IP_ADDR="192.168.$INDEX.100"

if [ "${XMODE}" = "SIM" ]; then
    IP_ADDR="localhost"
    SHORE_IP="localhost"
fi

     
#---------------------------------------------------------------
#  Part 4: If verbose, show vars and confirm before launching
#---------------------------------------------------------------
if [ "${VERBOSE}" = "yes" -o "${CONFIRM}" = "yes" ]; then 
    echo "$ME"
    echo "CMD_ARGS =      [${CMD_ARGS}]     "
    echo "TIME_WARP =     [${TIME_WARP}]    "
    echo "AUTO_LAUNCHED = [${AUTO_LAUNCHED}]"
    echo "----------------------------------"
    echo "MOOS_PORT =     [${MOOS_PORT}]    "
    echo "PSHARE_PORT =   [${PSHARE_PORT}]  "
    echo "IP_ADDR =       [${IP_ADDR}]      "
    echo "----------------------------------"
    echo "SHORE_IP =      [${SHORE_IP}]     "
    echo "SHORE_PSHARE =  [${SHORE_PSHARE}] "
    echo "INDEX =         [${INDEX}]        "
    echo "----------------------------------"
    echo "FSEAT_IP =      [${FSEAT_IP}]     "
    echo "XMODE =         [${XMODE}]        "
    echo "----------------------------------"
    echo "VNAME =         [${VNAME}]        "
    echo "VTYPE =         [${VTYPE}]        "
    echo "START_POS =     [${START_POS}]    "
    echo "LOITER_POS =    [${LOITER_POS}]   "
    echo -n "Hit any key to continue with launching"
    read ANSWER
fi


#--------------------------------------------------------------
#  Part 5: Create the .moos and .bhv files. 
#--------------------------------------------------------------
NSFLAGS="-s -f"
if [ "${AUTO}" = "" ]; then
    NSFLAGS="-i -f"
fi

nsplug meta_vehicle.moos targ_$VNAME.moos $NSFLAGS WARP=$TIME_WARP \
       VNAME=$VNAME                 VTYPE=$VTYPE                   \
       START_POS=$START_POS         LOITER_POS=$LOITER_POS         \
       PSHARE_PORT=$PSHARE_PORT     MOOS_PORT=$MOOS_PORT           \
       IP_ADDR=$IP_ADDR             SHORE_IP=$SHORE_IP             \
       SHORE_PSHARE=$SHORE_PSHARE   REGION=$REGION                 \
       FSEAT_IP=$FSEAT_IP           XMODE=$XMODE               \
       

nsplug meta_vehicle.bhv targ_$VNAME.bhv $NSFLAGS VNAME=$VNAME      \
    START_POS=$START_POS         LOITER_POS=$LOITER_POS         \
    REGION=$REGION

if [ ${JUST_MAKE} = "yes" ] ; then
    echo "Files assembled; nothing launched; exiting per request."
    exit 0
fi

#--------------------------------------------------------------
#  Part 6: Launch the processes
#--------------------------------------------------------------
echo "Launching $VNAME MOOS Community. WARP="$TIME_WARP
pAntler targ_$VNAME.moos >& /dev/null &
echo "Done Launching the $VNAME MOOS Community"

#---------------------------------------------------------------
#  Part 7: If launched from script, we're done, exit now
#---------------------------------------------------------------
if [ "${AUTO_LAUNCHED}" = "yes" ]; then
    exit 0
fi

#---------------------------------------------------------------
# Part 8: Launch uMAC until the mission is quit
#---------------------------------------------------------------
uMAC targ_$VNAME.moos
kill -- -$$
