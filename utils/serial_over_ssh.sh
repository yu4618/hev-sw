#!/bin/bash
ssh -fCo "ExitOnForwardFailure yes" -L 54333:localhost:54321 pi@nuliv "nc -l localhost 54321 </dev/ttyACM0 >/dev/ttyACM0" && socat pty,link=$HOME/vmodem0,wait-slave tcp:localhost:54333 &

echo "Connect to \$HOME/vmodem0 for serial connection"
