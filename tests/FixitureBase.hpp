/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FixitureBase.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 01:34:14 by tchoquet          #+#    #+#             */
/*   Updated: 2024/05/07 19:11:41 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FIXITUREBASE_HPP
# define FIXITUREBASE_HPP

#include <gtest/gtest.h>
#include <arpa/inet.h>
#include <csignal>
#include <sys/param.h>

#include "IO/IOManager.hpp"
#include "IO/IOTask/ReadTask/ClientSocketReadTask.hpp"
#include "http_parser.h"

#ifdef NGINX_PATH
    template<const char* WEBSERV_CONF, const char* NGINX_CONF>
#else
    template<const char* WEBSERV_CONF>
#endif // NGINX_PATH
class FixitureBase : public testing::Test
{
public:
    FixitureBase()
    {
        webserv::IOManager::init();

        const char* argv[] = { "webserv", WEBSERV_CONF };
        std::vector<webserv::ServerConfig> configs = webserv::parseServerConfig(2, argv);
        webserv::Logger::init(configs[0].error_log);
        webserv::IOManager::shared().loadConfigs(configs);

        m_webservPID = ::fork();
        if (m_webservPID < 0)
            throw std::runtime_error("fork(): " + std::string(std::strerror(errno)));
        if (m_webservPID == 0)
        {
            for(;;)
            {
                std::set<webserv::MasterSocketPtr> masterSockets;
                std::set<webserv::IReadTaskPtr> readTasks;
                std::set<webserv::IWriteTaskPtr> writeTasks;

                webserv::IOManager::shared().selectIOs(masterSockets, readTasks, writeTasks);

                for (std::set<webserv::MasterSocketPtr>::iterator curr = masterSockets.begin(); curr != masterSockets.end(); ++curr)
                {
                    if (webserv::ClientSocketPtr clientSocket = (*curr)->acceptNewClient(*curr))
                        webserv::IOManager::shared().insertReadTask(new webserv::ClientSocketReadTask(clientSocket));
                }

                for (std::set<webserv::IReadTaskPtr>::iterator curr = readTasks.begin(); curr != readTasks.end(); ++curr)
                    (*curr)->read();

                for (std::set<webserv::IWriteTaskPtr>::iterator curr = writeTasks.begin(); curr != writeTasks.end(); ++curr)
                    (*curr)->write();
            }
        }

        #ifdef NGINX_PATH
            m_nginxPID = ::fork();
            if (m_nginxPID < 0)
                throw std::runtime_error("fork(): " + std::string(std::strerror(errno)));
            if (m_nginxPID == 0)
            {
                char cwd[MAXPATHLEN];
                const char* argv[] = { "nginx", "-p", getwd(cwd), "-c", NGINX_CONF, NULL };
                const char* envp[] = { NULL };
                ::execve(NGINX_PATH, (char *const *)argv, (char *const *)envp);
            }
        #endif // NGINX_PATH
    }
    
    std::pair<bool, webserv::HTTPResponse> getResponse(const std::string& request, webserv::uint16 port)
    {
        webserv::HTTPResponse parsedResponse;

        http_parser_settings parserSettings;

        parserSettings.on_message_begin    = NULL;
        parserSettings.on_url              = NULL;
        parserSettings.on_status           = [](http_parser *parser, const char *status, unsigned long len) -> int
        {
            ((webserv::HTTPResponse*)parser->data)->statusDescription = std::string(status, status + len);
            return 0;
        };
        parserSettings.on_header_field     = [](http_parser *parser, const char *header, unsigned long len) -> int
        {
            ((webserv::HTTPResponse*)parser->data)->lastHeaderField = std::string(header, header + len);
            return 0;
        };
        parserSettings.on_header_value     = [](http_parser *parser, const char *value, unsigned long len) -> int
        {
            ((webserv::HTTPResponse*)parser->data)->headers[((webserv::HTTPResponse*)parser->data)->lastHeaderField] = std::string(value, value + len);
            return 0;
        };
        parserSettings.on_headers_complete = NULL;
        parserSettings.on_body             = [](http_parser *parser, const char *body, unsigned long len) -> int
        {
            ((webserv::HTTPResponse*)parser->data)->body = std::vector<webserv::Byte>(body, body + len);
            return 0;
        };
        parserSettings.on_message_complete = NULL;
        parserSettings.on_chunk_header     = NULL;
        parserSettings.on_chunk_complete   = NULL;

        std::unique_ptr<http_parser> parser = std::make_unique<http_parser>();
        http_parser_init(parser.get(), HTTP_RESPONSE);

        parser->data = &parsedResponse;
    
        int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);

        struct sockaddr_in server_addr = {
            .sin_family = AF_INET,
            .sin_port = htons(port)
        };

        ::inet_pton(AF_INET, "127.0.0.1", &(server_addr.sin_addr));

        if(::connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0)
        {
            ::close(sockfd);
            throw std::runtime_error("connect(): " + std::string(std::strerror(errno)));
        }

        if(::send(sockfd, request.c_str(), request.length(), 0) < 0)
        {
            ::close(sockfd);
            throw std::runtime_error("send(): " + std::string(std::strerror(errno)));
        }

        std::string responseStr;
        webserv::Byte buff[BUFFER_SIZE];

        ssize_t recvLen;
        do
        {
            recvLen = ::recv(sockfd, buff, BUFFER_SIZE, 0);
            
            if (recvLen < 0)
            {
                ::close(sockfd);
                throw std::runtime_error("recv(): " + std::string(std::strerror(errno)));
            }

            size_t nparsed = http_parser_execute(parser.get(), &parserSettings, (const char *)buff, (size_t)recvLen);
            if (nparsed != (size_t)recvLen)
            {
                ::close(sockfd);
                return std::make_pair(false, parsedResponse);
            }

            if (recvLen == 0)
                break;

            responseStr.append(buff, buff + recvLen);
        }
        while(recvLen == BUFFER_SIZE);

        ::close(sockfd);

        return std::make_pair(true, parsedResponse);
    }

    ~FixitureBase()
    {
        EXPECT_EQ(::waitpid(m_webservPID, NULL, WNOHANG), 0) << "Webserv is stop";
        ::kill(m_webservPID, SIGKILL);
        ::waitpid(m_webservPID, NULL, 0);

        #ifdef NGINX_PATH
            ::kill(m_nginxPID, SIGQUIT);
        #endif // NGINX_PATH

        webserv::IOManager::terminate();
        webserv::Logger::terminate();
    }

private:
    int m_webservPID;
    #ifdef NGINX_PATH
        int m_nginxPID;
    #endif // NGINX_PATH
};

#endif // FIXITUREBASE_HPP