#pragma once;
#include <unordered_map>
#include <functional>
#include <string>

class executable_object
{
public:
	using strs_it = std::vector<std::string>::iterator;
	virtual std::string execute(strs_it begin, strs_it end) = 0;
	virtual ~executable_object() = default;
};

class executable_string : public executable_object
{
public:
	std::string execute(strs_it begin, strs_it end) override;

private:
	using function_call = std::function<std::string(std::string&, strs_it begin, strs_it end)>;
	const static std::unordered_map<std::string, function_call> m_funcs;
	std::string m_obj;
};