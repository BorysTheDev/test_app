#pragma once;

#include <decl.h>

#include <windows.h> 
#include <stdio.h> 

#include <memory>
#include <string>

enum class client_mode
{
	undefined,
	sync,
	async,
};

class client
{
public:
	client(client_mode mode = client_mode::sync) noexcept;
	~client();

	bool init(std::string pipe_name);
	void start_session();

private:
	bool read_sync();
	bool write_sync(const std::string& msg);

	struct async_data
	{
		OVERLAPPED overlap = {};
		client* client = nullptr;
		std::string write_buf;
	};

	static void WINAPI write_async_callback(DWORD err, DWORD written_bytes, LPOVERLAPPED data);
	static void WINAPI read_async_callback(DWORD err, DWORD read_bytes, LPOVERLAPPED data);

	std::unique_ptr<char[]> m_read_buf = std::make_unique<char[]>(BUFSIZE);
	std::unique_ptr<async_data> m_async_data;

	HANDLE m_pipe = INVALID_HANDLE_VALUE;
	client_mode m_mode = client_mode::sync;
};