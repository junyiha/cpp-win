cmake_minimum_required(VERSION 3.12)

project(cpp-win)

# for Qt compiler
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

aux_source_directory(${CMAKE_SOURCE_DIR}/src/ src)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/state_machine src)

include(${CMAKE_SOURCE_DIR}/.vscode/path.cmake)
message(STATUS ${OpenCV_DIR})

# set(OpenCV_DIR "C:/programs/packages/opencv4.55/build")
# set(OpenCV_DIR "D:/packages/opencv4.55/build")
find_package(OpenCV REQUIRED)

set(Qt_DIR "C:/programs/Qt514/5.14.0/msvc2017_64/lib/cmake/")
find_package(Qt5 REQUIRED PATHS ${Qt_DIR} COMPONENTS Core Widgets Gui)

set(Base_DIR "D:/packages")
set(Base_DIR "C:/programs/packages/")

add_executable(${PROJECT_NAME} ${src})

target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS}
                                                  ${CMAKE_SOURCE_DIR}/header
                                                  ${CMAKE_SOURCE_DIR}/src/state_machine
                                                  ${Base_DIR}
                                                  )

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES} Qt5::Gui Qt5::Widgets Qt5::Core )

file(COPY "${OpenCV_DIR}/x64/vc15/bin/opencv_world455d.dll" DESTINATION ${CMAKE_BINARY_DIR}/Debug)
message(STATUS "copy dll file")