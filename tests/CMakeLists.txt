#  ************************************************************************** */
#                                                                             */
#                                                         :::      ::::::::   */
#    CMakeLists.txt                                     :+:      :+:    :+:   */
#                                                     +:+ +:+         +:+     */
#    By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
#                                                 +#+#+#+#+#+   +#+           */
#    Created: 2024/05/06 10:24:11 by tchoquet          #+#    #+#             */
#                                                     ###   ########.fr       */
#                                                                             */
#  ************************************************************************** */

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

# FetchContent_MakeAvailable(googletest)
FetchContent_MakeAvailable(cpp-httplib)
FetchContent_MakeAvailable(fuzztest)

include(GoogleTest)

fuzztest_setup_fuzzing_flags()
add_executable(webserv-test webserv-tests.cpp)

target_include_directories(webserv-test PRIVATE ${cpp-httplib_SOURCE_DIR})
# target_link_libraries(webserv-test PRIVATE webserv-lib GTest::gtest_main)
target_link_libraries(webserv-test PRIVATE webserv-lib)

link_fuzztest(webserv-test)
gtest_discover_tests(webserv-test WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})