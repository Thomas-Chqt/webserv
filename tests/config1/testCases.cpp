/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   testCases.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 01:55:03 by tchoquet          #+#    #+#             */
/*   Updated: 2024/05/08 01:10:01 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FixitureBase.hpp"
#include "http_parser.h"

constexpr char webservConfig1Path[] = "tests/config1/webserv.conf";

#ifdef NGINX_PATH
    constexpr char nginxConfig1Path[] = "tests/config1/nginx.conf";
    class Config1 : public FixitureBase<webservConfig1Path, nginxConfig1Path> { };
#else
    class Config1 : public FixitureBase<webservConfig1Path> { };
#endif // NGINX_PATH

TEST_F(Config1, simpleGET)
{
    const std::string request = \
    "GET / HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "\r\n";

    Config1::response webservResponse = getResponse(request, 8080);

    #ifdef NGINX_PATH
        Config1::response nginxResponse = getResponse(request, 8090);
    #endif // NGINX_PATH

    #ifdef NGINX_PATH
        ASSERT_EQ(webservResponse.error,                              nginxResponse.error);
        ASSERT_EQ(webservResponse.hasResponded,                       nginxResponse.hasResponded);
        ASSERT_EQ(webservResponse.isResponseComplete,                 nginxResponse.isResponseComplete);

        EXPECT_EQ(webservResponse.response.statusDescription,         nginxResponse.response.statusDescription);
        EXPECT_EQ(webservResponse.response.headers["content-length"], nginxResponse.response.headers["content-length"]);
        EXPECT_EQ(webservResponse.response.headers["content-type"],   nginxResponse.response.headers["content-type"]);
        EXPECT_TRUE(webservResponse.response.body          ==         nginxResponse.response.body) << "bodies are not equal";
    #else
        EXPECT_FALSE(webservResponse.error);
        EXPECT_EQ(webservResponse.response.statusDescription, "OK");
    #endif // NGINX_PATH
}

TEST_F(Config1, lowerCaseMethod)
{
    const std::string request = \
    "get / HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "\r\n";

    Config1::response webservResponse = getResponse(request, 8080);

    #ifdef NGINX_PATH
        Config1::response nginxResponse = getResponse(request, 8090);
    #endif // NGINX_PATH

    #ifdef NGINX_PATH
        ASSERT_EQ(webservResponse.error,                              nginxResponse.error);
        ASSERT_EQ(webservResponse.hasResponded,                       nginxResponse.hasResponded);
        ASSERT_EQ(webservResponse.isResponseComplete,                 nginxResponse.isResponseComplete);

        EXPECT_EQ(webservResponse.response.statusDescription,         nginxResponse.response.statusDescription);
        EXPECT_EQ(webservResponse.response.headers["content-length"], nginxResponse.response.headers["content-length"]);
        EXPECT_EQ(webservResponse.response.headers["content-type"],   nginxResponse.response.headers["content-type"]);
        EXPECT_TRUE(webservResponse.response.body          ==         nginxResponse.response.body) << "bodies are not equal";
    #else
        EXPECT_FALSE(webservResponse.first);
        EXPECT_EQ(webservResponse.second.statusDescription, "Bad Request");
    #endif // NGINX_PATH
}

TEST_F(Config1, badRequest)
{
    const std::string request = "BAD REQUEST";

    Config1::response webservResponse = getResponse(request, 8080);

    #ifdef NGINX_PATH
        Config1::response nginxResponse = getResponse(request, 8090);
    #endif // NGINX_PATH

    #ifdef NGINX_PATH
        ASSERT_EQ(webservResponse.error,                              nginxResponse.error);
        ASSERT_EQ(webservResponse.hasResponded,                       nginxResponse.hasResponded);
        ASSERT_EQ(webservResponse.isResponseComplete,                 nginxResponse.isResponseComplete);

        EXPECT_EQ(webservResponse.response.statusDescription,         nginxResponse.response.statusDescription);
        EXPECT_EQ(webservResponse.response.headers["content-length"], nginxResponse.response.headers["content-length"]);
        EXPECT_EQ(webservResponse.response.headers["content-type"],   nginxResponse.response.headers["content-type"]);
        EXPECT_TRUE(webservResponse.response.body          ==         nginxResponse.response.body) << "bodies are not equal";
    #else
        EXPECT_FALSE(webservResponse.error);
        EXPECT_EQ(webservResponse.second.statusDescription, "Bad Request");
    #endif // NGINX_PATH
}