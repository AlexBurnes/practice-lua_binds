#!/usr/bin/env bash
# Student practice C++ sort keys of unordered map on different containers libraries
# Copyright 2025 AlexBurnes@gmail.com
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

source "$(dirname "$(readlink -f "$0")")"/scripts/functions.sh

conan install . -of .build -pr debug --build missing
cmake -H. -B.build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_CXX_CPPCHECK=/usr/local/bin/cppcheck -DCMAKE_INSTALL_PREFIX=./
cmake --build .build
cmake --install .build
ctest --test-dir .build -T Test -V
ctest --test-dir .build -T Coverage
bin/test_bench --benchmark_counters_tabular=true