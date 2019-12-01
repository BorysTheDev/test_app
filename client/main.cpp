#include "client.hpp"
#include <iostream>
#include <limits>

int main()
{
	client_mode mode = client_mode::undefined;
	while (mode == client_mode::undefined)
	{
		std::cout << "Please choose client mode:" << std::endl
			<< "1 - sync;" << std::endl
			<< "2 - async" << std::endl;

		std::string str_mode;
		std::cin >> str_mode;
		try
		{
			mode = static_cast<client_mode>(std::stoi(str_mode));
		}
		catch(...) {}
		if (mode != client_mode::async && mode != client_mode::sync)
		{
			mode = client_mode::undefined;
		}
	}
	
	client cli(mode);
	if (cli.init(PIPENAME))
	{
		cli.start_session();
	}
}
