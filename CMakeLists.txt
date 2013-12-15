project(whee)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)
find_package(X11 REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(whee_SRCS whee.cpp main.cpp NTreeReader.cpp WidgetContainer.cpp MemoryWidget.cpp NetworkWidget.cpp CPUWidget.cpp
CommandWidget.cpp DiskWidget.cpp TemperatureWidget.cpp ProcessHelper.cpp)
# Note: for this to work, you much include [filename].moc at the end of [filename].cpp
qt4_automoc(${whee_SRCS})
add_executable(whee ${whee_SRCS})
target_link_libraries(whee ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${X11_LIBRARIES})
