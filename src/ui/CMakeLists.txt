set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(XB2AT_UI_SOURCES
	main.cpp
	
	# Extraction worker
	ExtractionWorker.h
	ExtractionWorker.cpp

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
# fixes clang screaming about microsoft include rules
target_include_directories(xb2at PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(xb2at xb2core Qt5::Widgets)

# Install xb2at in the install root
install(TARGETS xb2at 
 RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}"
)

if(WIN32)
# Install the Qt libraries into the installation prefix.

message(STATUS "Adding install code to deploy Qt libraries into install prefix.")
get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/temp_xb2at_path" CONTENT "$<TARGET_FILE:xb2at>")

install(CODE
	"
	file(READ \"${CMAKE_CURRENT_BINARY_DIR}/temp_xb2at_path\" _file)

	execute_process(
		COMMAND \"${CMAKE_COMMAND}\" -E
			env PATH=\"${_qt_bin_dir}\" \"${WINDEPLOYQT_EXECUTABLE}\"
				--dry-run
				--no-compiler-runtime
				--no-translations
				--no-system-d3d-compiler
				--no-angle
				--no-opengl-sw
				--list mapping
				\${_file}
		OUTPUT_VARIABLE _output
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	separate_arguments(_files WINDOWS_COMMAND \${_output})
	while(_files)
		list(GET _files 0 _src)
		list(GET _files 1 _dest)
		message(STATUS \"Copying \${_src} -> \${_dest} \")
		execute_process(
			COMMAND \"${CMAKE_COMMAND}\" -E
				copy \${_src} \"\${CMAKE_INSTALL_PREFIX}/\${_dest}\"
		)
		list(REMOVE_AT _files 0 1)
	endwhile()
	"
)

endif()