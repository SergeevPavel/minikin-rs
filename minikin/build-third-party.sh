#!/bin/bash
set -euo pipefail

cd "$( dirname "${BASH_SOURCE[0]}" )"
ROOT="$( pwd )"

PACKAGES_ROOT="${1}/packages"
mkdir -p $PACKAGES_ROOT

CARGO_TARGET=$TARGET
export TARGET="" # workaround because of this var is misleading for make install
echo "Building icu..."
cd $ROOT/third-party/icu/icu4c/source
./configure --prefix=$PACKAGES_ROOT \
            --enable-static=yes \
            --enable-shared=no
make install  -j $NUM_JOBS
export TARGET=CARGO_TARGET

echo "Building harfbuzz..."
cd $ROOT/third-party/harfbuzz
export PKG_CONFIG_PATH="$PACKAGES_ROOT/lib/pkgconfig/"
./autogen.sh
./configure --prefix=$PACKAGES_ROOT \
            --with-icu=yes \
            --enable-static=yes \
            --enable-shared=no \
            --without-freetype \
            --without-glib
make install -j $NUM_JOBS
