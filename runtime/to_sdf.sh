#!/usr/bin/env sh

convert $1 -filter Jinc -resize 400% -threshold 30% \( +clone -negate -morphology Distance Euclidean -level 50%,-50% \) -morphology Distance Euclidean -compose Plus -composite -level 45%,55% -resize 25% $2
