#!/usr/bin/env bash

cmake --build /home/timur/projects/badbaby/.debug
if [ $? -eq 0 ]; then
  cd /home/timur/projects/badbaby/runtime/sub3000
  /home/timur/projects/badbaby/.debug/sub3000/sub3000
else
  echo "Build Failed"
fi
