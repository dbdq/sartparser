# Copyright (c) 2014 Miguel Sarabia
# Imperial College London
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#


# CMAKE SETTINGS
#-------------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8.3)
project(SARTParser)

#Add cmake modules to cmake path
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeResources")


# LIBRARY VERSION
#-------------------------------------------------------------------------------
set(SARTParser_VERSION "5.0")


# DOXYGEN SETTINGS
#-------------------------------------------------------------------------------
set( PROJECT_VERSION ${SARTParser_VERSION} )
set( DOXYFILE_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/doc" )
set( DOXYFILE_LATEX OFF )
set( DOXYFILE_IN "${PROJECT_SOURCE_DIR}/CMakeResources/Doxyfile.in")
include(UseDoxygen OPTIONAL)


# COMPILER SETTINGS (Warnings and visibility)
#-------------------------------------------------------------------------------
#Set a lot of warnings in order to have a clean codebase
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  list(APPEND CMAKE_CXX_FLAGS "-Wall -Wextra -Wsign-conversion")
endif()


# USEFUL INSTALLATION PATHS
#-------------------------------------------------------------------------------
set(SARTParser_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin")
set(SARTParser_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib")
set(SARTParser_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include")
set(SARTParser_CMAKE_DIR "${CMAKE_INSTALL_PREFIX}/share/SARTParser/cmake")



# GET PUBLIC HEADERS MACRO
#-------------------------------------------------------------------------------
# Check files that finish by .h and .hxx but don't have .impl in them.
macro(get_public_headers HEADERS)
    #Loop through all arguments
    foreach( VALUE ${ARGN} )
        #And check whether they qualify as a public header
        if( ${VALUE} MATCHES ".*\\.h(xx)?$"
            AND NOT ${VALUE} MATCHES ".*\\.impl.*"
        )
            list(APPEND ${HEADERS} ${VALUE} )
        endif()
    endforeach(VALUE)
endmacro(get_public_headers)


# ADD SUBDIRECTORIES
#-------------------------------------------------------------------------------
add_subdirectory("src")

