# CMake flags (see the debug section in the documentation for more details)
set(DEBUG_PARSER_CONFLICTS 1)

# C flags (see the debug section in the documentation for more details)
add_compile_definitions(RID_VERSION="v0.1a")

if (DEFINED ENV{RID_DEBUG})
    add_compile_definitions(DEBUG)
endif()

if (DEFINED ENV{RID_DEBUG_ERRORS})
    add_compile_definitions(DEBUG_ERRORS)
endif()

if (DEFINED ENV{RID_DEBUG_GC})
    add_compile_definitions(DEBUG_GC)
endif()

add_compile_definitions(RID_STD_PATH="$ENV{RID_STD_PATH}")

# Parse CMake flags
if (${DEBUG_PARSER_CONFLICTS})
    set(BISON_FLAGS "-Wconflicts-sr -Wconflicts-rr -Wcounterexamples")
endif()

# Bison / Flex
find_package(BISON)
find_package(FLEX)

bison_target(Parser parser/parser.yy ${CMAKE_CURRENT_BINARY_DIR}/parser.cc COMPILE_FLAGS ${BISON_FLAGS})
flex_target(Scanner parser/scanner.ll ${CMAKE_CURRENT_BINARY_DIR}/scanner.cc)
add_flex_bison_dependency(Scanner Parser)

# C++ source
file(GLOB SRC objects/*.cc parser/*.cc runtime/*.cc)

add_executable(riddim ${SRC} ${BISON_Parser_OUTPUTS} ${FLEX_Scanner_OUTPUTS})

include_directories(objects)
include_directories(parser)
include_directories(runtime)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set_property(TARGET riddim PROPERTY CXX_STANDARD 17)
