cmake_minimum_required(VERSION 3.9.2)
project(project-micric2)

add_executable(micric2 main.cpp ${Micric2_SRC_FILES})
target_include_directories(micric2 PRIVATE include/ lib/)
target_link_libraries(micric2 micric-lib)