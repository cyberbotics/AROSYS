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

### Webot's controller libraries importation into SmartMDSD
export WEBOTS_LIBRARY=$(realpath --relative-to=$SMART_ROOT_ACE/lib $WEBOTS_HOME/lib/controller)
export DEPLOY_LIBRARIES="
$WEBOTS_LIBRARY/libController.so
$WEBOTS_LIBRARY/libCppController.so
"

# It is also possible to copy the library.so directly in SmartMDSD's lib folder using:
#cp $WEBOTS_HOME/lib/controller/libController.so $SMART_ROOT_ACE/lib/
#cp $WEBOTS_HOME/lib/controller/libCppController.so $SMART_ROOT_ACE/lib/


### Paths verification:
echo " --------------------------------------------------------"
if [ -z "$WEBOTS_HOME" ]
then
  echo " |> Error: \$WEBOTS_HOME is not defined, please define it in ~/.profile."
else
  echo " |> WEBOTS_HOME: $WEBOTS_HOME"
fi

echo " |> Webots' configuration is done and libraries are set."
echo " --------------------------------------------------------"
