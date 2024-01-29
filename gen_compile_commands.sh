#!/bin/sh

# xmake f --toolchain=clang
# xmake project -k compile_commands

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -Bbuild .
cp build/compile_commands.json ./

