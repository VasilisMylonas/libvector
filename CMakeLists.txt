cmake_minimum_required(VERSION 3.0.0)
project(libvector VERSION 0.1.0)

add_library(vector vector.c)
target_include_directories(vector PUBLIC .)

add_executable(vector_test vector_test.c)
target_link_libraries(vector_test vector)
add_test(NAME vector_test COMMAND vector_test)

set(DOC_DIR ${CMAKE_BINARY_DIR}/doc)

add_custom_target(documentation ALL
    doxygen ${CMAKE_SOURCE_DIR}/Doxyfile &&
    mkdir -p ${DOC_DIR}/md && doxybook2 -i ${DOC_DIR}/xml -o ${DOC_DIR}/md &&
    mv ${DOC_DIR}/md/Files/*.md ${DOC_DIR} && rm -r ${DOC_DIR}/md/* &&
    mv ${DOC_DIR}/*.md ${DOC_DIR}/md/)
