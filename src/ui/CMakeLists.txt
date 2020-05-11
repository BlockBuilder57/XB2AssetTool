set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(XB2AT_SOURCES
	main.cpp
	xb2at_ui.cpp
	
	# QT/win32 things
	xb2at.qrc
	xb2at.ui
	xb2at.rc
)

add_custom_target(gittag COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/gittag.cmake WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} SOURCES ${CMAKE_CURRENT_BINARY_DIR}/Version.hpp)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/Version.hpp PROPERTIES GENERATED TRUE)

add_executable(xb2at WIN32 ${XB2AT_SOURCES})
add_dependencies(xb2at gittag)

set_property(TARGET xb2at PROPERTY CXX_STANDARD 17)
set_property(TARGET xb2at PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(xb2at xb2core Qt5::Widgets)

target_include_directories(xb2at PRIVATE ${CMAKE_CURRENT_BINARY_DIR})



