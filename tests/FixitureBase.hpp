/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FixitureBase.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 01:34:14 by tchoquet          #+#    #+#             */
/*   Updated: 2024/05/07 02:50:55 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FIXITUREBASE_HPP
# define FIXITUREBASE_HPP

#include <gtest/gtest.h>
#include <arpa/inet.h>

#include "IO/IOManager.hpp"
#include "IO/IOTask/ReadTask/ClientSocketReadTask.hpp"

template<const char* CONF>
class FixitureBase : public testing::Test
{
public:
    FixitureBase()
    {
        webserv::IOManager::init();

        const char* argv[] = { "webserv", CONF };
        std::vector<webserv::ServerConfig> configs = webserv::parseServerConfig(2, argv);

        webserv::Logger::init(configs[0].error_log);
        webserv::IOManager::shared().loadConfigs(configs);
    }
    
    std::string getResponse(const std::string& request)
    {
        int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);

        struct sockaddr_in server_addr = {
            .sin_family = AF_INET,
            .sin_port = htons(8080)
        };

        ::inet_pton(AF_INET, "127.0.0.1", &(server_addr.sin_addr));

        if(::connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0)
            throw std::runtime_error("connect(): " + std::string(std::strerror(errno)));

        if(::send(sockfd, request.c_str(), request.length(), 0) < 0)
            throw std::runtime_error("send(): " + std::string(std::strerror(errno)));

        do
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
        while (webserv::IOManager::shared().hasPendingTask());

        std::string response;
        webserv::Byte buff[BUFFER_SIZE];

        ssize_t recvLen;
        do
        {
            recvLen = ::recv(sockfd, buff, BUFFER_SIZE, 0);
            if (recvLen < 0)
                throw std::runtime_error("recv(): " + std::string(std::strerror(errno)));

            response.append(buff, buff + recvLen);
        }
        while (recvLen == BUFFER_SIZE);

        ::close(sockfd);

        return response;
    }

    ~FixitureBase()
    {
        webserv::IOManager::terminate();
        webserv::Logger::terminate();
    }
};

#endif // FIXITUREBASE_HPP