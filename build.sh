#!/bin/bash

set -e
cmake --preset default-linux
cmake --build bin --config Debug