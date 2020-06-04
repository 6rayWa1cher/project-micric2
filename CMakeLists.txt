cmake_minimum_required(VERSION 3.16)
project(project_micric2)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

configure_file(CMakeLists.txt.in pmicric-download/CMakeLists.txt)

execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/pmicric-download)
if (result)
    message(FATAL_ERROR "CMake step for pmicric failed: ${result}")
endif ()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/pmicric-download)
if (result)
    message(FATAL_ERROR "Build step for pmicric failed: ${result}")
endif ()

add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/pmicric-src
        ${CMAKE_CURRENT_BINARY_DIR}/pmicric-build
        EXCLUDE_FROM_ALL)

file(GLOB Micric2_SRC_FILES
        ${PROJECT_SOURCE_DIR}/src/core/*.cpp
        ${PROJECT_SOURCE_DIR}/src/include/*.h
        ${PROJECT_SOURCE_DIR}/src/lib/*.cpp
        ${PROJECT_SOURCE_DIR}/src/lib/*.h)

enable_testing()
add_subdirectory(src)
add_subdirectory(test)

#target_link_libraries(app PRIVATE micric-lib)