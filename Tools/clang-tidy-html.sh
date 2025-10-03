#!/bin/bash

cmake -S . -B build/debug -G Ninja
cmake --build build/debug --target clang_tidy_log

echo "Activating virtual environment..."
source ~/venvs/clangtidy/bin/activate

echo "Running Clang-Tidy..."
find ./App -name '*.cpp' | xargs -I{} clang-tidy '{}' -p ./build/debug --quiet --extra-arg=-fno-color-diagnostics > ./build/debug/clang-tidy-clean.log 2>&1

echo "Waiting for log file..."
while [ ! -s ./build/debug/clang-tidy-clean.log ]; do sleep 1; done

echo "Generating HTML report..."
python3 -m clang_html ./build/debug/clang-tidy-clean.log -o ./build/debug/clang-tidy-report.html

echo "Deactivating virtual environment..."
deactivate
