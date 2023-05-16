set(PSP_DIR ${CMAKE_CURRENT_SOURCE_DIR})                        # Set root directory

set(PSP_SRC_DIR ${PSP_DIR}/src)                                 # Set source directory
set(PSP_INC_DIR ${PSP_DIR}/include)                             # Set include directory

set(PSP_SRCS                                                    # Set sources
    ${PSP_SRC_DIR}/eeprom.c
    ${PSP_SRC_DIR}/commands.c
    ${PSP_SRC_DIR}/lcd.c
    ${PSP_SRC_DIR}/strings.c
)

add_library(PSP_LIBRARY ${PSP_SRCS} ${PSP_INC_DIR})             # Create library

target_include_directories(PSP_LIBRARY PUBLIC ${PSP_INC_DIR})   # Export headers

target_link_libraries(PSP_LIBRARY PUBLIC BSP_LIBRARY)