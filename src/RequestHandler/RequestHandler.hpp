/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/10 15:24:37 by tchoquet          #+#    #+#             */
/*   Updated: 2024/03/29 20:20:56 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTHANDLER_HPP
# define REQUESTHANDLER_HPP

#include "Utils/Utils.hpp"
#include "ConfigParser/ConfigParser.hpp"
#include "HTTP/HTTPRequest.hpp"
#include "Socket/ClientSocket.hpp"
#include "RequestHandler/Resource.hpp"

#include <vector>

namespace webserv
{

class RequestHandler;
typedef SharedPtr<RequestHandler> RequestHandlerPtr;

class RequestHandler
{
public:
    RequestHandler(const HTTPRequestPtr& request, const ClientSocketPtr& clientSocket);

    int processRequestLine();
    int processHeaders();

    inline void makeResponse() { internalRedirection(m_request->method, m_request->uri, m_request->query); }
    void makeErrorResponse(int code);
    void makeRedirectionResponse(int code, const std::string& location);
    void makeResponseAutoindex(const std::string& uri);
    void makeUploadResponse();

    void runTasks(const RequestHandlerPtr& _this);

    void internalRedirection(const std::string& method, const std::string& uri, const std::string& query);

    inline bool needBody() { return m_needBody; }
    inline bool shouldEndConnection() { return m_shouldEndConnection; }

private:
    HTTPRequestPtr m_request;
    ClientSocketPtr m_clientSocket;

    ServerConfig m_config;
    LocationDirective m_location;

    HTTPResponsePtr m_response;
    std::vector<ResourcePtr> m_resources;

    bool m_needBody;
    bool m_shouldEndConnection;
    uint32 m_internalRedirectionCount;
};

}

#endif // REQUESTHANDLER_HPP