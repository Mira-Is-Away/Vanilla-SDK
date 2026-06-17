#!/bin/bash

set -e

mkdir -p bin

cd bin
cmake ..
make -j"$(nproc)"