#!/bin/bash
set -euo pipefail

cd "$( dirname "${BASH_SOURCE[0]}" )"
ROOT="$( pwd )"

export DYLD_FALLBACK_LIBRARY_PATH=$ROOT/packages/lib
$ROOT/out/app