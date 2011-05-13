#!/bin/bash
for f in `find ../src/ -name "moc_*" -and -name "*.cpp" -and -type f`; do
    rm $f
done

for f in `find ../include/ -name "*.hpp" -and -type f -exec grep -l Q_OBJECT {} \;`; do
    BASENAME=$(basename $f .hpp)
    SUBDIR=$(dirname ${f:11})
    mkdir -p ../src/$SUBDIR
    echo "moccing $f to ../src/$SUBDIR/moc_${BASENAME}.cpp"
    moc $f > ../src/$SUBDIR/moc_${BASENAME}.cpp
done
