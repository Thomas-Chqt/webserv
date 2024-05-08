/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   testCases.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 12:27:20 by tchoquet          #+#    #+#             */
/*   Updated: 2024/05/08 15:10:02 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FixitureBase.hpp"

constexpr char webservConfigPath[] = "tests/cgiConfig/webserv-local.conf";

#ifdef NGINX_PATH
    constexpr char nginxConfigPath[] = "tests/cgiConfig/nginx.conf";
    class cgiConfig : public FixitureBase<webservConfigPath, nginxConfigPath, 3> { };
#else
    class cgiConfig : public FixitureBase<webservConfigPath, 3> { };
#endif // NGINX_PATH

TEST_F(cgiConfig, cgiTester)
{
    const std::string request = \
    "GET /cgi_tester.py HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "\r\n";

    cgiConfig::response webservResponse = getResponse(request, 8080);

    #ifdef NGINX_PATH
        cgiConfig::response nginxResponse = getResponse(request, 8090);
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

TEST_F(cgiConfig, noResponse)
{
    const std::string request = \
    "GET /no_response.py HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "\r\n";

    cgiConfig::response webservResponse = getResponse(request, 8080);

    #ifdef NGINX_PATH
        cgiConfig::response nginxResponse = getResponse(request, 8090);
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
        EXPECT_EQ(webservResponse.response.statusDescription, "Gateway Timeout");
    #endif // NGINX_PATH
}