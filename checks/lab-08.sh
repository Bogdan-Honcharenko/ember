#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
python3 checks/lab-08-check.py
binary=$(mktemp /tmp/ember-lab08.XXXXXX)
trap 'rm -f "$binary"' EXIT
c++ -std=c++17 -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer -g -Isrc checks/lab-08-test.cpp src/lexer.cpp src/assembler.cpp src/cpu.cpp src/alu.cpp src/memory.cpp src/stack.cpp -o "$binary"
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1 "$binary"
