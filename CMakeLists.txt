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

include(FetchContent)

# FetchContent_Declare (
#     googletest
#     GIT_REPOSITORY https://github.com/google/googletest.git
#     GIT_TAG        v1.14.0
# )

FetchContent_Declare (
    cpp-httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG        v0.15.3
)

FetchContent_Declare (
    fuzztest
    GIT_REPOSITORY https://github.com/google/fuzztest.git
    GIT_TAG        main
)

option(WEBSERV_BUILD_TEST "Build webserv test executable" OFF)

if (WEBSERV_BUILD_TEST)
    enable_testing()
endif()

project(webserv LANGUAGES CXX)
if (WEBSERV_BUILD_TEST)
    set(CMAKE_CXX_STANDARD 17)
else()    
    set(CMAKE_CXX_STANDARD 98)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WEBSERV_BUILD_TEST)
    FetchContent_MakeAvailable(fuzztest)
    fuzztest_setup_fuzzing_flags()
    SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=fuzzer -fsanitize=address")
endif()

add_library(webserv-lib)
add_executable(webserv)
if (WEBSERV_BUILD_TEST)
    add_executable(webserv-test)
endif()

file(GLOB_RECURSE sources "src/*.cpp" "src/*.tpp" "src/*.hpp")
list(REMOVE_ITEM sources ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_sources(webserv-lib PRIVATE "${sources}")
target_include_directories(webserv-lib PUBLIC src)

target_sources(webserv PRIVATE "src/main.cpp")

if (WEBSERV_BUILD_TEST)
    file(GLOB_RECURSE sources "tests/*.cpp" "tests/*.tpp" "tests/*.hpp")
    target_sources(webserv-test PRIVATE "${sources}")
    FetchContent_MakeAvailable(cpp-httplib)
    target_include_directories(webserv-test PRIVATE ${cpp-httplib_SOURCE_DIR})
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(webserv-lib PRIVATE -Wall -Wextra -Werror)
    target_compile_options(webserv PRIVATE -Wall -Wextra -Werror)
endif()    

target_link_libraries(webserv PRIVATE webserv-lib)
if (WEBSERV_BUILD_TEST)
    target_link_libraries(webserv-test PRIVATE webserv-lib)
    FetchContent_MakeAvailable(fuzztest)
    link_fuzztest(webserv-test)
endif()

if (WEBSERV_BUILD_TEST)
    include(GoogleTest)
    gtest_discover_tests(webserv-test WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()