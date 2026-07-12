cmake_minimum_required(VERSION 3.13)

include (/code/pico/pico-sdk/pico_sdk_init.cmake)

project(tm1638 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(${PROJECT_NAME}
	main.c
	tm1638.h
)

target_include_directories(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(${PROJECT_NAME}
	pico_stdlib
	hardware_gpio
)

# Output to uart only
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

# Additional output files
pico_add_extra_outputs(${PROJECT_NAME})
