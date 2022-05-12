cmake_minimum_required(VERSION 3.5)

include(pico_sdk_import.cmake)

project(MacroPad)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_PARALLEL_LEVEL 512)
set(FAMILY rp2040)
set(BOARD raspberry_pi_pico)
set(PICO_BOARD pico)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rapidjson
	${CMAKE_CURRENT_SOURCE_DIR}/libs/
)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/MacroHandler)

#family_initialize_project(TFT_Screen ${CMAKE_CURRENT_LIST_DIR})

# family_add_subdirectory will filter what to actually add based on selected FAMILY
#family_add_subdirectory(midi)
add_subdirectory(libs)
add_subdirectory(MacroPad)
#add_subdirectory(Examples)