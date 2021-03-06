cmake_minimum_required(VERSION 3.5)

#include(../../pico_sdk_import.cmake)

include(${PICO_SDK_PATH}/lib/tinyusb/hw/bsp/family_support.cmake)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

#set(PROJECT FirstLight)
set(FAMILY rp2040)
set(BOARD raspberry_pi_pico)
# Default path to the SDK
set(PICO_BOARD pico)

add_subdirectory(FatFS build)
#add_subdirectory(MIDI)
# Pull in Pico SDK (must be before project)
family_get_project_name(PROJECT ${CMAKE_CURRENT_LIST_DIR})
project(${PROJECT} C CXX ASM)


family_initialize_project(${PROJECT} ${CMAKE_CURRENT_LIST_DIR})
#pico_sdk_init()


add_executable(${PROJECT})
pico_generate_pio_header(${PROJECT} ${CMAKE_CURRENT_LIST_DIR}/Debounce/debounce.pio)

target_include_directories(${PROJECT} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/FatFS
        ${CMAKE_CURRENT_SOURCE_DIR}/MacroHandler
        ${CMAKE_CURRENT_SOURCE_DIR}/ImageReader
        ${CMAKE_CURRENT_SOURCE_DIR}/Debounce
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI
        ${CMAKE_CURRENT_SOURCE_DIR}/Touch
        ${CMAKE_CURRENT_SOURCE_DIR}/MIDI
)        

target_sources(${PROJECT} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/MacroPad.cpp
#  ${CMAKE_CURRENT_SOURCE_DIR}/core1Tasks.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tud.c
  ${CMAKE_CURRENT_SOURCE_DIR}/msc_disk.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/usb_descriptors.c
  ${CMAKE_CURRENT_SOURCE_DIR}/sdspi_config.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Touch/Touch.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/MacroHandler/macrohandler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/MacroHandler/TinyUSB_Mouse_and_Keyboard.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ImageReader/ImageReader.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Debounce/debounce.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/GUI/gui.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/GUI/tools.c
  ${CMAKE_CURRENT_SOURCE_DIR}/MIDI/MIDI.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/MIDI/MIDI.hpp
)

pico_set_program_name(${PROJECT} "MacroPad")
pico_set_program_version(${PROJECT} "0.5")

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT} 0)
pico_enable_stdio_uart(${PROJECT} 1)
#set(PICO_STDIO_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE 1)
#set(PICO_STDIO_USB_RESET_INTERFACE_SUPPORT_RESET_TO_BOOTSEL  1)

target_link_libraries(${PROJECT} PUBLIC
        pico_stdlib
        pico_bootsel_via_double_reset
        hardware_watchdog
        hardware_gpio
        hardware_pio
        hardware_spi
        hardware_dma
#        pico_multicore
        GFX-lib
        FatFs_SPI
#        midi
)

family_configure_device_example(${PROJECT})        
#pico_add_extra_outputs(${PROJECT})

add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND picotool reboot -f -u 
    VERBATIM
    COMMAND ping -n 2 localhost 2> nul
    VERBATIM
    COMMAND	picotool load -x ${PROJECT}.uf2
    VERBATIM
)
