#!/bin/bash
for f in `find ../src/ -name "moc_*" -and -name "*.cpp" -and -type f`; do
    rm $f
done

for f in `find ../include/ -name "*.hpp" -and -type f -exec grep -l Q_OBJECT {} \;`; do
    BASENAME=`basename $f`
    SUBDIR=`dirname ${f:11}`
    mkdir -p ../src/$SUBDIR
    moc $f > ../src/$SUBDIR/moc_${BASENAME:0:(-4)}.cpp
done
