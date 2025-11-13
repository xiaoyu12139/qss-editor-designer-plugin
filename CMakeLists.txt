cmake_minimum_required(VERSION 3.16)
project(LiveQssDesignerPlugin LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets Designer QUIET)
if(NOT Qt6_FOUND)
  find_package(Qt5 COMPONENTS Widgets Designer REQUIRED)
endif()

set(SRC
  src/LiveQssRegistrar.h
  src/LiveQssRegistrar.cpp)

add_library(LiveQssDesignerPlugin SHARED ${SRC})
if(Qt6_FOUND)
  target_link_libraries(LiveQssDesignerPlugin PRIVATE Qt6::Widgets Qt6::Designer)
else()
  target_link_libraries(LiveQssDesignerPlugin PRIVATE Qt5::Widgets Qt5::Designer)
endif()

set_target_properties(LiveQssDesignerPlugin PROPERTIES
  OUTPUT_NAME LiveQssDesignerPlugin
)
