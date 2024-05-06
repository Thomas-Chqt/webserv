/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NoSuchFileResource.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/21 16:58:06 by tchoquet          #+#    #+#             */
/*   Updated: 2024/05/06 12:23:31 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler/Resource/NoSuchFileResource.hpp"

#include "Utils/Logger.hpp"

namespace webserv
{

NoSuchFileResource::NoSuchFileResource(const std::string& path) : Resource(path)
{

}

int NoSuchFileResource::open()
{
    m_fd = webserv::open(m_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (!m_fd)
    {
        logg << "\"" << m_path << "\": open(): " << std::strerror(errno) << '\n';
        return -1;
    }
    logg << "File \"" << m_path << "\" opened for writing (fd: " << m_fd << ")\n"; 
    return 0;
}

}
