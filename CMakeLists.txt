cmake_minimum_required(VERSION 3.12)
project (serial_port)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#--------------------------------------- 第三方库 ---------------------------------------#

## QT ##
#自动调用moc，uic，rcc处理qt的扩展部分
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
# 设置Qt5的cmake目录
set(CMAKE_PREFIX_PATH C:/Qt/Qt5.14.2/5.14.2/msvc2017_64/lib/cmake)
# 找到对应的qt模块
find_package(Qt5 COMPONENTS Core Widgets Gui SerialPort REQUIRED)
# 添加QT5依赖库
set(QT5_LIBS Qt5::Core Qt5::Widgets Qt5::Gui  Qt5::SerialPort)

# ---------------------------------------- 添加资源文件 ---------------------------------------- #
AUX_SOURCE_DIRECTORY(./src ROOT_SOURCES)
SET(RC_SOURCES src/resource/images.qrc src/resource/logo.rc)


SET(PROJECT_SOURCES  ${ROOT_SOURCES} ${RC_SOURCES})

# ---------------------------------------- 生成可执行文件 ---------------------------------------- #
IF(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_executable(${PROJECT_NAME} WIN32 ${PROJECT_SOURCES})
ELSEIF(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
ENDIF()

#链接静态库
target_link_libraries(${PROJECT_NAME} ${QT5_LIBS})

#指定二进制文件位置
IF(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/release)
ELSEIF(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/debug)
ENDIF()