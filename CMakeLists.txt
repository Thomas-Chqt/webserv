#  ************************************************************************** */
#                                                                             */
#                                                         :::      ::::::::   */
#    CMakeLists.txt                                     :+:      :+:    :+:   */
#                                                     +:+ +:+         +:+     */
#    By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
#                                                 +#+#+#+#+#+   +#+           */
#    Created: 2024/02/10 13:34:45 by tchoquet          #+#    #+#             */
#                                                     ###   ########.fr       */
#                                                                             */
#  ************************************************************************** */

cmake_minimum_required(VERSION 3.23)

project(webserv LANGUAGES CXX C)

include(FetchContent)

FetchContent_Declare (
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)

option(WEBSERV_BUILD_TESTS "Build webserv test executable" OFF)

if (WEBSERV_BUILD_TESTS)
    enable_testing()
endif()

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(webserv-lib)
add_executable(webserv)
if (WEBSERV_BUILD_TESTS)
    add_library(http-parser)
    add_executable(webserv-test)
endif()

file(GLOB_RECURSE sources "src/*.cpp" "src/*.tpp" "src/*.hpp")
list(REMOVE_ITEM sources ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_sources(webserv-lib PRIVATE "${sources}")

target_sources(webserv PRIVATE "src/main.cpp")

if (WEBSERV_BUILD_TESTS)
    execute_process(COMMAND git submodule update --init ${CMAKE_CURRENT_SOURCE_DIR}/tests/http-parser)
    target_sources(http-parser PRIVATE tests/http-parser/http_parser.c tests/http-parser/http_parser.h)

    file(GLOB_RECURSE sources "tests/*.cpp" "tests/*.tpp" "tests/*.hpp")
    file(GLOB_RECURSE http-parser-files "tests/http-parser/*")
    list(REMOVE_ITEM sources ${http-parser-files})
    target_sources(webserv-test PRIVATE "${sources}")
endif()

target_include_directories(webserv-lib PUBLIC src)
if (WEBSERV_BUILD_TESTS)
    target_include_directories(http-parser PUBLIC tests/http-parser)
    target_include_directories(webserv-test PUBLIC tests)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(webserv-lib PRIVATE -Wall -Wextra -Werror)
    target_compile_options(webserv PRIVATE -Wall -Wextra -Werror)
endif()    

target_link_libraries(webserv webserv-lib)
if (WEBSERV_BUILD_TESTS)
    FetchContent_MakeAvailable(googletest)
    target_link_libraries(webserv-test webserv-lib GTest::gtest_main http-parser)
endif()

if (WEBSERV_BUILD_TESTS)
    include(GoogleTest)
    gtest_discover_tests(webserv-test WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

if (WEBSERV_BUILD_TESTS)
    find_program(NGINX_PATH nginx)
    if (NGINX_PATH STREQUAL NGINX_PATH-NOTFOUND)
        message(STATUS "Nginx not installed, tests will not be able to compare there result with nginx")
    else()
        target_compile_definitions(webserv-test PRIVATE NGINX_PATH="${NGINX_PATH}")
    endif()
endif()

if (WEBSERV_BUILD_TESTS)
    find_program(PYTHON_PATH python REQUIRED)
    file(GLOB_RECURSE WEBSERV_CONFIGS "tests/*/webserv.conf")
    foreach(file ${WEBSERV_CONFIGS})
        file(READ ${file} FILE_CONTENTS)
        string(REPLACE "<PYTHON_PATH>" "${PYTHON_PATH}" FILE_CONTENTS "${FILE_CONTENTS}")
        string(REGEX REPLACE "\\.[^.]*$" "" FILE_WITHOUT_EXT "${file}")
        file(WRITE "${FILE_WITHOUT_EXT}-local.conf" "${FILE_CONTENTS}")
    endforeach()
endif()

if (WEBSERV_BUILD_TESTS AND NOT NGINX_PATH STREQUAL NGINX_PATH-NOTFOUND)
    find_program(BREW_PATH brew REQUIRED)
    execute_process(COMMAND zsh "-c" "brew --prefix | tr -d '\n'" OUTPUT_VARIABLE BREW_PREFIX)
    find_program(FCGIWRAP_PATH fcgiwrap HINT "${BREW_PREFIX}/opt/fcgiwrap/sbin/fcgiwrap" REQUIRED)
    find_program(SPAWNFCGI_PATH spawn-fcgi REQUIRED)
    execute_process(COMMAND zsh "-c" "kill $(ps -A | grep \"${FCGIWRAP_PATH}\" | grep -v grep | while read -r first second rest; do echo $first; done)")
    execute_process(COMMAND spawn-fcgi -f "${FCGIWRAP_PATH}" -d ${CMAKE_SOURCE_DIR} -s ${CMAKE_SOURCE_DIR}/tests/fcgiwrap.sock)
endif()