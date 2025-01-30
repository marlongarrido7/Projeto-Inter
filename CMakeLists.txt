# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Inicializa o SDK do Pico
include(pico_sdk_import.cmake)

project(inter C CXX ASM)

# Inicializa o SDK
pico_sdk_init()

# Adiciona o executável
add_executable(inter
    inter.c
)

# Processa o arquivo PIO
pico_generate_pio_header(inter ${CMAKE_CURRENT_LIST_DIR}/inter.pio)

# Configura o nome do programa e a versão
pico_set_program_name(inter "inter")
pico_set_program_version(inter "0.1")

# Configura UART e USB
pico_enable_stdio_uart(inter 0)
pico_enable_stdio_usb(inter 1)

# Linka as bibliotecas necessárias
target_link_libraries(inter
    pico_stdlib
    hardware_pio
    hardware_clocks
    hardware_gpio
)

# Inclui o diretório atual
target_include_directories(inter PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Adiciona saídas extras (UF2, bin, etc.)
pico_add_extra_outputs(inter)

