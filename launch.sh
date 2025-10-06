#!/bin/bash

if [ "$EUID" -ne 0 ];
then
  echo "launch.sh: no root permissions!"
else
  echo "launch.sh: running server process with lower niceness..."
  nice -n -20 ./mainserver > $1 2>&1 # TODO Test different niceness values!
fi
echo "launch.sh: terminating server process..."

exit
