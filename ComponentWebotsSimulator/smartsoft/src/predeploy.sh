#!/bin/bash

# Use this variable to name files that are to be deployed to the data
# instance directory of
# the component (<COMPONENT INSTANCE NAME>_data/) on the target host.
#
# Files can be relative to component project directory or absolute.
# Add one file or directory per line.
#
# Examples:
#
# DEPLOY_LIBRARIES="../bin/libMyLibrary.so"
#
# DEPLOY_COMPONENT_FILES="
# $SMART_ROOT_ACE/myFILE
# $SMART_ROOT_ACE/myFILE2
# "

#-----------------------------------------------------------------------
#   /|\                                                           /|\
#  / | \  Don't modify except in the part where it is allowed!   / | \
# /__o__\                                                       /__o__\
#-----------------------------------------------------------------------


### Pre-requirement
# Execute this command one time in a Terminal to be able to launch Webots software and restart the computer/Virtual Machine
#echo -e "export WEBOTS_HOME=/home/smartsoft/SOFTWARE/webots" >> ~/.profile


### Configuration (paths, world and startup options)
# It is possible to add some starting options when launching Webots.
# (more info: https://cyberbotics.com/doc/guide/starting-webots)
##### ---------------------------------------------------------------------------
# The user can modify the following parameters for his simulation.
export WEBOTS_ROBOT_NAME="Pioneer 3-DX"
export WORLD_SELECTED=PioneerLMS291/worlds/pioneerLMS291.wbt
export WEBOTS_WORLD_PATH=$SMART_ROOT_ACE/repos/DataRepository/webots/$WORLD_SELECTED
export OPTIONS='--batch --mode=realtime'
# Do not modify below this line.
##### ---------------------------------------------------------------------------


### Webot's controller libraries importation into SmartMDSD
export WEBOTS_LIBRARY=$(realpath --relative-to=$SMART_ROOT_ACE/lib $WEBOTS_HOME/lib)
export DEPLOY_LIBRARIES="
$WEBOTS_LIBRARY/libController.so
$WEBOTS_LIBRARY/libCppController.so
"

# It is also possible to copy the library.so directly in SmartMDSD's lib folder using:
#cp $WEBOTS_HOME/lib/libController.so $SMART_ROOT_ACE/lib/
#cp $WEBOTS_HOME/lib/libCppController.so $SMART_ROOT_ACE/lib/


### Paths verification:
echo " --------------------------------------------------------"
if [ -z "$WEBOTS_HOME" ]
then
  echo " |> Error: \$WEBOTS_HOME is not defined, please define it in ~/.profile."
else
  echo " |> WEBOTS_HOME: $WEBOTS_HOME"
fi

if [ -z "$WORLD_SELECTED" ]
then
  echo " |> Error: \$WORLD_SELECTED is not defined, please define it in \"predeploy.sh\"."
else
  echo " |> WORLD_SELECTED: $WORLD_SELECTED"
fi

if [ -z "$WEBOTS_WORLD_PATH" ]
then
  echo " |> Error: \$WEBOTS_WORLD_PATH is not defined, please define it in \"predeploy.sh\"."
else
  echo " |> WEBOTS_WORLD_PATH: $WEBOTS_WORLD_PATH"
fi

echo " |> Webots' configuration is done and libraries are set."
echo " --------------------------------------------------------"


### Launch Webots with the defined world and startup options
echo " Webots will be launched..."
xterm -title "Webots Simulator" -hold -e bash $WEBOTS_HOME/webots $OPTIONS $WEBOTS_WORLD_PATH &
sleep 5
echo " Webots is running..."
