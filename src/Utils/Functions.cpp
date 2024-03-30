#include <cstring>

#include "Functions.hpp"

namespace webserv
{

std::vector<Byte>   to_vector(const std::string& str)
{
	std::vector<Byte>   vec;

	vec.resize(str.size());
	std::memcpy(vec.data(), str.c_str(), str.size());
	return (vec);
}

template<>
std::string to_string(const std::vector<Byte>& value)
{
	return (std::string(value.begin(), value.end()));
}

std::vector<std::string>    splitByChars(const std::string& src, const std::string& chars)
{
	std::vector<std::string>	splitedStr;
	size_t	i = 0;
	size_t	wc = 0;
	bool	hasDelimiter = false;

	for ( ; i < src.size(); ++i)
	{
		if (std::strchr(chars.c_str(), static_cast<int>(src[i])))
        {
            if (wc)
    			hasDelimiter = true;
        }
		else
			++wc;
		if (hasDelimiter && wc)
		{
			splitedStr.push_back(std::string(src, i - wc, wc));
			wc = 0;
			hasDelimiter = false;
		}
	}
	if (wc)
		splitedStr.push_back(std::string(src, i - wc, wc));
	return (splitedStr);
};

std::string	dequote(const std::string& str)
{
	if (str.size() == 1)
		return (str);
	if (str[0] == '"' && str[str.size() - 1] == '"')
		return (str.substr(1, str.size() - 2));
	return (str);
}

}