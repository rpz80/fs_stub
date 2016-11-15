project("fs_stub_test")

set(SRC "test.c")
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} fs_stub)
