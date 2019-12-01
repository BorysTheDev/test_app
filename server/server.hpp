#pragma once;

#include "interpreter.hpp"

#include <decl.h>

#include <windows.h> 
#include <stdio.h> 

#include <memory>
#include <string>

class server
{
public:
	server();
	~server();
	
	bool init(std::string pipe_name);
	void process_client_msgs();

private:
	bool read_msg();
	bool write_msg(std::string msg);

	std::unique_ptr<char[]> read_buf = std::make_unique<char[]>(BUFSIZE);

	HANDLE m_pipe = INVALID_HANDLE_VALUE;
	interpreter m_interpreter;
};