#!/bin/bash

if [ "$EUID" -ne 0 ];
then
  echo "launch.sh: no root permissions!"
else
  echo "launch.sh: running server process with lower niceness..."
  nice -n -5 ./mainserver 1> std.out # TODO Test different niceness values!
fi
echo "launch.sh: terminating server process..."

exit
