include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_package(BISON)
find_package(FLEX)

bison_target(Parser parser.yy ${CMAKE_CURRENT_BINARY_DIR}/parser.cc)
flex_target(Scanner scanner.ll ${CMAKE_CURRENT_BINARY_DIR}/scanner.cc)

add_flex_bison_dependency(Scanner Parser)

file(GLOB SRC *.cc)

add_executable(riddim ${SRC} ${BISON_Parser_OUTPUTS} ${FLEX_Scanner_OUTPUTS})
set_property(TARGET riddim PROPERTY CXX_STANDARD 17)
