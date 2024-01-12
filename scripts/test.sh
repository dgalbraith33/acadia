#! /bin/bash

set -e

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

pushd "$DIR/.."
cmake -B test-bin/ -G Ninja -D enable_testing=on
pushd test-bin/
ninja build_test
ctest --output-on-failure -T memcheck
popd
popd

