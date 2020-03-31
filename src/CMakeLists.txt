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


# EXTERNAL LIBRARIES
#-------------------------------------------------------------------------------
find_package(Eigen 3.0 REQUIRED)
list(APPEND INCLUDES ${Eigen_INCLUDE_DIRS})
include_directories( SYSTEM  ${INCLUDES} ) 


# SOURCES
#-------------------------------------------------------------------------------
set( LIB_SRCS 
    All.h
    Array.impl.h
    Common.h
    CellUtils.cpp
    CellUtils.impl.h
    CFGrammar.cpp
    CFGrammar.h
    CFGrammar.impl.h
    Grammar.cpp
    Grammar.impl.h
    Production.cpp
    Production.impl.h
    PTerminal.h
    SCell.cpp
    SCell.impl.h
    SGrammar.cpp
    SGrammar.impl.h
    SParser.cpp
    SParser.h
    SParserUtils.cpp
    SParserUtils.h
    SParserUtils.impl.h
    SRule.impl.h
    SState.cpp
    SState.impl.h
    Stream.cpp
    Stream.h
    Token.impl.h )

add_library( ${PROJECT_NAME} STATIC ${LIB_SRCS} )


# SET PUBLIC HEADERS
#-------------------------------------------------------------------------------
#Extract normal headers from list of sources
get_public_headers(HEADERS ${LIB_SRCS} )


# INSTALLATION
#-------------------------------------------------------------------------------
#Install library
install(TARGETS SARTParser EXPORT SARTParserDepends
        ARCHIVE DESTINATION "${SARTParser_LIB_DIR}"
        LIBRARY DESTINATION "${SARTParser_LIB_DIR}" )

#Install regular headers
install(FILES ${HEADERS} DESTINATION "${SARTParser_INCLUDE_DIR}/SARTParser")

#Install CMake configuration file
install(EXPORT SARTParserDepends DESTINATION ${SARTParser_CMAKE_DIR} )


# CONFIGURE CMAKE CONFIG FILES AND INSTALL THEM
#-------------------------------------------------------------------------------
# This one specifies the main variables that find_package will use
# @ONLY means only variables between @foo@ will be replaced
configure_file(
    "${CMAKE_SOURCE_DIR}/CMakeResources/SARTParserConfig.cmake.in"
    "${CMAKE_BINARY_DIR}/InstallFiles/SARTParserConfig.cmake" @ONLY
    )

# This one takes care of the version
configure_file(
    "${CMAKE_SOURCE_DIR}/CMakeResources/SARTParserConfigVersion.cmake.in"
    "${CMAKE_BINARY_DIR}/InstallFiles/SARTParserConfigVersion.cmake" @ONLY
    )

install(FILES
    "${CMAKE_BINARY_DIR}/InstallFiles/SARTParserConfig.cmake"
    "${CMAKE_BINARY_DIR}/InstallFiles/SARTParserConfigVersion.cmake"
    DESTINATION ${SARTParser_CMAKE_DIR}
    )


# ADD SUBDIRECTORIES
#-------------------------------------------------------------------------------
add_subdirectory("app")
add_subdirectory("python")

