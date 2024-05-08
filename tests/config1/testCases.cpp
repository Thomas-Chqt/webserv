/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   testCases.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 01:55:03 by tchoquet          #+#    #+#             */
/*   Updated: 2024/05/08 14:23:21 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FixitureBase.hpp"

constexpr char webservConfigPath[] = "tests/config1/webserv-local.conf";

#ifdef NGINX_PATH
    constexpr char nginxConfigPath[] = "tests/config1/nginx.conf";
    class Config1 : public FixitureBase<webservConfigPath, nginxConfigPath, 3> { };
#else
    class Config1 : public FixitureBase<webservConfigPath, 3> { };
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
        ASSERT_EQ(webservResponse.error,                      false);
        ASSERT_EQ(webservResponse.hasResponded,               true);
        ASSERT_EQ(webservResponse.isResponseComplete,         true);
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
        ASSERT_EQ(webservResponse.error,                      false);
        ASSERT_EQ(webservResponse.hasResponded,               true);
        ASSERT_EQ(webservResponse.isResponseComplete,         true);
        EXPECT_EQ(webservResponse.response.statusDescription, "Bad Request");
    #endif // NGINX_PATH
}

TEST_F(Config1, badRequest)
{
    const std::string request = "BAD REQUEST\r\n";

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
        ASSERT_EQ(webservResponse.error,                      false);
        ASSERT_EQ(webservResponse.hasResponded,               true);
        ASSERT_EQ(webservResponse.isResponseComplete,         true);
        EXPECT_EQ(webservResponse.response.statusDescription, "Bad Request");
    #endif // NGINX_PATH
}