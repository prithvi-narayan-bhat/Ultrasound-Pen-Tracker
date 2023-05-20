set(APP_DIR ${CMAKE_CURRENT_SOURCE_DIR}) # Set root directory

set(UPATS_SRC ${APP_DIR}/upats.c PARENT_SCOPE)

add_executable(upats
    ${APP_DIR}/upats.c
)

target_include_directories(upats PUBLIC
    ${BOARD_INC_DIR}
    ${UTILS_INC_DIR}
)

target_link_libraries(upats PRIVATE
    BOARD_LIB
    UTILS_LIB
)
