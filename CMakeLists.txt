cmake_minimum_required(VERSION 3.16)
project(TunerScope VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Qt6Core_DIR "c:/workspace/qt/qtbase/lib/cmake/Qt6Core")
set(Qt6_DIR "c:/workspace/qt/qtbase/lib/cmake/Qt6")
set(QT_QMAKE_EXECUTABLE "c:/workspace/qt/qtbase/bin/qmake.exe")
find_package(Qt6 REQUIRED COMPONENTS Core Quick Multimedia)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appTunerScope
    src/globals.h
    src/main.cpp
    src/AudioEngine.cpp
    src/AudioEngine.h
    src/SpectrumModel.cpp
    src/SpectrumModel.h
    src/ScaleModel.cpp
    src/ScaleModel.h
    src/TunerModel.cpp
    src/TunerModel.h
)



qt_add_qml_module(appTunerScope
    URI TunerScope
    VERSION 1.0
    QML_FILES
        qml/TunerScope/main.qml
        qml/TunerScope/TunerView.qml
        qml/TunerScope/SpectrumView.qml
        qml/TunerScope/ScaleView.qml
        qml/TunerScope/CustomTooltip.qml
        qml/TunerScope/LabelHelper.qml
)
set_target_properties(appTunerScope PROPERTIES
    WIN32_EXECUTABLE TRUE
)

target_include_directories(appTunerScope
    PRIVATE src
    PRIVATE 3rdparty/fftw/api
)

target_link_directories(appTunerScope
    PRIVATE 3rdparty/fftw/build/Release
)
target_link_libraries(appTunerScope
    PRIVATE Qt6::Core
    PRIVATE Qt6::Quick
    PRIVATE Qt6::Multimedia
    PRIVATE fftw3
)
include(GNUInstallDirs)
install(TARGETS appTunerScope
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
