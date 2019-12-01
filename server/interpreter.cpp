#include "interpreter.hpp"
#include <sstream>
#include <iterator>

std::string interpreter::interpret(std::string command)
{
	std::istringstream iss(command);
	std::vector<std::string> words(std::istream_iterator<std::string>{iss},
		std::istream_iterator<std::string>());

	if (words.size() == 2 && words[0] == "create")
	{
		// For this example I use only string, 
		// but adding several if, it's possible to make different types
		m_dic.emplace(words[1], std::make_unique<executable_string>());
		return "OK";
	}
	else if (words.size() >= 2 && words[0] == "call")
	{
		auto obj_it = m_dic.find(words[1]);
		if (obj_it != m_dic.end())
		{
			return obj_it->second->execute(words.begin() + 2, words.end());
		}
	}
	return "FAIL";
}