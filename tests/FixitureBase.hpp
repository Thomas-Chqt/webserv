/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FixitureBase.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 01:34:14 by tchoquet          #+#    #+#             */
/*   Updated: 2024/05/08 19:34:39 by tchoquet         ###   ########.fr       */
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
    template<const char* WEBSERV_CONF, const char* NGINX_CONF, int TIMEOUT_SEC>
#else
    template<const char* WEBSERV_CONF, int TIMEOUT_SEC>
#endif // NGINX_PATH
class FixitureBase : public testing::Test
{
public:
    struct Response
    {
        bool hasResponded  = false;
        bool isComplete    = false;
        bool isBadResponse = false;
        webserv::HTTPResponse httpResponse;
        std::string lastHeaderField;
    };

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

        usleep(100000);
    }
    
    FixitureBase::Response getResponse(const std::string& request, webserv::uint16 port)
    {
        http_parser_settings parserSettings;

        parserSettings.on_message_begin    = NULL;
        parserSettings.on_url              = NULL;
        parserSettings.on_status           = [](http_parser *parser, const char *status, unsigned long len) -> int
        {
            ((Response*)parser->data)->httpResponse.statusDescription = std::string(status, status + len);
            return 0;
        };
        parserSettings.on_header_field     = [](http_parser *parser, const char *header, unsigned long len) -> int
        {
            ((Response*)parser->data)->lastHeaderField = webserv::stringToLower(std::string(header, header + len));
            return 0;
        };
        parserSettings.on_header_value     = [](http_parser *parser, const char *value, unsigned long len) -> int
        {
            ((Response*)parser->data)->httpResponse.headers[((Response*)parser->data)->lastHeaderField] = std::string(value, value + len);
            return 0;
        };
        parserSettings.on_headers_complete = NULL;
        parserSettings.on_body             = [](http_parser *parser, const char *body, unsigned long len) -> int
        {
            ((Response*)parser->data)->httpResponse.body = std::vector<webserv::Byte>(body, body + len);
            return 0;
        };
        parserSettings.on_message_complete = [](http_parser *parser) -> int
        {
            ((Response*)parser->data)->isComplete = true;
            return 0;
        };
        parserSettings.on_chunk_header     = NULL;
        parserSettings.on_chunk_complete   = NULL;

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

        std::unique_ptr<http_parser> parser = std::make_unique<http_parser>();
        FixitureBase::Response response;

        http_parser_init(parser.get(), HTTP_RESPONSE);
        parser->data = &response;

        webserv::Byte buff[BUFFER_SIZE];

        for (;;)
        {
            fd_set fdSet;
            FD_ZERO(&fdSet);
            FD_SET(sockfd, &fdSet);

            struct timeval timeVal;

            timeVal.tv_sec = TIMEOUT_SEC;
            timeVal.tv_usec = 0;

            if (::select(sockfd + 1, &fdSet, NULL, NULL, &timeVal) < 0)
            {
                ::close(sockfd);
                throw std::runtime_error("select(): " + std::string(std::strerror(errno)));
            }

            if (FD_ISSET(sockfd, &fdSet) == false)
                break;
                
            ssize_t recvLen = ::recv(sockfd, buff, BUFFER_SIZE, 0);
            
            if (recvLen < 0)
            {
                ::close(sockfd);
                throw std::runtime_error("recv(): " + std::string(std::strerror(errno)));
            }
            response.hasResponded = true;
            size_t nparsed = http_parser_execute(parser.get(), &parserSettings, (const char *)buff, (size_t)recvLen);
            if (nparsed != (size_t)recvLen)
            {
                response.isBadResponse = true;
                break;
            }

            if (recvLen == 0 || ((FixitureBase::Response*)parser->data)->isComplete)
                break;
        }

        ::close(sockfd);
        return response;
    }

    ~FixitureBase()
    {
        EXPECT_EQ(::waitpid(m_webservPID, NULL, WNOHANG), 0) << "Webserv is stop";
        ::kill(m_webservPID, SIGKILL);
        ::waitpid(m_webservPID, NULL, 0);

        #ifdef NGINX_PATH
            ::kill(m_nginxPID, SIGQUIT);
            ::waitpid(m_nginxPID, NULL, 0);
        #endif // NGINX_PATH

        webserv::IOManager::terminate();
        webserv::Logger::terminate();

        int pid = getpid();

        std::string cmd = \
        "echo \"$(lsof -i -P -n | grep LISTEN | grep 8080 | while read -r first second rest; do echo $second; done ; "\
        "ps -A | grep webserv | grep -v grep | while read -r first second rest; do echo $first; done ; "\
        "ps -A | grep nginx | grep -v grep | while read -r first second rest; do echo $first; done)\" | "\
        "grep -v " + std::to_string(pid) + " | "\
        "sort | uniq | xargs kill";

        std::system(cmd.c_str());
    }

private:
    int m_webservPID;
    #ifdef NGINX_PATH
        int m_nginxPID;
    #endif // NGINX_PATH
};

#endif // FIXITUREBASE_HPP