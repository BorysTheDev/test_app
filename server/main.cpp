#include "server.hpp"


int main()
{
	server serv;
	if (serv.init(PIPENAME))
	{
		while (true)
		{
			serv.process_client_msgs();
		}
	}

	return 0;
}