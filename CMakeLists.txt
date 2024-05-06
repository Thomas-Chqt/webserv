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

add_library(webserv-lib)
add_executable(webserv)

file(GLOB_RECURSE sources "src/*.cpp" "src/*.tpp" "src/*.hpp")
list(REMOVE_ITEM sources ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

target_sources(webserv-lib PRIVATE "${sources}")

target_include_directories(webserv-lib PUBLIC src)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(webserv-lib PRIVATE -Wall -Wextra -Werror)
endif()

target_sources(webserv PRIVATE "src/main.cpp")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(webserv PRIVATE -Wall -Wextra -Werror)
endif()

target_link_libraries(webserv webserv-lib)

if (WEBSERV_BUILD_TEST)
    add_subdirectory(tests)
endif()