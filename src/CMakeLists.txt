
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

add_library (Sqr DataReader.cpp TypeParser.cpp InterfaceWriter.cpp)

target_include_directories(Sqr PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)