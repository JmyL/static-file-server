cmake_minimum_required(VERSION 3.20.0)
cmake_policy(SET CMP0167 NEW)

project(static_file_server VERSION 1.0.0 LANGUAGES CXX)

include(CPM.cmake)
CPMAddPackage(
    NAME cmake_modules
    GITHUB_REPOSITORY JmyL/cmake-modules
    VERSION 0.1.9
)
list(APPEND CMAKE_MODULE_PATH ${cmake_modules_SOURCE_DIR})

include(ExportCompileCommands)
include(NoInSourceBuilds)
include(SetReleaseAsDefault)
include(UseCcache)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src bin)
include(CTest)
add_subdirectory(test)