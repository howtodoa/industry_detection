cmake_minimum_required(VERSION 3.16)
project(Industry_Detection)

# 设置 Qt 的自动生成工具
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 Qt6 组件
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets VirtualKeyboard)




# 定义源文件
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
)

# 定义头文件
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
    "include/MZ_ClientControl.h"  # 相对路径，指向项目中的 include 目录
    "include/MZ_ADOConn.h"
    "include/Mz_LogConn.h"
    "include/MZ_VC3000.h"
     "include/MsvDeviceLib.h"

)

# 定义 UI 文件
set(UI_FILES
    mainwindow.ui
)


# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${UI_FILES}

  syspara.h syspara.cpp

  resources.qrc
  cameralabelwidget.h cameralabelwidget.cpp
  zoomablelabel.h zoomablelabel.cpp
  fullscreenwindow.h fullscreenwindow.cpp
  tcp_client.h tcp_client.cpp
  imageviewerwindow.h imageviewerwindow.cpp
  rangeclass.h rangeclass.cpp
  algoclass.h algoclass.cpp
  cameralclass.h cameralclass.cpp
  addcameradialog.h addcameradialog.cpp
  include/Mz_LogConn.h
  rezultinfo.h rezultinfo.cpp
  rezultinfo_nayin.h rezultinfo_nayin.cpp)

# 添加头文件路径
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include")

# 链接库

file(GLOB OPENVINO_LIBS "${CMAKE_SOURCE_DIR}/lib/Release_openvino/*.lib")

if(Qt6_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::VirtualKeyboard
        "${CMAKE_SOURCE_DIR}/lib/MZ_ClientControl.lib"  # 相对路径，指向项目中的 lib 目录
        "${CMAKE_SOURCE_DIR}/lib/MZ_ADOConnect.lib"
        "${CMAKE_SOURCE_DIR}/lib/MZ_Log.lib"
        "${CMAKE_SOURCE_DIR}/lib/VC3000DLL.lib"
        "${CMAKE_SOURCE_DIR}/lib/MZ_CameraDevice.lib"
        "${CMAKE_SOURCE_DIR}/lib/opencv_world490.lib"
         ${OPENVINO_LIBS}

         Dbghelp
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
if(OPENVINO_LIBS)
    message(STATUS "OpenVINO libs found:")
    foreach(lib ${OPENVINO_LIBS})
        message(STATUS "  ${lib}")
    endforeach()
else()
    message(STATUS "OpenVINO libs not found!")
endif()
set(CMAKE_AUTOMOC ON)

# 设置输出目录（可选）
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
