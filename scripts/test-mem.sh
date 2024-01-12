#! /bin/bash

set -e

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

pushd "$DIR/.."
if  [[ ! -e test-bin  ]]; then
  cmake -B test-bin/ -G Ninja -D enable_testing=on
fi
pushd test-bin/
ninja build_test
ctest --output-on-failure -T memcheck
popd
popd

