cmake_minimum_required(VERSION 3.10)
project(TivaC_Projects C ASM)

# Set the Tiva C compiler and linker
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)

# Compiler and linker flags
set(CMAKE_C_FLAGS "-mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -Os -ffunction-sections -fdata-sections -MD -std=c99")
set(CMAKE_EXE_LINKER_FLAGS "-T ${CMAKE_SOURCE_DIR}/ld_scripts/tm4c123gh6pm.ld -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -specs=nano.specs -Wl,--gc-sections")

add_subdirectory(psplib)
add_subdirectory(bsplib)
add_subdirectory(app)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)     # Set the output directory for executables