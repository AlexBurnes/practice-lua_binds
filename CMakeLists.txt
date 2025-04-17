cmake_minimum_required(VERSION 3.20)
project(lua_bind CXX)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

include(CTest)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(out_of_source_dir)
include(pedantic)

find_package(cxxopts REQUIRED)
find_package(fmt REQUIRED)
find_package(tscns REQUIRED)
find_package(GTest)
find_package(gperftools)
find_package(benchmark)
add_compile_definitions(LUA_VERSION_NUM=504)
find_package(sol2 CONFIG REQUIRED)
include(sparsepp.cmake)

enable_testing()

add_subdirectory(src)


