# Adafruit GFX Library
# https://github.com/adafruit/Adafruit-GFX-Library
# BSD License

add_library(GFX-lib INTERFACE)
target_sources(GFX-lib INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/GFX-lib/Adafruit_GFX.cpp
	${CMAKE_CURRENT_LIST_DIR}/GFX-lib/Adafruit_SPITFT.cpp
	${CMAKE_CURRENT_LIST_DIR}/GFX-lib/glcdfont.c
	${CMAKE_CURRENT_LIST_DIR}/GFX-lib/Print.cpp
	${CMAKE_CURRENT_LIST_DIR}/mmc_sd/MMC_SD.cpp
	${CMAKE_CURRENT_LIST_DIR}/TFTSDTouch.cpp
)

target_include_directories(GFX-lib INTERFACE 
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/GFX-lib
	${CMAKE_CURRENT_LIST_DIR}/mmc_sd
)

target_link_libraries(GFX-lib INTERFACE pico_stdlib hardware_spi hardware_gpio hardware_dma)