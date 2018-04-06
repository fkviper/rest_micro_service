#include "stdafx.h"
#include "http_server.h"

int main(int argc, char* argv[])
{
	try
	{
		http_server server;
		server.config_.port_ = 8080;
		server.start();
	}
	catch (std::exception const& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}