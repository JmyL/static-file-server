add_library(calc calc.cpp)
target_include_directories(
    calc
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
set_target_properties(calc PROPERTIES PUBLIC_HEADER src/calc/calc.h)

include(Coverage)
clean_coverage(calc)
include(Format)
format(calc .)
include(CppCheck)
add_cppcheck(calc)
include(ClangTidy)
add_clangtidy(calc)
include(STLDebugChecks)
add_stl_debug_check(calc)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SANITIZE_ADDRESS ON)
    # set(SANITIZE_MEMORY ON)
    # set(SANITIZE_THREAD ON)
    # set(SANITIZE_UNDEFINED ON)
    find_package(Sanitizers)
    # sanitizer_add_blacklist_file("blacklist.txt")
    add_sanitizer(calc)
endif()
include(Doxygen)
add_doxygen(calc src/calc)