#!/bin/bash -e
#---------------------------------------------------------------
#   Script: launch.sh
#  Mission: alpha_heron
#   Author: Mike Benjamin
#   LastEd: 2021-Jun-08
#---------------------------------------------------------------
#  Part 1: Set global var defaults
#---------------------------------------------------------------
ME=`basename "$0"`
TIME_WARP=1
JUST_MAKE=""
VERBOSE=""
NUM_VEHICLES=1
RANDOM_HEADING=0
VEHICLE_NAMES=("abe" "ben" "cal" "deb" "eve" "fin" "max" "ned" "oak" "pip")
VEHICLE_START_POS=("4,-4,90" "20,-1,100" "14,-6,240" "27,2,210" "17,0,180" "4,-8,90" "20,-5,100" "14,-10,240" "27,-2,210" "17,-4,180")
BATCH_NAME=""
SLAUNCH_ARGS=""

#---------------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#---------------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
        echo "$ME [SWITCHES] [time_warp]                                 "
        echo "  --help, -h         Show this help message                "
        echo "  --just_make, -j    Just make targ files, no launch       "
        echo "  --verbose, -v      Verbose output, confirm before launch "
        echo "  --vnum=<number>    Number of vehicles to launch          "
        echo "  --rand, -r         Randomize vehicle starting positions   "
        exit 0
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then
        TIME_WARP=$ARGI
    elif [[ "${ARGI}" =~ --vnum=([0-9]+) ]]; then
        NUM_VEHICLES=${BASH_REMATCH[1]}
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
        JUST_MAKE="-j"
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
        VERBOSE="--verbose"
    elif [ "${ARGI}" = "--nogui" -o "${ARGI}" = "-n" ]; then
        SLAUNCH_ARGS+=" $ARGI"
    elif [ "${ARGI}" = "--rand" -o "${ARGI}" = "-r" ]; then
        RANDOM_HEADING=1
    elif [ "${ARGI:0:8}" = "--batch=" ]; then
        BATCH_NAME="${ARGI#--batch=*}"
    else
        echo "$ME: Bad arg:" $ARGI "Exit Code 1."
        exit 1
    fi
done

# Ensure the number of vehicles does not exceed the length of the VEHICLE_NAMES array
if [ "$NUM_VEHICLES" -gt "${#VEHICLE_NAMES[@]}" ]; then
    echo "Error: Number of vehicles exceeds the number of available vehicle names."
    exit 1
fi

if [ "$RANDOM_HEADING" -eq 1 ]; then
    x_min=0
    x_max=28
    y_min=-30
    y_max=-20

    min_distance=4

    VEHICLE_START_POS=()

    generate_random_number() {
        local min=$1
        local max=$2
        echo $((RANDOM % (max - min + 1) + min))
    }
    generate_random_heading() {
        echo $((RANDOM % 361))
    }
    calculate_distance() {
        local x1=$1
        local y1=$2
        local x2=$3
        local y2=$4
        echo "sqrt(($x2 - $x1) * ($x2 - $x1) + ($y2 - $y1) * ($y2 - $y1))" | bc -l
    }
    # TODO: Look into pickpos

    # Loop to generate random coordinates and headings
    for ((i = 0; i < NUM_VEHICLES; i++)); do
        while true; do
            # Generate random x and y coordinates
            x=$(generate_random_number $x_min $x_max)
            y=$(generate_random_number $y_min $y_max)
            # Check if the new position is at least 1 meter away from all previous positions
            valid=true
            for pos in "${VEHICLE_START_POS[@]}"; do
                IFS=',' read -r px py _ <<< "$pos"
                distance=$(calculate_distance $x $y $px $py)
                if (( $(echo "$distance < $min_distance" | bc -l) )); then
                    valid=false
                    break
                fi
            done
            if $valid; then
                # Generate a random heading between 0 and 360
                heading=$(generate_random_heading)
                # Combine the coordinates with the heading
                start_pos="${x},${y},${heading}"
                # Add the start position to the array
                VEHICLE_START_POS+=("${start_pos}")
                break
            fi
        done
    done
fi

#---------------------------------------------------------------
#  Part 3: Initialize and Launch the vehicles
#---------------------------------------------------------------

# Unique mission name
MISSION_NAME=$(date +"%y%m%d-%H%M%S")
echo "$ME: Mission Name is $MISSION_NAME"
echo "$ME: Batch Name is $BATCH_NAME"
mkdir -p ./logs_simulation/$BATCH_NAME/$MISSION_NAME

for ((i=1; i<=NUM_VEHICLES; i++))
do
    VNAME=${VEHICLE_NAMES[$((i-1))]}
    VPOS=${VEHICLE_START_POS[$((i-1))]}
    VLAUNCH_ARGS=" --auto --noconfirm --sim --vname=$VNAME --index=$i --start=$VPOS --mission_name=$MISSION_NAME --batch=$BATCH_NAME"
    VLAUNCH_ARGS+=" --use_compass=false"
    echo "$ME: Launching $VNAME ..."
    ./launch_vehicle.sh $VLAUNCH_ARGS $VERBOSE $JUST_MAKE $TIME_WARP
done

#---------------------------------------------------------------
#  Part 4: Launch the shoreside
#---------------------------------------------------------------
echo "$ME: Launching Shoreside ..."
./launch_shoreside.sh --auto --noconfirm $VERBOSE $JUST_MAKE $TIME_WARP --mission_name=$MISSION_NAME --batch=$BATCH_NAME --sim $SLAUNCH_ARGS

#---------------------------------------------------------------
# Part 5: Launch uMAC until the mission is quit
#---------------------------------------------------------------
uMAC targ_shoreside.moos
kill -- -$$