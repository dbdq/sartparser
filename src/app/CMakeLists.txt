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


# CMake settings
cmake_minimum_required(VERSION 2.8.3)

# Set includes (Need to get rid of this)
include_directories("../")

set(LIBRARIES SARTParser)

if( ${OpenCL_FOUND} )
    list(APPEND DEFINES "-DUSE_CLPARSER")
    list(APPEND LIBRARIES clSARTParser)
endif()


#Include macro to check for CXX11
include(CXX11)
check_for_cxx11_compiler(CXX11_COMPILER)
if( ${CXX11_COMPILER} )
    list(APPEND DEFINES "-DUSE_CXX11")
    enable_cxx11()
endif()

add_definitions( ${DEFINES} )


add_executable(sartparser parser.cpp)
target_link_libraries(sartparser ${LIBRARIES})

#Installation instructions
install(TARGETS sartparser
        RUNTIME DESTINATION "${SARTParser_BIN_DIR}" )
