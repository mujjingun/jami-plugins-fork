cmake_minimum_required(VERSION 3.10)

# set the project name
set (ProjectName AutoSubs)
set (Version 1.0)

project(${ProjectName} VERSION ${Version})

set (DAEMON ${PROJECT_SOURCE_DIR}/../../daemon)
set (PLUGIN_NAME AutoSubs)
set (JPL_FILE_NAME ${PLUGIN_NAME}.jpl)
set (DAEMON_SRC ${DAEMON}/src)
set (CONTRIB_PATH ${DAEMON}/contrib)
set (PLUGINS_LIB ${PROJECT_SOURCE_DIR}/../lib)
set (JPL_DIRECTORY ${PROJECT_BINARY_DIR}/jpl)
set (LIBS_DIR ${PROJECT_SOURCE_DIR}/../contrib/Libs)
set (preferencesFile preferences.json)

find_package(Freetype REQUIRED)
message(NOTICE "Freetype found: ${FREETYPE_LIBRARIES}")

if(WIN32)
    message(OS:\  WINDOWS\ ${CMAKE_SYSTEM_PROCESSOR})
    if (NOT ${CMAKE_CL_64})
        message( FATAL_ERROR "\nUse CMake only for x64 Windows" )
    endif()
    set (CONTRIB_PLATFORM_CURT x64)
    set (CONTRIB_PLATFORM ${CONTRIB_PLATFORM_CURT}-windows)
    set (LIBRARY_FILE_NAME ${PLUGIN_NAME}.dll)
    set (LIBS_BIN_DIR $ENV{PLUGIN_ENV})
endif()

if(UNIX)
    message(OS:\  LINUX\ ${CMAKE_SYSTEM_PROCESSOR})
    set (CONTRIB_PLATFORM_CURT ${CMAKE_SYSTEM_PROCESSOR})
    set (CONTRIB_PLATFORM ${CONTRIB_PLATFORM_CURT}-linux-gnu)
    set (LIBRARY_FILE_NAME lib${PLUGIN_NAME}.so)
    set (LIBS_BIN_DIR /home/${USER}/Libs)
endif()


message(Building:\   ${ProjectName}\   ${Version})
message(Build\ path:\ ${PROJECT_BINARY_DIR})
message(JPL\ assembling\ path:\ ${JPL_DIRECTORY})
message(JPL\ path:\ ${JPL_DIRECTORY}/../../../build/${ProjectName}/${JPL_FILE_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(WIN32)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

set(plugin_SRC main.cpp
               pluginMediaHandler.cpp
               pluginParameters.cpp
               videoSubscriber.cpp
               audiosubscriber.cpp
               )

set(plugin_HDR pluginMediaHandler.h
               pluginParameters.h
               videoSubscriber.h
               ../lib/accel.h
               ../lib/framescaler.h
               ../lib/pluglog.h
               )


# add the library
add_library(${ProjectName} SHARED ${plugin_SRC}
                                  ${plugin_HDR}
                                  )
if (WIN32)
target_include_directories(${ProjectName} PUBLIC ${PROJECT_BINARY_DIR}
                                                 ${PROJECT_SOURCE_DIR}
                                                 ${PLUGINS_LIB}
                                                 ${DAEMON_SRC}
                                                 ${CONTRIB_PATH}
                                                 ${FFMPEG}/include
                                                 ${CONTRIB_PATH}/build/opencv/build/install/include
                                                 ${FREETYPE_INCLUDE_DIRS}
                                                 )
target_link_directories(${ProjectName} PUBLIC ${CONTRIB_PATH}
                                        ${CONTRIB_PATH}/build/opencv/build/lib/Release
                                        ${CONTRIB_PATH}/build/opencv/build/3rdparty/lib/Release
                                        ${FFMPEG}/bin
                                        )

target_link_libraries(${ProjectName} PUBLIC swscale avutil opencv_imgcodecs411 opencv_imgproc411 opencv_core411 zlib ${FREETYPE_LIBRARIES})
endif()

if (UNIX)
target_include_directories(${ProjectName} PUBLIC ${PROJECT_BINARY_DIR}
                                                 ${PROJECT_SOURCE_DIR}
                                                 ${PLUGINS_LIB}
                                                 ${DAEMON_SRC}
                                                 ${CONTRIB_PATH}/${CONTRIB_PLATFORM}/include
                                                 ${CONTRIB_PATH}/${CONTRIB_PLATFORM}/include/opencv4
                                                 ${FREETYPE_INCLUDE_DIRS}
                                                 )
target_link_directories(${ProjectName} PUBLIC ${CONTRIB_PATH}
                                        ${CONTRIB_PATH}/${CONTRIB_PLATFORM}/lib
                                        ${CONTRIB_PATH}/${CONTRIB_PLATFORM}/lib/opencv4/3rdparty
                                        )
target_link_libraries(${ProjectName} PUBLIC swscale avutil opencv_imgcodecs opencv_imgproc opencv_core ${FREETYPE_LIBRARIES})
endif()

add_custom_command(
    TARGET ${ProjectName}
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory -r ${JPL_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E remove_directory -r ${JPL_DIRECTORY}/../../../build/${ProjectName}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data ${JPL_DIRECTORY}/data
    COMMAND ${CMAKE_COMMAND} -E make_directory ${JPL_DIRECTORY}/lib/${CONTRIB_PLATFORM}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/manifest.json ${JPL_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/${preferencesFile} ${JPL_DIRECTORY}/data
    COMMENT "Assembling Plugin files"
)

if (WIN32)
    add_custom_command(
        TARGET ${ProjectName}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${JPL_DIRECTORY}/../../../build/${CONTRIB_PLATFORM}
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/${ProjectName}.lib ${JPL_DIRECTORY}/lib/${CONTRIB_PLATFORM}
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/${LIBRARY_FILE_NAME} ${JPL_DIRECTORY}/lib/${CONTRIB_PLATFORM}
        COMMAND python ${PROJECT_SOURCE_DIR}/../assemble-plugin.py --plugins=${ProjectName}
        COMMENT "Generating JPL archive"
    )
else()
    add_custom_command(
        TARGET ${ProjectName}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${JPL_DIRECTORY}/../../../build/${CONTRIB_PLATFORM}
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/${LIBRARY_FILE_NAME} ${JPL_DIRECTORY}/lib/${CONTRIB_PLATFORM}
        COMMAND python ${PROJECT_SOURCE_DIR}/../assemble-plugin.py --plugins=${ProjectName}
        COMMENT "Generating JPL archive"
    )

endif()
