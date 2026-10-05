cmake_minimum_required(VERSION 3.24)
project(soulcalibur_recomp VERSION 0.1.0 LANGUAGES C CXX)

# Soulcalibur (Dreamcast, USA) as a native PC port, built with dreamcomp.
# dreamcomp is the `dreamcomp/` submodule; a local checkout elsewhere can be used instead with
# -DDREAMCOMP_DIR=<path> (tools/dc.py passes the checkout it lives in).
set(DREAMCOMP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dreamcomp" CACHE PATH "dreamcomp checkout")
if(NOT EXISTS "${DREAMCOMP_DIR}/cmake/DreamcompPort.cmake")
  message(FATAL_ERROR "dreamcomp not found at ${DREAMCOMP_DIR}: git submodule update --init --recursive")
endif()
add_subdirectory(${DREAMCOMP_DIR} dreamcomp)
add_subdirectory(game)
