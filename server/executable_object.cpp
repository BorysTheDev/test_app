#include "executable_object.hpp"

const std::unordered_map<std::string, executable_string::function_call> executable_string::m_funcs =
{
	{"assign", function_call([](std::string& str, strs_it begin, strs_it end) {
			return end - begin == 1 ? str.assign(*begin) : "FAIL"; }) 
	},
	{"substr", function_call([](std::string& str, strs_it begin, strs_it end) {
			auto nargs = end - begin;
			if (nargs >= 0 && nargs <= 2)
			{
				try
				{
					auto arg1 = nargs >= 1 ? std::stol(*begin) : 0;
					auto arg2 = nargs == 2 ? std::stol(*(begin + 1)) : std::string::npos;
					return str.substr(arg1, arg2);
				}
				catch(...){ }
			}
			return std::string("FAIL"); })
	},
	{"append", function_call([](std::string& str, strs_it begin, strs_it end) {
		return end - begin == 1 ? str.append(*begin) : "FAIL"; })
	},
};

std::string executable_string::execute(strs_it begin, strs_it end)
{
	if (begin == end)
	{
		return m_obj;
	}
	auto func_it = m_funcs.find(*begin);
	if (func_it != m_funcs.end())
	{
		return func_it->second(m_obj, begin + 1, end);
	}
	return "FAIL";
}