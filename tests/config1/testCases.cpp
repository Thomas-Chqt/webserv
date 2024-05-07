/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   testCases.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 01:55:03 by tchoquet          #+#    #+#             */
/*   Updated: 2024/05/07 15:21:55 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FixitureBase.hpp"

constexpr char Config1Path[] = "tests/config1/webserv.conf";

class Config1 : public FixitureBase<Config1Path> { };

TEST_F(Config1, simpleGET)
{
    const std::string request = \
    "GET / HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "\r\n";

    std::string webservResponse = getWebservResponse(request, 8080);
    #ifdef NGINX_PATH
        std::string nginxResponse = getWebservResponse(request, 8090);
    #endif // NGINX_PATH

    EXPECT_FALSE(webservResponse.empty());
}