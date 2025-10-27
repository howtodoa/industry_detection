cmake_minimum_required(VERSION 3.16)
project(Industry_Detection)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets VirtualKeyboard)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

set(SOURCES
    main.cpp
    mainwindow.cpp
    cameral.cpp
    cameralmenu.cpp
    public.cpp
    login.cpp
    role.cpp
    virtualbox.cpp
    parawidget.cpp
    fileoperator.cpp
    syspara.cpp
    cameralabelwidget.cpp
    zoomablelabel.cpp
    fullscreenwindow.cpp
    tcp_client.cpp
    imageviewerwindow.cpp
    rangeclass.cpp
    algoclass.cpp
    cameralclass.cpp
    addcameradialog.cpp
    rezultinfo.cpp
    rezultinfo_nayin.cpp
    imageprocess.cpp
    imagesavingworker.cpp
    localfiledetector.cpp
    rezultinfo_plate.cpp
    rezultinfo_lift.cpp
    rezultinfo_abut.cpp
    imagepaint.cpp
    algoclass_nayin.cpp
    algoclass_lift.cpp
    algoclass_plate.cpp
    algoclass_top.cpp
    algoclass_pin.cpp
    algoclass_side.cpp
    initsystem.cpp
    directorymonitorworker.cpp
    rezultinfo_top.cpp
    rezultinfo_side.cpp
    imageprocess_brader.cpp
    rezultinfo_pin.cpp
    displayinfowidget.cpp
    rightcontrolpanel.cpp
    aboutwidget.cpp
    debugtool.cpp
    algoclass_abut.cpp
    imageprocess_plate.cpp
)

set(HEADERS
    mainwindow.h
    cameral.h
    cameralmenu.h
    public.h
    login.h
    role.h
    virtualbox.h
    parawidget.h
    fileoperator.h
    "include/MZ_ClientControl.h"
    "include/MZ_ADOConn.h"
    "include/Mz_LogConn.h"
    "include/MZ_VC3000H.h"
    "include/MZ_VC3000.h"
    "include/MsvDeviceLib.h"
    "include/CapacitanceProgram.h"
    "include/DBOperation.h"
    syspara.h
    cameralabelwidget.h
    zoomablelabel.h
    fullscreenwindow.h
    tcp_client.h
    imageviewerwindow.h
    rangeclass.h
    algoclass.h
    cameralclass.h
    addcameradialog.h
    rezultinfo.h
    rezultinfo_nayin.h
    typdef.h
    localfiledetector.h
    rezultinfo_plate.h
    rezultinfo_lift.h
    rezultinfo_abut.h
    imagepaint.h
    algoclass_nayin.h
    algoclass_lift.h
    algoclass_plate.h
    algoclass_top.h
    imageprocess_brader.h
    algoclass_pin.h
    algoclass_side.h
    initsystem.h
    directorymonitorworker.h
    rezultinfo_top.h
    rezultinfo_side.h
    rezultinfo_pin.h
    displayinfowidget.h
    rightcontrolpanel.h
    aboutwidget.h
    debugtool.h
    algoclass_abut.h
    imageprocess_plate.h
)

set(UI_FILES
    mainwindow.ui
)

set(QRC_FILE
    resources.qrc
)

set(RC_FILE
    application.rc
)

add_executable(${PROJECT_NAME}  ${SOURCES} ${HEADERS} ${UI_FILES} ${RC_FILE} ${QRC_FILE})

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    #target_compile_definitions(${PROJECT_NAME} PRIVATE QT_NO_DEBUG_OUTPUT)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
endif()

if(USE_MAIN_WINDOW_CAPACITY)
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_MAIN_WINDOW_CAPACITY=1)
    message(STATUS "CMake: Building with USE_DEFAULT_MAIN_WINDOW_MACRO defined (for MainWindow::MainWindow()).")
else()
    #target_compile_definitions(${PROJECT_NAME} PRIVATE USE_PARAM_MAIN_WINDOW_MACRO=1)
    message(STATUS "CMake: Building with USE_PARAM_MAIN_WINDOW_MACRO defined (for MainWindow::MainWindow(int)).")
endif()

if(MSVC AND CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Configuring Release with debug info and no optimization for MSVC")
    set(CMAKE_CXX_FLAGS_RELEASE "/Od /Zi /MD")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/DEBUG")
endif()

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include")

set(OPENCV_LIB_TO_LINK "")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(OPENCV_LIB_TO_LINK "${CMAKE_SOURCE_DIR}/lib/opencv_world490d.lib")
else()
    set(OPENCV_LIB_TO_LINK "${CMAKE_SOURCE_DIR}/lib/opencv_world460.lib")
endif()

file(GLOB OPENVINO_LIBS "${CMAKE_SOURCE_DIR}/lib/Release_openvino/*.lib")

if(Qt6_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::VirtualKeyboard
        "${CMAKE_SOURCE_DIR}/lib/MZ_ClientControl.lib"
        "${CMAKE_SOURCE_DIR}/lib/MZ_ADOConnect.lib"
        "${CMAKE_SOURCE_DIR}/lib/MZ_Log.lib"
        "${CMAKE_SOURCE_DIR}/lib/SqliteDB.lib"
        "${CMAKE_SOURCE_DIR}/lib/VC3000HDLL.lib"
        "${CMAKE_SOURCE_DIR}/lib/VC3000DLL.lib"
        "${CMAKE_SOURCE_DIR}/lib/MZ_CameraDevice.lib"
        "${CMAKE_SOURCE_DIR}/lib/CapacitanceProgram.lib"
        "${CMAKE_SOURCE_DIR}/lib/BraidedTape.lib"
        ${OPENVINO_LIBS}
        ${OPENCV_LIB_TO_LINK}
        Dbghelp
        dwmapi
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::VirtualKeyboard
        "${CMAKE_SOURCE_DIR}/lib/MZ_ClientControl.lib"
        "${CMAKE_SOURCE_DIR}/lib/MZ_ADOConnect.lib"
    )
endif()
target_compile_options(${PROJECT_NAME} PRIVATE /EHa)
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)