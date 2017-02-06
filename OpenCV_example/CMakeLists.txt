 cmake_minimum_required(VERSION 2.8)
 project( example )
 
 set(CMAKE_BUILD_TYPE Release)

 
 add_executable( example example.cpp )

  
 find_package(dlib REQUIRED HINTS "/usr/local/lib") #possibly supporting components!
 message(STATUS "Using dlib-${dlib_VERSION}")
 include_directories(${dlib_INCLUDE_DIRS})
 target_link_libraries(example ${dlib_LIBRARIES})
 
 option(DLIB_NO_GUI_SUPPORT ${DLIB_NO_GUI_SUPPORT_STR} OFF)
 
 
 if (NOT DLIB_NO_GUI_SUPPORT)
            find_library(xlib X11)
            # make sure X11 is in the include path
            find_path(xlib_path Xlib.h
               PATHS 
               /usr/include/X11
               PATH_SUFFIXES X11
               )
            if (xlib AND xlib_path)
               get_filename_component(x11_path ${xlib_path} PATH CACHE)
               include_directories(${x11_path})
               set(dlib_needed_libraries ${dlib_needed_libraries} ${xlib} )
            else()
               set(DLIB_NO_GUI_SUPPORT ON CACHE STRING ${DLIB_NO_GUI_SUPPORT_STR} FORCE )
            endif()
         endif()
         
         
 find_package( OpenCV REQUIRED )
 include_directories( ${OpenCV_INCLUDE_DIRS})  
 target_link_libraries( example ${OpenCV_LIBS}) 
         
# instructions -msse4 -mavx needed for dlib to run faster
add_definitions(-DDLIB_HAVE_SSE2)
add_definitions(-DDLIB_HAVE_SSE3)
add_definitions(-DDLIB_HAVE_SSE41)

option(USE_SSE2_INSTRUCTIONS "Compile your program with SSE2 instructions" ON)
option(USE_SSE4_INSTRUCTIONS "Compile your program with SSE4 instructions" ON)
option(USE_AVX_INSTRUCTIONS "Compile your program with AVX instructions" ON)
 
 include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    # instructions -msse4 -mavx needed for dlib to run faster
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -msse -msse2 -msse3 -msse4 -mavx")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -msse -msse2 -msse3 -msse4 -mavx")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()
