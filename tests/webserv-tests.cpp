/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv-tests.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 10:44:59 by tchoquet          #+#    #+#             */
/*   Updated: 2024/05/06 12:45:38 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include <cstring>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <signal.h>

#include "Parser/HTTPRequestParser/HTTPRequestParser.hpp"
#include "Parser/ConfigParser/ConfigParser.hpp"
#include "IO/IOManager.hpp"
#include "Socket/ClientSocket.hpp"
#include "IO/IOTask/ReadTask/ClientSocketReadTask.hpp"

#include "httplib.h"

class WebservTest : public testing::Test
{
protected:
    WebservTest()
    {
        webserv::IOManager::init();

        const char* argv[] = {"webserv", "configs/tests.conf"};
        std::vector<webserv::ServerConfig> configs = webserv::parseServerConfig(2, argv);
        webserv::Logger::init(configs[0].error_log);
        webserv::IOManager::shared().loadConfigs(configs);

        for (std::vector<webserv::ServerConfig>::iterator it = configs.begin(); it != configs.end(); ++it)
        {
            for (std::vector<webserv::uint16>::iterator it2 = it->listens.begin(); it2 != it->listens.end(); ++it2)
                ports.push_back(*it2);
        }

        pid = ::fork();
        if (pid < 0)
            std::exit(1);

        if (pid == 0)
        {
            while (true)
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
    }

    int pid;
    std::vector<webserv::uint16> ports;

    ~WebservTest()
    {
        ::kill(pid, SIGKILL);
    }
    
};

TEST_F(WebservTest, simpleGET)
{
    httplib::Client cli("http://localhost:" + webserv::to_string(ports[0]));

    auto res = cli.Get("/");
    ASSERT_EQ(res->status, 200);
}
