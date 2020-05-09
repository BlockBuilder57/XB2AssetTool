set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(XB2AT_SOURCES
	main.cpp
	xb2at_ui.cpp
	
	# QT things
	xb2at.qrc
	xb2at.ui
)

add_executable(xb2at WIN32 ${XB2AT_SOURCES})

set_property(TARGET xb2at PROPERTY CXX_STANDARD 17)
set_property(TARGET xb2at PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(xb2at xb2core Qt5::Widgets)
target_include_directories(xb2at PRIVATE ${PROJECT_ROOT}/vendor/tinygltf)



