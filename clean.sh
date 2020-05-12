#!/bin/bash
set -euo pipefail

cd "$( dirname "${BASH_SOURCE[0]}" )"
ROOT="$( pwd )"

cd $ROOT/minikin/third-party/harfbuzz
git clean -f -d -x

cd $ROOT/minikin/third-party/icu
git clean -f -d -x

cd $ROOT
cargo clean
