cmake_minimum_required(VERSION 3.8)

project(cpp-win)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "D:/Qt514/5.14.0/msvc2017_64")
set(BaseDIR D:/)

add_compile_options(/W0)
add_compile_options(/MP)
add_definitions(-DROOT_PATH="${CMAKE_SOURCE_DIR}")

# Qt
file(COPY ${CMAKE_PREFIX_PATH}/bin/Qt5Core.dll DESTINATION ${CMAKE_BINARY_DIR}/Debug/)
file(COPY ${CMAKE_PREFIX_PATH}/bin/Qt5Cored.dll DESTINATION ${CMAKE_BINARY_DIR}/Debug/)
file(COPY ${CMAKE_PREFIX_PATH}/bin/Qt5Gui.dll DESTINATION ${CMAKE_BINARY_DIR}/Debug/)
file(COPY ${CMAKE_PREFIX_PATH}/bin/Qt5Guid.dll DESTINATION ${CMAKE_BINARY_DIR}/Debug/)
file(COPY ${CMAKE_PREFIX_PATH}/bin/Qt5Widgets.dll DESTINATION ${CMAKE_BINARY_DIR}/Debug/)
file(COPY ${CMAKE_PREFIX_PATH}/bin/Qt5Widgetsd.dll DESTINATION ${CMAKE_BINARY_DIR}/Debug/)
file(COPY ${CMAKE_PREFIX_PATH}/plugins/platforms DESTINATION ${CMAKE_BINARY_DIR}/Debug//plugins/)

file(COPY D:/packages/OnnxRuntime/Microsoft.ML.OnnxRuntime.1.7.0/runtimes/win-x64/native/onnxruntime.lib DESTINATION ${CMAKE_BINARY_DIR}/Debug/)
file(COPY ${CMAKE_SOURCE_DIR}/ThirdParty/libevent/lib/event_core.lib DESTINATION ${CMAKE_BINARY_DIR}/Debug/)
file(COPY ${CMAKE_SOURCE_DIR}/ThirdParty/libevent/lib/event_extra.lib DESTINATION ${CMAKE_BINARY_DIR}/Debug/)
file(COPY ${CMAKE_SOURCE_DIR}/ThirdParty/libevent/lib/event.lib DESTINATION ${CMAKE_BINARY_DIR}/Debug/)

file(COPY ${CMAKE_SOURCE_DIR}/ThirdParty/libevent/bin/event_core.dll DESTINATION ${CMAKE_BINARY_DIR}/Debug/)
file(COPY ${CMAKE_SOURCE_DIR}/ThirdParty/libevent/bin/event_extra.dll DESTINATION ${CMAKE_BINARY_DIR}/Debug/)
file(COPY ${CMAKE_SOURCE_DIR}/ThirdParty/libevent/bin/event.dll DESTINATION ${CMAKE_BINARY_DIR}/Debug/)

find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        Network
        Xml
        REQUIRED)

# boost
find_package(Boost 1.86 REQUIRED COMPONENTS system filesystem PATHS "${BaseDIR}/packages/boost_1_86_0/lib/cmake/" )
message(${Boost_INCLUDE_DIRS})

# opencv
find_package(OpenCV REQUIRED PATHS "${BaseDIR}/packages/OpenCV4.8.0/opencv/build/x64/vc16/lib/" NO_DEFAULT_PATH)
message(${OpenCV_INCLUDE_DIRS})

# openvino
find_package(OpenVINO REQUIRED PATHS "C:/Intel/openvino_2024.6.0/runtime/cmake/")
if (OpenVINO_FOUND)
    message(STATUS "openvino found")
else()
    message(STATUS "openvino not found")
endif()
message(STATUS "openvino include dir: ${OpenVINO_INCLUDE_DIRS}")

file(COPY ${BaseDIR}/packages/OpenCV4.8.0/opencv/build/x64/vc16/bin/opencv_world480d.dll DESTINATION ${CMAKE_BINARY_DIR}/Debug/)

# yaml
add_subdirectory(${CMAKE_SOURCE_DIR}/ThirdParty/yaml-cpp-0.8.0)

link_directories(${CMAKE_SOURCE_DIR}/ThirdParty/libevent/lib)

aux_source_directory(${CMAKE_SOURCE_DIR}/src src_files)

message(STATUS ${src_files})

add_executable(${PROJECT_NAME} ${src_files})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src
                                                   ${CMAKE_SOURCE_DIR}/ThirdParty/
                                                   ${CMAKE_SOURCE_DIR}/ThirdParty/yaml-cpp-0.8.0/include/
                                                   ${CMAKE_SOURCE_DIR}/ThirdParty/cxxopts-3.2.0/include/
                                                   ${CMAKE_SOURCE_DIR}/ThirdParty/CLI11-2.4.2/include/
                                                   ${CMAKE_SOURCE_DIR}/ThirdParty/asio-1.30.2/include
                                                   ${Boost_INCLUDE_DIRS}/boost_1_86_0/include/boost-1_86/
                                                   ${OpenCV_INCLUDE_DIRS}
                                                   ${OpenVINO_INCLUDE_DIRS}
                                                   ${CMAKE_SOURCE_DIR}/ThirdParty/libevent/include/
						)

# qt5
target_link_libraries(${PROJECT_NAME} Qt5::Core
                                      Qt5::Gui
                                      Qt5::Widgets
                                      Qt5::Network
                                      Qt5::Xml
                                      ${Boost_LIBRARIES}
                                      ${OpenCV_LIBS}
                                      openvino::runtime
                                      event_core
                                      event_extra
                                      event
)

# yaml-0.8.0
target_link_libraries(${PROJECT_NAME} yaml-cpp::yaml-cpp)

target_include_directories(${PROJECT_NAME} PRIVATE D:/packages/OnnxRuntime/Microsoft.ML.OnnxRuntime.1.7.0/build/native/include/)
target_link_libraries(${PROJECT_NAME} D:/packages/OnnxRuntime/Microsoft.ML.OnnxRuntime.1.7.0/runtimes/win-x64/native/onnxruntime.lib)