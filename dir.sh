#!/bin/sh

if which pdir >/dev/null; then
    echo pdir exists.
else
    echo pdir does not exist.
    npm install -g print-dir
fi

pdir . --igd CMake --igf \.out$ -m 2