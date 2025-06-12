#!/bin/bash

set -e

options="-Wall -Wextra -pedantic -std=c99 -g -fsanitize=address -fsanitize=undefined"

gcc "examples/$1.c" optiscope.c -o "$1" $options
./"$1"
rm "$1"
