#!/bin/bash

# copy the data folder
mkdir -p data
cp -r ../data/* data/

# print line count
echo -n "Lines of code: "
find ../src/ ../include/ -type f -not -name ".*" -not -name "rapid*" -not -name "moc_*" -not -name "stb_*" -not -name "pstdint.h" | xargs egrep -v "^\s*$" | wc -l | tail -1
