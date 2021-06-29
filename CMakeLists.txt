cmake_minimum_required (VERSION 3.15)

project (CHP LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS				ON)
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE			ON)
set(CMAKE_COLOR_MAKEFILE                        ON)
set(CMAKE_VERBOSE_MAKEFILE                      OFF)


include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/windows
)

add_subdirectory (src)
aux_source_directory(windows   SRCS)

add_executable(${PROJECT_NAME} ${SRCS} main.cpp)

target_link_libraries (${PROJECT_NAME} Sqr)

enable_testing()
add_subdirectory( test ./libs/unit_tests/)

    