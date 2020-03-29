#!/bin/bash

sketch=Multiloop_skeleton

arduino-cli config init
arduino-cli core update-index
arduino-cli board list
arduino-cli core install arduino:avr
arduino-cli core install arduino:samd

arduino-cli lib install VariableTimedAction
port=$(arduino-cli board list | grep 'arduino:' | head -1 | sed 's/ .*//')
fqbn=$(arduino-cli board list --format=json | sed 's/"//g' | grep 'FQBN' | head -1 | awk '{ print $2 }')

arduino-cli compile --fqbn $fqbn  $sketch
arduino-cli upload -p $port --fqbn $fqbn $sketch

# to run serial console 
# minicom -D $port -b 9600 # or whatever the baud is set to
# to quit > enter, ctrl-a, q, enter

