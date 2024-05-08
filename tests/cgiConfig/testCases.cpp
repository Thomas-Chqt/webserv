/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   testCases.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 12:27:20 by tchoquet          #+#    #+#             */
/*   Updated: 2024/05/08 18:45:57 by tchoquet         ###   ########.fr       */
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

    cgiConfig::Response webservResponse = getResponse(request, 8080);

    #ifdef NGINX_PATH
        cgiConfig::Response nginxResponse = getResponse(request, 8090);
    #endif // NGINX_PATH

    #ifdef NGINX_PATH
        ASSERT_EQ(webservResponse.hasResponded,  nginxResponse.hasResponded);
        ASSERT_EQ(webservResponse.isBadResponse, nginxResponse.isBadResponse);

        ASSERT_EQ(webservResponse.httpResponse.statusDescription,         nginxResponse.httpResponse.statusDescription);
        ASSERT_EQ(webservResponse.httpResponse.headers["content-type"],   nginxResponse.httpResponse.headers["content-type"]);
        ASSERT_EQ(webservResponse.httpResponse.headers["content-length"], nginxResponse.httpResponse.headers["content-length"]);
        
        ASSERT_TRUE(webservResponse.httpResponse.body == nginxResponse.httpResponse.body) << "bodies are not equal";
    #else
        ASSERT_TRUE(webservResponse.hasResponded);
        ASSERT_FALSE(webservResponse.isBadResponse);

        ASSERT_EQ(webservResponse.httpResponse.statusDescription, "OK");
    #endif // NGINX_PATH
}

TEST_F(cgiConfig, noResponse)
{
    const std::string request = \
    "GET /no_response.py HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "\r\n";

    cgiConfig::Response webservResponse = getResponse(request, 8080);

    #ifdef NGINX_PATH
        cgiConfig::Response nginxResponse = getResponse(request, 8090);
    #endif // NGINX_PATH

    #ifdef NGINX_PATH
        ASSERT_EQ(webservResponse.hasResponded,  nginxResponse.hasResponded);
        ASSERT_EQ(webservResponse.isBadResponse, nginxResponse.isBadResponse);

        ASSERT_EQ(webservResponse.httpResponse.statusDescription,         nginxResponse.httpResponse.statusDescription);
        ASSERT_EQ(webservResponse.httpResponse.headers["content-type"],   nginxResponse.httpResponse.headers["content-type"]);
        // ASSERT_EQ(webservResponse.httpResponse.headers["content-length"], nginxResponse.httpResponse.headers["content-length"]);
        
        // ASSERT_TRUE(webservResponse.httpResponse.body == nginxResponse.httpResponse.body) << "bodies are not equal";
    #else
        ASSERT_TRUE(webservResponse.hasResponded);
        ASSERT_FALSE(webservResponse.isBadResponse);

        ASSERT_EQ(webservResponse.httpResponse.statusDescription, "Gateway Time-out");
    #endif // NGINX_PATH
}