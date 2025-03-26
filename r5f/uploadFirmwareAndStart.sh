#!/bin/bash

REMOTEPROC=$1
FIRMWARE_FILE=$2

# Check if the remote processor state is "offline"
if [[ "$(sudo cat /sys/class/remoteproc/remoteproc$REMOTEPROC/state)" == "offline" ]]; then
    # Copy firmware file
    sudo cp "$FIRMWARE_FILE" /lib/firmware/
    
    # Set firmware file
    sudo echo "$FIRMWARE_FILE" > /sys/class/remoteproc/remoteproc"$REMOTEPROC"/firmware
    
    # Start remote processor
    sudo echo start > /sys/class/remoteproc/remoteproc"$REMOTEPROC"/state
    
    echo "Firmware loaded and remote processor started."
else
    echo "Remote processor is already running."
fi
