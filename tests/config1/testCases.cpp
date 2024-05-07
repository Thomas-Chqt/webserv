/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   testCases.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 01:55:03 by tchoquet          #+#    #+#             */
/*   Updated: 2024/05/07 15:40:50 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FixitureBase.hpp"

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

    std::string webservResponse = getWebservResponse(request, 8080);
    #ifdef NGINX_PATH
        std::string nginxResponse = getNginxResponse(request, 8090);
    #endif // NGINX_PATH

    EXPECT_FALSE(webservResponse.empty());
}