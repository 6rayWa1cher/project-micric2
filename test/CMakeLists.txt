cmake_minimum_required(VERSION 3.9.2)
project(project-micric2)

add_executable(AllTestsPM2 modulartests.cpp ${Micric2_SRC_FILES} integrationtests/translator_expression.cpp integrationtests/translator_program.cpp tools.cpp tools.h codegenmodulartests.cpp)
target_link_libraries(AllTestsPM2 gtest_main)
target_link_libraries(AllTestsPM2 micric-lib)
add_test(NAME AllTestsPM2 COMMAND AllTestsPM2)