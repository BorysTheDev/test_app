#pragma once

#include "executable_object.hpp"

#include <unordered_map>
#include <memory>

class interpreter
{
public:
	std::string interpret(std::string command);

private:
	std::unordered_map<std::string, std::unique_ptr<executable_object>> m_dic;
};