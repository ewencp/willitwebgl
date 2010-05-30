# CMakeLists.txt
# Copyright (c) 2010, Ewen Cheslack-Postava
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#    * Redistributions of source code must retain the above copyright notice,
#      this list of conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above copyright notice,
#      this list of conditions and the following disclaimer in the documentation
#      and/or other materials provided with the distribution.
#    * Neither the name of willitwebgl nor the names of its contributors
#      may be used to endorse or promote products derived from this software
#      without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
SET(CMAKE_VERBOSE_MAKEFILE OFF)
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug CACHE STRING
      "Build types include: Debug Release None RelWithDebInfo." FORCE
      )
ENDIF()

SET(WIW_DEFAULT_COMPILE_FLAGS "-Wall -Wno-unused-function -Wno-unused-variable -fvisibility=hidden")
SET(CMAKE_CXX_FLAGS ${WIW_DEFAULT_COMPILE_FLAGS})

PROJECT(wiw C CXX)

IF(NOT TOP_LEVEL)
  SET(TOP_LEVEL ${CMAKE_CURRENT_SOURCE_DIR}/..)
ENDIF()
GET_FILENAME_COMPONENT(TOP_LEVEL ${TOP_LEVEL} ABSOLUTE)


FIND_PACKAGE(OpenGL REQUIRED)


SET(WIW_SOURCE_DIR ${TOP_LEVEL}/src)

INCLUDE_DIRECTORIES(BEFORE
  ${OPENGL_INCLUDE_DIR}
  )

SET(WIW_SOURCES
  ${WIW_SOURCE_DIR}/willitwebgl.cpp
)

ADD_EXECUTABLE(willitwebgl ${WIW_SOURCES})
TARGET_LINK_LIBRARIES(willitwebgl
  ${OPENGL_gl_LIBRARY}
  ${OPENGL_glu_LIBRARY}
  )
