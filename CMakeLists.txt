cmake_minimum_required(VERSION 3.8)

project(cpp-win)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "D:/Qt514/5.14.0/msvc2017_64")
set(BaseDIR D:/)

add_compile_options(/W0)
add_definitions(-DROOT_PATH="${CMAKE_SOURCE_DIR}")

# Qt
file(COPY ${CMAKE_PREFIX_PATH}/bin/Qt5Core.dll DESTINATION ${CMAKE_BINARY_DIR}/Debug/)
file(COPY ${CMAKE_PREFIX_PATH}/bin/Qt5Cored.dll DESTINATION ${CMAKE_BINARY_DIR}/Debug/)
file(COPY ${CMAKE_PREFIX_PATH}/bin/Qt5Gui.dll DESTINATION ${CMAKE_BINARY_DIR}/Debug/)
file(COPY ${CMAKE_PREFIX_PATH}/bin/Qt5Guid.dll DESTINATION ${CMAKE_BINARY_DIR}/Debug/)
file(COPY ${CMAKE_PREFIX_PATH}/bin/Qt5Widgets.dll DESTINATION ${CMAKE_BINARY_DIR}/Debug/)
file(COPY ${CMAKE_PREFIX_PATH}/bin/Qt5Widgetsd.dll DESTINATION ${CMAKE_BINARY_DIR}/Debug/)
file(COPY ${CMAKE_PREFIX_PATH}/plugins/platforms DESTINATION ${CMAKE_BINARY_DIR}/Debug//plugins/)

find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        Network
        Xml
        REQUIRED)

find_package(Boost 1.86 REQUIRED COMPONENTS system filesystem PATHS "${BaseDIR}/packages/boost_1_86_0/lib/cmake/" )

message(${Boost_INCLUDE_DIRS})

add_subdirectory(${CMAKE_SOURCE_DIR}/ThirdParty/yaml-cpp-0.8.0)

aux_source_directory(${CMAKE_SOURCE_DIR}/src src_files)

message(STATUS ${src_files})

add_executable(${PROJECT_NAME} ${src_files})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src
                                                   ${CMAKE_SOURCE_DIR}/ThirdParty/
                                                   ${CMAKE_SOURCE_DIR}/ThirdParty/yaml-cpp-0.8.0/include/
                                                   ${CMAKE_SOURCE_DIR}/ThirdParty/cxxopts-3.2.0/include/
                                                   ${CMAKE_SOURCE_DIR}/ThirdParty/CLI11-2.4.2/include/
                                                  ${Boost_INCLUDE_DIRS}/boost_1_86_0/include/boost-1_86/
						)

# qt5
target_link_libraries(${PROJECT_NAME} Qt5::Core
                                      Qt5::Gui
                                      Qt5::Widgets
                                      Qt5::Network
                                      Qt5::Xml
                                      ${Boost_LIBRARIES}
)

# yaml-0.8.0
target_link_libraries(${PROJECT_NAME} yaml-cpp::yaml-cpp)