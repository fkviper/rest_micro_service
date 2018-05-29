# C++11 REST Service using beast,asio and rapid json

Dependecies:
1. Boost(asio,system,regex,datetime)
2. Beast
3. RapidJson


Features planned :
1. Synchronous and Asynchronous server support.(implemented)
2. Routing support.(implemented using regex)
3. SSL support (pending)
4. REST response caching(pending)
5. Graceful termination on crash.(pending)


Hello World Example:
```
#include "stdafx.h"
#include "http_server.h"

//project/{project_id}/task/{task_id}
void get_task_in_project(http::response<http::dynamic_body>& res, http::request<http::dynamic_body>& req)
{
	std::string str = "Hello " + req.target().to_string();
	boost::beast::ostream(res.body()) << str;
	res.result(http::status::ok);
}
//project/{project_id}/tasks
void get_all_task_in_project(http::response<http::dynamic_body>& res, http::request<http::dynamic_body>& req)
{
	std::string str = "Hello " + req.target().to_string();
	boost::beast::ostream(res.body()) << str;
	res.result(http::status::ok);
}


int main(int argc, char* argv[])
{
	try
	{
		http_server server;
		server.config_.port_ = 8080;
		server.config_.is_sync_ = true;

		server.set_get_handler(std::string("^/project/[0-9]+/tasks"), std::bind(get_all_task_in_project, std::placeholders::_1, std::placeholders::_2));
		server.set_get_handler(std::string("^/project/[0-9]+/task/[0-9]+"), std::bind(get_task_in_project, std::placeholders::_1, std::placeholders::_2));

		server.start();

	}
	catch (std::exception const& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
```




