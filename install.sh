#!/bin/bash

if [ ! $UID -eq 0 ]; then
    echo "You need to be root to run the install routine!"
    exit 1
fi

/usr/bin/env > /dev/null
SUDO_HOME="$(cut -d':' -f1,6 /etc/passwd | egrep ^${SUDO_USER}: | cut -d':' -f2)/"
DST="${SUDO_HOME}dominator/"
FILEPATH="$(readlink -f "${SUDO_COMMAND}")"
SRC="$(dirname "${FILEPATH}")/.install/"

apt-get install libglew1.5 libqt4 lib3ds libboost-thread1.40.0 libboost-filesystem1.40.0
mkdir -p "${DST}"

cp "${SRC}libs/*" "/usr/lib/"
cp "${SRC}dominator" "${DST}"
cp -r "${SRC}data" "${DST}"
cp -r "${SRC}licenses" "${DST}"
cp "${SRC}LICENSE" "${SRC}README" "${DST}"

chown -R ${SUDO_UID}:${SUDO_GID} "${DST}"

echo -e "\n# Dominator binary\nexport PATH=${PATH}:${DST}\n" >> "${SUDO_HOME}.bashrc"
