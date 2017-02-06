 cmake_minimum_required(VERSION 2.8)
 project( dlib_face )
 set(CMAKE_BUILD_TYPE Release) # VERY essential for dlib!

 find_package( OpenCV REQUIRED )
 include_directories( ${OpenCV_INCLUDE_DIRS}) 
 #add_executable( dlib_face dlib_face.cpp )
 ADD_LIBRARY(dlib_face SHARED dlib_face.cpp) # SHARED makes .so
 target_link_libraries( dlib_face ${OpenCV_LIBS})  
 
 
 target_link_libraries( dlib_face dlib) 
 target_link_libraries( dlib_face ${dlib_needed_libraries})
 
  
 find_package(dlib REQUIRED HINTS "/usr/local/lib") #possibly supporting components!
 message(STATUS "Using dlib-${dlib_VERSION}")
 include_directories(${dlib_INCLUDE_DIRS})
 target_link_libraries(dlib_face ${dlib_LIBRARIES})
 
 option(DLIB_NO_GUI_SUPPORT ${DLIB_NO_GUI_SUPPORT_STR} OFF)
 
       
         
option( USE_PYTHON2 "If on, link to Python 2 instead of 3" OFF )

if ( USE_PYTHON2 )
  set( Python_ADDITIONAL_VERSIONS 2.7 2.6 )
  find_package( PythonLibs 2.6 REQUIRED )  # 2.6 is ONLY the mininum

  if ( NOT PYTHONLIBS_VERSION_STRING VERSION_LESS "3.0.0" )
    message( FATAL_ERROR
      "You set USE_PYTHON2 to make CMake find python2 libs only, but CMake "
      "found python3 libs instead. Either don't set USE_PYTHON2 or install the "
      "necessary python2 headers & libraries." )
  endif()
else()
  set( Python_ADDITIONAL_VERSIONS 3.6 3.5 3.4 3.3 )
  find_package( PythonLibs 3.3 REQUIRED )  # 3.3 is ONLY the mininum
endif()


 find_package(PythonLibs REQUIRED )
 

 include_directories(${PYTHON_INCLUDE_DIRS})
 target_link_libraries( dlib_face ${PYTHON_LIBRARIES} ) 
 
 
 
 
         
# instructions -msse4 -mavx needed for dlib to run faster
add_definitions(-DDLIB_HAVE_SSE2)
add_definitions(-DDLIB_HAVE_SSE3)
add_definitions(-DDLIB_HAVE_SSE41)

option(USE_SSE2_INSTRUCTIONS "Compile your program with SSE2 instructions" ON)
option(USE_SSE4_INSTRUCTIONS "Compile your program with SSE4 instructions" ON)
option(USE_AVX_INSTRUCTIONS "Compile your program with AVX instructions" ON)
 
 include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)

if(COMPILER_SUPPORTS_CXX11)
    # instructions -msse4 -mavx needed for dlib to run faster
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -msse -msse2 -msse3 -msse4 -mavx")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++14 support. Please use a different C++ compiler.")
endif()
