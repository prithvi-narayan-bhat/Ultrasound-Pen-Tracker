set(BOARD_DIR ${CMAKE_CURRENT_SOURCE_DIR})                      # Set root directory

set(BOARD_SRC_DIR ${BOARD_DIR}/src)                             # Set source directory
set(BOARD_INC_DIR ${BOARD_DIR}/include)                         # Set include directory

set(BOARD_SRCS                                                  # Set sources
    ${BOARD_SRC_DIR}/clock.c
    ${BOARD_SRC_DIR}/gpio.c
    ${BOARD_SRC_DIR}/i2c.c
    ${BOARD_SRC_DIR}/nvic.c
    ${BOARD_SRC_DIR}/timer.c
    ${BOARD_SRC_DIR}/uart.c
    ${BOARD_SRC_DIR}/wait.c
)

add_library(BOARD_LIB ${BOARD_SRCS})                            # Create library

target_include_directories(BOARD_LIB PUBLIC ${BOARD_INC_DIR})   # Export headers