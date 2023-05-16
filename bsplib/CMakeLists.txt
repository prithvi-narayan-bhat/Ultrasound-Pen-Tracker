set(BSP_DIR ${CMAKE_CURRENT_SOURCE_DIR})                        # Set root directory

set(BSP_SRC_DIR ${BSP_DIR}/src)                                 # Set source directory
set(BSP_INC_DIR ${BSP_DIR}/include)                             # Set include directory

set(BSP_SRCS                                                    # Set sources
    ${BSP_SRC_DIR}/clock.c
    ${BSP_SRC_DIR}/gpio.c
    ${BSP_SRC_DIR}/i2c.c
    ${BSP_SRC_DIR}/nvic.c
    ${BSP_SRC_DIR}/timer.c
    ${BSP_SRC_DIR}/uart.c
    ${BSP_SRC_DIR}/wait.c
)

add_library(BSP_LIBRARY ${BSP_SRCS})                            # Create library

target_include_directories(BSP_LIBRARY PUBLIC ${BSP_INC_DIR})   # Export headers