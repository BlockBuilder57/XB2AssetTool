set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(XB2AT_UI_SOURCES
	main.cpp

	MainWindow.h
	MainWindow.cpp
	
	ShowValueSlider.h
	ShowValueSlider.cpp

	# QT/win32 things
	xb2at.qrc
	xb2at.ui
	xb2at.rc
)

add_executable(xb2at WIN32 ${XB2AT_UI_SOURCES})

add_dependencies(xb2at __xb2at_gittag)
target_include_directories(xb2at PRIVATE ${PROJECT_BINARY_DIR})

set_property(TARGET xb2at PROPERTY CXX_STANDARD 17)
set_property(TARGET xb2at PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(xb2at xb2core Qt5::Widgets)

# PCH mainwindow.h for nyoom speed max
target_precompile_headers(xb2at PRIVATE MainWindow.h)
