cmake_minimum_required(VERSION 3.16)

project(QWidebandSpectrogram VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(OpenMP)


set(PROJECT_SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/tabviewercontainer.cpp
        src/form_specviewer1.cpp
		src/form_specviewer2.cpp
        src/form_specviewer3.cpp 
        src/spectrumviewer.cpp
        src/specviewer_axisfreq.cpp
        src/specviewer_axistime.cpp
    )

set(PROJECT_UI
    src/tabviewercontainer.ui
    src/form_specviewer1.ui
	src/form_specviewer2.ui
	src/form_specviewer3.ui
    src/spectrumviewer.ui
    src/specviewer_axisfreq.ui
    src/specviewer_axistime.ui
    src/mainwindow.ui
    )
		
set(PROJECT_INCLUDES
        include/tabviewercontainer.h 
        include/spectrumviewer.h 
        include/specviewer_axisfreq.h
        include/specviewer_axistime.h
        include/fftw_allocator.h
        include/mainwindow.h
        include/form_specviewer1.h
        include/form_specviewer2.h
		include/form_specviewer3.h 
)



set(CMAKE_INCLUDE_CURRENT_DIR ON)
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(QWidebandSpectrogram
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${PROJECT_UI}
        ${PROJECT_INCLUDES}


    )
	
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET QWidebandSpectrogram APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(QWidebandSpectrogram SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(QWidebandSpectrogram
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(QWidebandSpectrogram PUBLIC include)

add_subdirectory(vendor/fmt-11.0.2)
target_include_directories(QWidebandSpectrogram PRIVATE "vendor/fmt/include")
target_link_libraries(QWidebandSpectrogram PRIVATE fmt)

target_include_directories(QWidebandSpectrogram PRIVATE "vendor/fftw")
target_link_directories(QWidebandSpectrogram PRIVATE "${CMAKE_SOURCE_DIR}/vendor/fftw")
target_link_libraries(QWidebandSpectrogram PRIVATE "libfftw3f-3")
target_link_libraries(QWidebandSpectrogram PRIVATE "libfftw3-3")

target_link_libraries(QWidebandSpectrogram PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)


if(OpenMP_CXX_FOUND)
    target_link_libraries(QWidebandSpectrogram PUBLIC OpenMP::OpenMP_CXX)
endif()
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)
# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.QWidebandSpectrogram)
endif()
set_target_properties(QWidebandSpectrogram PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS QWidebandSpectrogram
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(QWidebandSpectrogram)
endif()

set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ -lwsock32 -lws2_32 ${CMAKE_CXX_STANDARD_LIBRARIES}")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")


