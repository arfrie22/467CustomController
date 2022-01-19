cmake_minimum_required(VERSION 3.17)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)
#include(${PICO_SDK_PATH}/tools/CMakeList.txt)

project(467CustomController C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


#pico_sdk_init()

set(FAMILY rp2040)
set(BOARD pico_sdk)
set(TINYUSB_FAMILY_PROJECT_NAME_PREFIX "tinyusb_dev_")

include(${PICO_SDK_PATH}/lib/tinyusb/hw/bsp/family_support.cmake)
include(${PICO_SDK_PATH}/lib/tinyusb/hw/bsp/${FAMILY}/family.cmake)


# gets PROJECT name for the example (e.g. <BOARD>-<DIR_NAME>)
family_get_project_name(PROJECT ${CMAKE_CURRENT_LIST_DIR})

project(${PROJECT})

# Checks this example is valid for the family and initializes the project
family_initialize_project(${PROJECT} ${CMAKE_CURRENT_LIST_DIR})

add_executable(${PROJECT} src/main.c src/usb_descriptors.c src/data_protocol.h src/led.c src/led.h)

# Example source
target_sources(${PROJECT} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/usb_descriptors.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/led.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/led.h
        )

# Example include
target_include_directories(${PROJECT} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/generated
        )

# Configure compilation flags and libraries for the example... see the corresponding function
# in hw/bsp/FAMILY/family.cmake for details.
family_configure_device_example(${PROJECT})

# generate the header file into the source tree as it is included in the RP2040 datasheet
#pico_generate_pio_header(${PROJECT} ${CMAKE_CURRENT_LIST_DIR}/src/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/src/generated)
target_link_libraries(${PROJECT} PUBLIC pico_stdlib hardware_pio hardware_dma)

#add_executable(467CustomController src/main.c src/usb_descriptors.c src/usb_descriptors.h src/tusb_config.h)
#
## Pull in our pico_stdlib which pulls in commonly used features
#target_link_libraries(467CustomController pico_stdlib)
#
## create map/bin/hex file etc.
#pico_add_extra_outputs(467CustomController)

