# Adafruit GFX Library
# https://github.com/adafruit/Adafruit-GFX-Library
# BSD License

add_library(TFTSDTouch INTERFACE)
target_sources(TFTSDTouch INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/GFX-lib/Adafruit_GFX.cpp
	${CMAKE_CURRENT_LIST_DIR}/GFX-lib/Adafruit_SPITFT.cpp
	${CMAKE_CURRENT_LIST_DIR}/GFX-lib/glcdfont.c
	${CMAKE_CURRENT_LIST_DIR}/GFX-lib/Print.cpp
	${CMAKE_CURRENT_LIST_DIR}/mmc_sd/MMC_SD.cpp
	${CMAKE_CURRENT_LIST_DIR}/mmc_sd/fatfs/ff.cpp
	${CMAKE_CURRENT_LIST_DIR}/mmc_sd/fatfs/diskio.cpp
	${CMAKE_CURRENT_LIST_DIR}/ImageReader/ImageReader.cpp
	${CMAKE_CURRENT_LIST_DIR}/Touch/Touch.cpp
	${CMAKE_CURRENT_LIST_DIR}/TFTSDTouch.cpp
)

target_include_directories(TFTSDTouch INTERFACE 
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/GFX-lib
	${CMAKE_CURRENT_LIST_DIR}/mmc_sd
	${CMAKE_CURRENT_LIST_DIR}/mmc_sd/fatfs
)

target_link_libraries(TFTSDTouch INTERFACE pico_stdlib hardware_spi hardware_gpio hardware_dma)