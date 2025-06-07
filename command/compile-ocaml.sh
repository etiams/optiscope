#!/bin/bash

set -e

cd benchmarks-ocaml

ocamlopt -c interpreter.ml

for file in *.ml; do
    if [[ -f "$file" && "$(basename "$file")" != "interpreter.ml" ]]; then
        basename_file=$(basename "$file" .ml)
        ocamlopt -I . interpreter.cmx "$file" -o "$basename_file"
    fi
done
