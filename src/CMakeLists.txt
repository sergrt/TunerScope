cmake_minimum_required(VERSION 3.16)
project(TunerScope VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# <!-- TODO: Fix - for MSVC build
set(Qt6Core_DIR "c:/workspace/qt/qtbase/lib/cmake/Qt6Core")
set(Qt6_DIR "c:/workspace/qt/qtbase/lib/cmake/Qt6")
set(QT_QMAKE_EXECUTABLE "c:/workspace/qt/qtbase/bin/qmake.exe")
# -->

find_package(Qt6 REQUIRED COMPONENTS Core Quick Multimedia)

qt_standard_project_setup(REQUIRES 6.8)

set(HEADERS
    AudioEngine.h
    Globals.h
    ScaleModel.h
    SpectrumModel.h
    TunerModel.h
)

set(SOURCES
    main.cpp
    AudioEngine.cpp
    ScaleModel.cpp
    SpectrumModel.cpp
    TunerModel.cpp
)

set(QML_SOURCES
    qml/TunerScope/main.qml
    qml/TunerScope/CustomTooltip.qml
    qml/TunerScope/LabelHelper.qml
    qml/TunerScope/ScaleView.qml
    qml/TunerScope/SpectrumView.qml
    qml/TunerScope/TunerView.qml
)

set(OTHER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/../README.md
    ${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE
)

qt_add_executable(${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
    ${OTHER_FILES}
)

source_group("Headers" FILES ${HEADERS})
source_group("QML" FILES ${QML_SOURCES})
source_group("Sources" FILES ${SOURCES})
source_group("Other files" FILES ${OTHER_FILES})

qt_add_qml_module(${PROJECT_NAME}
    URI ${PROJECT_NAME}
    VERSION 1.0
    QML_FILES ${QML_SOURCES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
)

target_include_directories(${PROJECT_NAME}
    PRIVATE src
    PRIVATE ${THIRDPARTY_DIR}/fftw/api
)

target_link_directories(${PROJECT_NAME}
    PRIVATE ${THIRDPARTY_DIR}/fftw/build/Release
)
target_link_libraries(${PROJECT_NAME}
    PRIVATE Qt6::Core
    PRIVATE Qt6::Quick
    PRIVATE Qt6::Multimedia
    PRIVATE fftw3
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
