#!/usr/bin/env bash

cmake --build /home/timur/projects/badbaby/.debug
if [ $? -eq 0 ]; then
  /home/timur/projects/badbaby/.debug/painter/painter $1
else
  echo "Build Failed"
fi
