cmake_minimum_required(VERSION 3.0)

project(demo)

aux_source_directory(. DIR_SRCS)

add_subdirectory(tinyXml)
add_subdirectory(jsoncpp)

add_executable(demo ${DIR_SRCS})

target_link_libraries(demo tinyXml)
target_link_libraries(demo jsoncpp)
                        
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)