#include "server.hpp"

server::server() = default;

server::~server()
{
	CloseHandle(m_pipe);
}

bool server::init(std::string pipe_name)
{
	constexpr auto pipemode = PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT;
	m_pipe = CreateNamedPipe(pipe_name.c_str(), PIPE_ACCESS_DUPLEX, pipemode, 2, BUFSIZE, BUFSIZE, 0, NULL);

	if (m_pipe == INVALID_HANDLE_VALUE)
	{
		printf("CreateNamedPipe failed, GLE=%d.\n", GetLastError());
		return false;
	}
	printf("Pipe Server awaiting client connection on %s.\n", pipe_name.c_str());
	return true;
}

bool server::read_msg()
{
	DWORD bytes_read;
	auto res = ReadFile(m_pipe, read_buf.get(), BUFSIZE, &bytes_read, NULL);

	if (!res || bytes_read == 0)
	{
		if (GetLastError() == ERROR_BROKEN_PIPE)
		{
			printf("Client disconnected.\n");
		}
		else
		{
			printf("ReadFile failed, GLE=%d.\n", GetLastError());
		}
	}
	printf("Get message: %s\n", read_buf.get());
	return res;
}

bool server::write_msg(std::string msg)
{
	auto msg_size = static_cast<DWORD>(msg.size() + 1);
	if (msg.size() >= BUFSIZE)
	{
		printf("Max msg size is %d", BUFSIZE);
		return false;
	}
	DWORD writen_bytes = 0;
	auto res = WriteFile(m_pipe, msg.data(), msg_size, &writen_bytes, NULL);

	if (!res || msg_size != writen_bytes)
	{
		printf("WriteFile failed, GLE=%d.\n", GetLastError());
	}
	return res;
}

void server::process_client_msgs()
{
	if (!ConnectNamedPipe(m_pipe, NULL))
	{
		printf("ConnectNamedPipe failed, GLE=%d.\n", GetLastError());
	}
	printf("Client connected.\n");


	while (read_msg())
	{
		write_msg(m_interpreter.interpret(read_buf.get()));
	}

	FlushFileBuffers(m_pipe);
	DisconnectNamedPipe(m_pipe);
}
