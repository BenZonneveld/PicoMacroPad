# Adafruit GFX Library
# https://github.com/adafruit/Adafruit-GFX-Library
# BSD License

#cmake_minimum_required(VERSION 3.5)
#include(pico_sdk_import.cmake)
#pico_sdk_init()

#set(CMAKE_C_STANDARD 11)
#set(CMAKE_CXX_STANDARD 17)

#idf_component_register(SRCS "Adafruit_GFX.cpp" "Adafruit_GrayOLED.cpp" "Adafruit_SPITFT.cpp" "glcdfont.c" "Print.cpp"
#                       INCLUDE_DIRS "."
#                       REQUIRES arduino Adafruit_BusIO)

add_library(GFX-lib INTERFACE)
target_sources(GFX-lib INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/Adafruit_GFX.cpp
#	${CMAKE_CURRENT_LIST_DIR}/Adafruit_GrayOLED.cpp
	${CMAKE_CURRENT_LIST_DIR}/Adafruit_SPITFT.cpp
	${CMAKE_CURRENT_LIST_DIR}/glcdfont.c
	${CMAKE_CURRENT_LIST_DIR}/Print.cpp
)

target_include_directories(GFX-lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(GFX-lib INTERFACE pico_stdlib hardware_spi hardware_gpio hardware_dma)