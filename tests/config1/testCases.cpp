/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   testCases.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 01:55:03 by tchoquet          #+#    #+#             */
/*   Updated: 2024/05/07 20:41:21 by tchoquet         ###   ########.fr       */
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

    std::pair<bool, webserv::HTTPResponse> webservResponse = getResponse(request, 8080);

    #ifdef NGINX_PATH
        std::pair<bool, webserv::HTTPResponse> nginxResponse = getResponse(request, 8090);
    #endif // NGINX_PATH

    #ifdef NGINX_PATH
        EXPECT_EQ(webservResponse.first,                            nginxResponse.first);
        EXPECT_EQ(webservResponse.second.statusDescription,         nginxResponse.second.statusDescription);
        EXPECT_EQ(webservResponse.second.headers["content-length"], nginxResponse.second.headers["content-length"]);
        EXPECT_EQ(webservResponse.second.body,                      nginxResponse.second.body);
    #else
        EXPECT_TRUE(webservResponse.first);
        EXPECT_EQ(webservResponse.second.statusDescription,         "OK");
        EXPECT_EQ(webservResponse.second.headers["content-length"], "5");
    #endif // NGINX_PATH
}

TEST_F(Config1, lowerCaseMethod)
{
    const std::string request = \
    "get / HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "\r\n";

    std::pair<bool, webserv::HTTPResponse> webservResponse = getResponse(request, 8080);

    #ifdef NGINX_PATH
        std::pair<bool, webserv::HTTPResponse> nginxResponse = getResponse(request, 8090);
    #endif // NGINX_PATH

    #ifdef NGINX_PATH
        EXPECT_EQ(webservResponse.first, nginxResponse.first);
        EXPECT_EQ(webservResponse.second.statusDescription, nginxResponse.second.statusDescription);
    #else
        EXPECT_TRUE(webservResponse.first);
        EXPECT_EQ(webservResponse.second.statusDescription, "Bad Request");
    #endif // NGINX_PATH
}

TEST_F(Config1, badRequest)
{
    const std::string request = "BAD REQUEST";

    std::pair<bool, webserv::HTTPResponse> webservResponse = getResponse(request, 8080);

    #ifdef NGINX_PATH
        std::pair<bool, webserv::HTTPResponse> nginxResponse = getResponse(request, 8090);
    #endif // NGINX_PATH

    #ifdef NGINX_PATH
        EXPECT_EQ(webservResponse.first, nginxResponse.first);
        EXPECT_EQ(webservResponse.second.statusDescription, nginxResponse.second.statusDescription);
    #else
        EXPECT_TRUE(webservResponse.first);
        EXPECT_EQ(webservResponse.second.statusDescription, "Bad Request");
    #endif // NGINX_PATH
}