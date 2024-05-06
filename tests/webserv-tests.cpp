/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv-tests.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 10:44:59 by tchoquet          #+#    #+#             */
/*   Updated: 2024/05/06 19:52:39 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include <cstring>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <signal.h>
#include "fuzztest/fuzztest.h"

#include "Parser/HTTPRequestParser/HTTPRequestParser.hpp"
#include "Parser/ConfigParser/ConfigParser.hpp"
#include "IO/IOManager.hpp"
#include "Socket/ClientSocket.hpp"
#include "IO/IOTask/ReadTask/ClientSocketReadTask.hpp"

// #include "httplib.h"

// void runServer(std::vector<webserv::uint16>& ports, int& pid)
// {
//     webserv::IOManager::init();
// 
//     const char* argv[] = {"webserv", "configs/tests-local.conf"};
//     std::vector<webserv::ServerConfig> configs = webserv::parseServerConfig(2, argv);
//     webserv::Logger::init(configs[0].error_log);
//     webserv::IOManager::shared().loadConfigs(configs);
// 
//     for (std::vector<webserv::ServerConfig>::iterator it = configs.begin(); it != configs.end(); ++it)
//     {
//         for (std::vector<webserv::uint16>::iterator it2 = it->listens.begin(); it2 != it->listens.end(); ++it2)
//             ports.push_back(*it2);
//     }
// 
//     pid = ::fork();
//     if (pid < 0)
//         std::exit(1);
// 
//     if (pid == 0)
//     {
//         while (true)
//         {
//             std::set<webserv::MasterSocketPtr> masterSockets;
//             std::set<webserv::IReadTaskPtr> readTasks;
//             std::set<webserv::IWriteTaskPtr> writeTasks;
// 
//             webserv::IOManager::shared().selectIOs(masterSockets, readTasks, writeTasks);
// 
//             for (std::set<webserv::MasterSocketPtr>::iterator curr = masterSockets.begin(); curr != masterSockets.end(); ++curr)
//             {
//                 if (webserv::ClientSocketPtr clientSocket = (*curr)->acceptNewClient(*curr))
//                     webserv::IOManager::shared().insertReadTask(new webserv::ClientSocketReadTask(clientSocket));
//             }
// 
//             for (std::set<webserv::IReadTaskPtr>::iterator curr = readTasks.begin(); curr != readTasks.end(); ++curr)
//                 (*curr)->read();
// 
//             for (std::set<webserv::IWriteTaskPtr>::iterator curr = writeTasks.begin(); curr != writeTasks.end(); ++curr)
//                 (*curr)->write();
//         }
//     }
// }
// 
// void stopServer(int pid)
// {
//     ::kill(pid, SIGKILL);
// 
//     webserv::IOManager::terminate();
//     webserv::Logger::terminate();
// }

// class WebservTest : public testing::Test
// {
// protected:
//     WebservTest()
//     {
//         runServer(ports, pid);
//     }
// 
//     int pid;
//     std::vector<webserv::uint16> ports;
// 
//     ~WebservTest()
//     {
//         stopServer(pid);
//     }
    // 
// };

// TEST_F(WebservTest, simpleGET)
// {
//     httplib::Client cli("http://localhost:" + webserv::to_string(ports[0]));
// 
//     auto res = cli.Get("/");
//     ASSERT_EQ(res->status, 200);
// }

class WebservFuzzTest
{
public:
    WebservFuzzTest()
    {
        webserv::IOManager::init();

        const char* argv[] = {"webserv", "configs/tests-local.conf"};
        std::vector<webserv::ServerConfig> configs = webserv::parseServerConfig(2, argv);
        webserv::Logger::init(configs[0].error_log);
        webserv::IOManager::shared().loadConfigs(configs);

        for (std::vector<webserv::ServerConfig>::iterator it = configs.begin(); it != configs.end(); ++it)
        {
            for (std::vector<webserv::uint16>::iterator it2 = it->listens.begin(); it2 != it->listens.end(); ++it2)
                ports.push_back(*it2);
        }
    }

    void runLoop()
    {
        while (webserv::IOManager::shared().hasPendingTask())
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

    void webservNoCrash(std::string request)
    {
        int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
        
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(8080);
        
        ::inet_pton(AF_INET, "127.0.0.1", &(server_addr.sin_addr));
        if(::connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0)
            ;
        ::send(sockfd, request.c_str(), request.length(), 0);
        ::close(sockfd);

        runLoop();
    }

    std::vector<webserv::uint16> ports;

    ~WebservFuzzTest()
    {
        webserv::IOManager::terminate();
        webserv::Logger::terminate();
    }
};

FUZZ_TEST_F(WebservFuzzTest, webservNoCrash)
    .WithDomains(fuzztest::PrintableAsciiString())
    .WithSeeds({"GET", "POST", "GET /"});


void parserNoCrash(std::string requestStr)
{
    std::cout << requestStr << std::endl;
    
    webserv::HTTPRequestPtr request = new webserv::HTTPRequest();
    webserv::HTTPRequestParser parser(request);

    webserv::Byte* buff = parser.getBuffer();

    std::memcpy(buff, requestStr.c_str(), requestStr.size() < BUFFER_SIZE ? requestStr.size() : BUFFER_SIZE);

    parser.parse(requestStr.size() < BUFFER_SIZE ? requestStr.size() : BUFFER_SIZE);
}

FUZZ_TEST(WebservFuzz, parserNoCrash)
    .WithDomains(fuzztest::PrintableAsciiString());