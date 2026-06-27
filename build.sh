#!/bin/bash

set -e
cmake -S . -B bin
cmake --build bin --config Debug