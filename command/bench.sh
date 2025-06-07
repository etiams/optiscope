#!/bin/bash

set -e

optiscope_options="-DNDEBUG -DOPTISCOPE_MAX_COMMUTATIONS=1000000"
compiler_options="-Wall -Wextra -std=gnu99 -O3 -march=native -Wno-unused-function"
all_options="$optiscope_options $compiler_options"

if [ -z $CC ]; then
    CC=gcc
fi

if [ "$GITHUB_ACTIONS" = "true" ]; then
    warmup=0
    runs=1
else
    warmup=1
    runs=5
fi

for filename in benchmarks/*.c; do
    base_filename="$(basename $filename .c)"
    $CC "$filename" optiscope.c -o "$base_filename" $all_options
    hyperfine --warmup=$warmup --runs=$runs ./$base_filename
    rm "$base_filename"
done
