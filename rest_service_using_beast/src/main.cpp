#include "stdafx.h"
#include "http_server.h"


//tasks/{task_id}
void get_task_by_id(http::response<http::dynamic_body>& res, http::request<http::dynamic_body>& req)
{
	std::string str = "Hello World from tasks";
	boost::beast::ostream(res.body()) << str;
	res.result(http::status::ok);
}


//tasks/{task_id}/checklist/{checklist_id}
void get_checklists_id(http::response<http::dynamic_body>& res, http::request<http::dynamic_body>& req)
{
	std::string str = "Hello World from check list";
	boost::beast::ostream(res.body()) << str;
	res.result(http::status::ok);
}


int main(int argc, char* argv[])
{
	try
	{
		http_server server;
		server.config_.port_ = 8080;

		CREATE_GET_ENDPOINT(URI(^/tasks/[0-9]+), get_task_by_id);
		CREATE_GET_ENDPOINT(URI(^/tasks/[0-9]+/checklist/[0-9]+), get_checklists_id);

		server.start();

	}
	catch (std::exception const& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}