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

//GET :project/{project_id}/tasks
void get_all_task_in_project(http::response<http::dynamic_body>& res, http::request<http::dynamic_body>& req)
{
	std::string str = " { \"get_uri\" : \"" + req.target().to_string() + "\"}";
	boost::beast::ostream(res.body()) << str;
}

//project/{project_id}/task/{task_id}
void get_task_in_project(http::response<http::dynamic_body>& res, http::request<http::dynamic_body>& req)
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

		std::shared_ptr<resource> uri_endpoints(new resource);
		uri_endpoints->add_get_handler(std::string("^/project/[0-9]+/tasks"), std::bind(get_all_task_in_project, 				std::placeholders::_1, std::placeholders::_2));
		uri_endpoints->add_post_handler(std::string("^/github/comment"), std::bind(post_github_comment_handle, 					std::placeholders::_1, std::placeholders::_2));

		server.publish(uri_endpoints);
		server.start();

	}
	catch (std::exception const& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
```




