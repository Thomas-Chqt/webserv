/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   testCases.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 01:55:03 by tchoquet          #+#    #+#             */
/*   Updated: 2024/05/08 19:21:59 by tchoquet         ###   ########.fr       */
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

    Config1::Response webservResponse = getResponse(request, 8080);

    #ifdef NGINX_PATH
        Config1::Response nginxResponse = getResponse(request, 8090);
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

TEST_F(Config1, lowerCaseMethod)
{
    const std::string request = \
    "get / HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "\r\n";

    Config1::Response webservResponse = getResponse(request, 8080);

    #ifdef NGINX_PATH
        Config1::Response nginxResponse = getResponse(request, 8090);
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

        ASSERT_EQ(webservResponse.httpResponse.statusDescription, "Bad Request");
    #endif // NGINX_PATH
}

TEST_F(Config1, badRequest)
{
    const std::string request = "BAD REQUEST\r\n";

    Config1::Response webservResponse = getResponse(request, 8080);

    #ifdef NGINX_PATH
        Config1::Response nginxResponse = getResponse(request, 8090);
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

        ASSERT_EQ(webservResponse.httpResponse.statusDescription, "Bad Request");
    #endif // NGINX_PATH
}

TEST_F(Config1, badRequest2)
{
    const std::string request = "BAD REQUEST";

    Config1::Response webservResponse = getResponse(request, 8080);

    #ifdef NGINX_PATH
        Config1::Response nginxResponse = getResponse(request, 8090);
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

        ASSERT_EQ(webservResponse.httpResponse.statusDescription, "Bad Request");
    #endif // NGINX_PATH
}

TEST_F(Config1, badRequest3)
{
    const std::string request = "BAD REQUEST LOLOL";

    Config1::Response webservResponse = getResponse(request, 8080);

    #ifdef NGINX_PATH
        Config1::Response nginxResponse = getResponse(request, 8090);
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

        ASSERT_EQ(webservResponse.httpResponse.statusDescription, "Bad Request");
    #endif // NGINX_PATH
}

TEST_F(Config1, badRequest4)
{
    const std::string request = "BAD REQUEST HTTP/1.1";

    Config1::Response webservResponse = getResponse(request, 8080);

    #ifdef NGINX_PATH
        Config1::Response nginxResponse = getResponse(request, 8090);
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

        ASSERT_EQ(webservResponse.httpResponse.statusDescription, "Bad Request");
    #endif // NGINX_PATH
}

TEST_F(Config1, badRequest5)
{
    const std::string request = "BAD /REQUEST HTTP/1.1";

    Config1::Response webservResponse = getResponse(request, 8080);

    #ifdef NGINX_PATH
        Config1::Response nginxResponse = getResponse(request, 8090);
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

        ASSERT_EQ(webservResponse.httpResponse.statusDescription, "Bad Request");
    #endif // NGINX_PATH
}

TEST_F(Config1, badRequest6)
{
    const std::string request = "BaD /REQUEST HTTP/1.1";

    Config1::Response webservResponse = getResponse(request, 8080);

    #ifdef NGINX_PATH
        Config1::Response nginxResponse = getResponse(request, 8090);
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

        ASSERT_EQ(webservResponse.httpResponse.statusDescription, "Bad Request");
    #endif // NGINX_PATH
}

TEST_F(Config1, badRequest7)
{
    const std::string request = "g";

    Config1::Response webservResponse = getResponse(request, 8080);

    #ifdef NGINX_PATH
        Config1::Response nginxResponse = getResponse(request, 8090);
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

        ASSERT_EQ(webservResponse.httpResponse.statusDescription, "Bad Request");
    #endif // NGINX_PATH
}

TEST_F(Config1, badRequest8)
{
    const std::string request = "BAD REQUEST ";

    Config1::Response webservResponse = getResponse(request, 8080);

    #ifdef NGINX_PATH
        Config1::Response nginxResponse = getResponse(request, 8090);
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

        ASSERT_EQ(webservResponse.httpResponse.statusDescription, "Bad Request");
    #endif // NGINX_PATH
}

TEST_F(Config1, uriTooLong)
{
    const std::string request = \
    "GET /tyawedwed/qwefeqrfew/gewg/wrtg/rwt/grw/tg/rwtg/rwtg/rtg/r/tg/rwt/gwr/tgwrt/gw/rtg/rwt/grw/tg/rw/hteyj/yuj/ut/kuyik//e/dwq/edqw/ed/wqf/qer/fe/wrg/rt/hg/teyrh/tryjhrt/yj HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "\r\n";

    Config1::Response webservResponse = getResponse(request, 8080);

    #ifdef NGINX_PATH
        Config1::Response nginxResponse = getResponse(request, 8090);
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

        ASSERT_EQ(webservResponse.httpResponse.statusDescription, "Bad Request");
    #endif // NGINX_PATH
}

TEST_F(Config1, badMethodNumber)
{
    const std::string request = \
    "GET7 / HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "\r\n";

    Config1::Response webservResponse = getResponse(request, 8080);

    #ifdef NGINX_PATH
        Config1::Response nginxResponse = getResponse(request, 8090);
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

        ASSERT_EQ(webservResponse.httpResponse.statusDescription, "Bad Request");
    #endif // NGINX_PATH
}

TEST_F(Config1, badMethodAstrisk)
{
    const std::string request = \
    "* / HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "\r\n";

    Config1::Response webservResponse = getResponse(request, 8080);

    #ifdef NGINX_PATH
        Config1::Response nginxResponse = getResponse(request, 8090);
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

        ASSERT_EQ(webservResponse.httpResponse.statusDescription, "Bad Request");
    #endif // NGINX_PATH
}

TEST_F(Config1, badMethodJapan)
{
    const std::string request = \
    "は / HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "\r\n";

    Config1::Response webservResponse = getResponse(request, 8080);

    #ifdef NGINX_PATH
        Config1::Response nginxResponse = getResponse(request, 8090);
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

        ASSERT_EQ(webservResponse.httpResponse.statusDescription, "Bad Request");
    #endif // NGINX_PATH
}

TEST_F(Config1, badURI)
{
    const std::string request = \
    "GET /* HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "\r\n";

    Config1::Response webservResponse = getResponse(request, 8080);

    #ifdef NGINX_PATH
        Config1::Response nginxResponse = getResponse(request, 8090);
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

        ASSERT_EQ(webservResponse.httpResponse.statusDescription, "Bad Request");
    #endif // NGINX_PATH
}

TEST_F(Config1, badURI2)
{
    const std::string request = \
    "GET /% HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "\r\n";

    Config1::Response webservResponse = getResponse(request, 8080);

    #ifdef NGINX_PATH
        Config1::Response nginxResponse = getResponse(request, 8090);
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

        ASSERT_EQ(webservResponse.httpResponse.statusDescription, "Bad Request");
    #endif // NGINX_PATH
}

TEST_F(Config1, badURIJapan)
{
    const std::string request = \
    "GET /こんにちは HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "\r\n";

    Config1::Response webservResponse = getResponse(request, 8080);

    #ifdef NGINX_PATH
        Config1::Response nginxResponse = getResponse(request, 8090);
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

        ASSERT_EQ(webservResponse.httpResponse.statusDescription, "Bad Request");
    #endif // NGINX_PATH
}

