cmake_minimum_required(VERSION 3.12)

project(cpp-win)

# for Qt compiler
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

aux_source_directory(${CMAKE_SOURCE_DIR}/src/ src)

set(OpenCV_DIR "D:/packages/opencv4.55/build")
find_package(OpenCV REQUIRED)

set(Qt_DIR "D:/Qt514/5.14.0/msvc2017_64/lib/cmake/")
find_package(Qt5 REQUIRED PATHS ${Qt_DIR} COMPONENTS Core)

message(STATUS ${OpenCV_LIBRARIES})

add_executable(${PROJECT_NAME} ${src})

target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES} Qt5::Core)

file(COPY "D:/packages/opencv4.55/build/x64/vc15/bin/opencv_world455d.dll" DESTINATION ${CMAKE_BINARY_DIR}/Debug)
message(STATUS "copy dll file")