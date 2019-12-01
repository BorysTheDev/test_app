#include "client.hpp"

#include <decl.h>

#include <windows.h> 
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <iostream>

client::client(client_mode mode) noexcept : m_mode(mode)
{
}

client::~client()
{
	CloseHandle(m_pipe);
}

bool client::init(std::string pipe_name)
{
	constexpr auto pipe_access = GENERIC_READ | GENERIC_WRITE;
	const auto pipe_flags = m_mode == client_mode::sync ? 0 : FILE_FLAG_OVERLAPPED;
	m_pipe = CreateFile(pipe_name.c_str(), pipe_access, 0, NULL, OPEN_EXISTING, pipe_flags, NULL);

	if (m_pipe == INVALID_HANDLE_VALUE)
	{
		printf("Couldn't open pipe, GLE=%d.\n", GetLastError());
		return false;
	}

	DWORD pipe_mode = PIPE_READMODE_MESSAGE;
	if (!SetNamedPipeHandleState(m_pipe, &pipe_mode, NULL, NULL))
	{
		printf("SetNamedPipeHandleState failed. GLE=%d\n", GetLastError());
		return false;
	}

	printf("Client has connected to %s\n", pipe_name.c_str());
	return true;
}

void client::start_session()
{
	if (m_mode == client_mode::sync)
	{
		std::string command;
		while (true)
		{
			command.resize(0);
			while (command.empty())
			{
				std::getline(std::cin, command, '\n');
			}
			write_sync(command);
			read_sync();
		}
	}
	else
	{
		m_async_data = std::make_unique<async_data>();
		m_async_data->client = this;
		m_async_data->overlap.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
		if (m_async_data->overlap.hEvent == nullptr)
		{
			printf("CreateEvent failed with %d.\n", GetLastError());
			return;
		}
		read_async_callback(0, 1, reinterpret_cast<LPOVERLAPPED>(m_async_data.get()));
		while (true)
		{
			auto wfso = WaitForSingleObjectEx(m_async_data->overlap.hEvent, INFINITE, TRUE);
			switch (wfso)
			{
			case 0: break;
			case WAIT_IO_COMPLETION: break;
			default:
				printf("WaitForSingleObjectEx res=%d, GLE(%d)\n", wfso, GetLastError());
			}
		}
	}
}

bool client::read_sync()
{
	DWORD bytes_read;
	auto res = ReadFile(m_pipe, m_read_buf.get(), BUFSIZE, &bytes_read, NULL);

	if (!res || bytes_read == 0)
	{
		printf("read_sync failed, GLE=%d.\n", GetLastError());
	}
	printf("%s\n", m_read_buf.get());
	return res;
}

bool client::write_sync(const std::string& msg)
{
	DWORD writen_bytes = 0;
	auto msg_size = static_cast<DWORD>(msg.size() + 1);
	auto res = WriteFile(m_pipe, msg.c_str(), msg_size, &writen_bytes, NULL);

	if (!res || msg_size != writen_bytes)
	{
		printf("write_sync failed, GLE=%d.\n", GetLastError());
		return false;
	}
	return true;
}

void WINAPI client::write_async_callback(DWORD err, DWORD written_bytes, LPOVERLAPPED data)
{
	auto res = false;
	auto* a_data = reinterpret_cast<async_data*>(data);
	if ((err == 0) && (written_bytes == (a_data->write_buf.size() + 1)))
	{
		a_data->client->m_read_buf[0] = '\0';
		res = ReadFileEx(a_data->client->m_pipe, a_data->client->m_read_buf.get(), BUFSIZE,
			data, client::read_async_callback);
	}

	auto gle = GetLastError();
	if (!res || gle)
	{
		printf("read_async failed, GLE=%d.\n", gle);
	}
}

void WINAPI client::read_async_callback(DWORD err, DWORD read_bytes, LPOVERLAPPED data)
{
	auto res = false;
	if ((err == 0) && (read_bytes != 0))
	{
		auto* a_data = reinterpret_cast<async_data*>(data);
		printf("%s\n", a_data->client->m_read_buf.get());

		a_data->write_buf.resize(0);
		while (a_data->write_buf.empty())
		{
			std::getline(std::cin, a_data->write_buf, '\n');
		}
		
		auto msg_size = static_cast<DWORD>(a_data->write_buf.size() + 1);
		res = WriteFileEx(a_data->client->m_pipe, a_data->write_buf.c_str(),
			msg_size, data, client::write_async_callback);
	}

	if (!res)
	{
		printf("read_async failed, GLE=%d.\n", GetLastError());
	}
}