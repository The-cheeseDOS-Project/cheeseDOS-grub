#!/bin/bash

SU="sudo"
SHELL="/bin/bash"
TARGET="./src/build/build.sh"
FLAGS="-xe"

if [ ! -x "$TARGET" ]; then
  "$SU" chmod +x "$TARGET" || {
    exit 1
  }
fi

"$SHELL" "$FLAGS" "$TARGET" "$@"

