add_library(touch INTERFACE)
target_sources(touch INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/Touch.cpp
)

target_include_directories(touch INTERFACE
	${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(touch INTERFACE
        hardware_spi
        hardware_dma
        pico_stdlib
)
