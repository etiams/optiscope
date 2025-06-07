#!/bin/bash

set -e

cd benchmarks-haskell

for file in *.hs; do
    [[ "$(basename "$file")" == "interpreter.hs" ]] && continue
    basename_file=$(basename "$file" .hs)
    ghc -i interpreter.hs "$file" -o "$basename_file" -O0
done
