project(whee)
cmake_minimum_required(VERSION 3.1.0)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5 COMPONENTS Widgets Core X11Extras REQUIRED)
find_package(X11 REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(whee_SRCS whee.cpp main.cpp NTreeReader.cpp WidgetContainer.cpp MemoryWidget.cpp NetworkWidget.cpp CPUWidget.cpp
CommandWidget.cpp DiskWidget.cpp TemperatureWidget.cpp ProcessHelper.cpp)
add_executable(whee ${whee_SRCS})
target_link_libraries(whee Qt5::Widgets Qt5::Core Qt5::X11Extras ${X11_LIBRARIES})
