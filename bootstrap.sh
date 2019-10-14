#!/usr/bin/env sh

mkdir .debug
mkdir .release

cd .debug
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug ../

cd ../.release
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ../
