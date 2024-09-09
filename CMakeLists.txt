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

find_package(OpenCV REQUIRED)

find_package(Qt5 REQUIRED PATHS ${Qt_DIR} COMPONENTS Core Widgets Gui)

add_executable(${PROJECT_NAME} ${src})

target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS}
                                                  ${CMAKE_SOURCE_DIR}/headers
                                                  ${CMAKE_SOURCE_DIR}/src/state_machine
                                                  ${Base_DIR}
                                                  )

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES} Qt5::Gui Qt5::Widgets Qt5::Core )

file(COPY "${OpenCV_DIR}/x64/vc15/bin/opencv_world455d.dll" DESTINATION ${CMAKE_BINARY_DIR}/Debug)
message(STATUS "copy dll file")