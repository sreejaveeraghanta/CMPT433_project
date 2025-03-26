#!/bin/bash

REMOTEPROC=$1

sudo echo stop > /sys/class/remoteproc/remoteproc"$REMOTEPROC"/state
echo Sent shutdown to remoteproc"$REMOTEPROC"
sudo cat /sys/class/remoteproc/remoteproc"$REMOTEPROC"/state
