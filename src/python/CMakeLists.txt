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


# TRY TO FIND DEPENDENCIES
#-------------------------------------------------------------------------------
#Start off by finding Boost.Python
find_package(Boost COMPONENTS python)

#Try to determine which version of Boost.Python CMake found
foreach( component in ${Boost_LIBRARIES} )
    get_filename_component(real_path ${component} REALPATH)
    if( ${real_path} MATCHES "python[-]py2" )
        set(BoostPython_VERSION 2)
    elseif( ${real_path} MATCHES "python[-]py3" )
        set( BoostPython_VERSION 3 )
    endif()
endforeach()

#If we still don't know what version of Boost.Python CMake found, assume it's v2.x
if( NOT BoostPython_VERSION )
    set(BoostPython_VERSION 2)
endif()

#Find appropriate versions of the interpreter and the libraries
find_package(PythonInterp ${BoostPython_VERSION})
find_package(PythonLibs ${BoostPython_VERSION})


if( ${Boost_FOUND} AND ${PYTHONINTERP_FOUND} AND ${PYTHONLIBS_FOUND} )
    message(STATUS "Building bindings for Python ${BoostPython_VERSION}")
    
    # SOURCES
    #---------------------------------------------------------------------------
    list(APPEND PY_SRCS "pysartparser.cpp")
    foreach( file ${LIB_SRCS} )
        list(APPEND PY_SRCS "../${file}")
    endforeach()
    
    # ACTUAL COMPILATION
    #---------------------------------------------------------------------------
    add_library( pysartparser SHARED ${PY_SRCS} )
    target_link_libraries(pysartparser ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} )
    include_directories( SYSTEM 
        ${INCLUDES} ${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS} )
    
    # ASSEMBLE PYTHON PACKAGE (COPY LIBRARIES AND .PY FILES)
    #---------------------------------------------------------------------------
    set(PY_PKG "${CMAKE_BINARY_DIR}/python-package/sartparser")
    
    add_custom_command( TARGET pysartparser POST_BUILD 
        COMMAND "${CMAKE_COMMAND}" 
            -E copy "$<TARGET_FILE:pysartparser>" "${PY_PKG}/pysartparser.so"
        COMMAND "${CMAKE_COMMAND}"
            -E copy "__init__.py" "${PY_PKG}/__init__.py"
        COMMAND "${CMAKE_COMMAND}"
            -E copy "monkeypatch.py" "${PY_PKG}/monkeypatch.py"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"  
        COMMENT "Assembling Python package" )
    
    # CONFIGURE SETUP.PY
    #---------------------------------------------------------------------------
    configure_file(
        "${CMAKE_SOURCE_DIR}/CMakeResources/setup.py.in"
        "${CMAKE_BINARY_DIR}/setup.py" @ONLY )
    
    # ADD CUSTOM TARGET FOR INSTALLATION OF PYTHON BINDINGS
    #---------------------------------------------------------------------------
    add_custom_target( install_python
            COMMAND "${PYTHON_EXECUTABLE}"
                "setup.py" "install" "--prefix=${CMAKE_INSTALL_PREFIX}"
            COMMENT "Installing python bindings"
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

else()
    message(STATUS
        "Boost or Python not found, Python bindings will not be built")
endif()

