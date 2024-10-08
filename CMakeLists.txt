cmake_minimum_required(VERSION 3.8)

project(cpp-win)

aux_source_directory(${CMAKE_SOURCE_DIR}/src src_files)

add_executable(${PROJECT_NAME} ${src_files})