/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Resource.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/20 16:45:29 by tchoquet          #+#    #+#             */
/*   Updated: 2024/04/05 18:45:24 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include <string>
#include <map>

#include "Utils/SharedPtr.hpp"

namespace webserv
{

class Resource
{
public:
    static SharedPtr<Resource> create(const std::string& path, const std::map<std::string, std::string>& cgiExt);

    const std::string& path() { return m_path; }
    
    virtual int open() = 0;

    virtual inline ~Resource() {}

protected:
    Resource(const std::string& path);

    const std::string m_path;
};
typedef SharedPtr<Resource> ResourcePtr;

} // namespace webserv

#endif // RESOURCE_HPP
