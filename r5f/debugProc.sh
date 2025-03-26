#!/bin/bash

REMOTEPROC=$1

sudo cat /sys/class/remoteproc/remoteproc"$REMOTEPROC"/state
sudo cat /sys/kernel/debug/remoteproc/remoteproc"$REMOTEPROC"/trace0
