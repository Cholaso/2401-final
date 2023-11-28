cmake_minimum_required(VERSION 3.5.0)
project(2401-final VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

add_executable(fp evidence.c house.c hunter.c init.c logger.c main.c room.c utils.c ghost.c)


target_compile_options(fp PRIVATE -Wall -Wextra -Werror -g)#-fsanitize=thread
target_link_options(fp PRIVATE -pthread) # -fsanitize=thread
set(CPACK_PROJECT_NAME ${PROJECT_NAgME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)