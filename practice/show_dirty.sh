#!/bin/bash

while true
do
    cat /proc/meminfo | grep -i dirty
    sleep 1
done