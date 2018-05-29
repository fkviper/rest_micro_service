#include "stdafx.h"

class http_connection : public std::enable_shared_from_this<http_connection>
{
public:
	http_connection(tcp::socket socket,bool is_sync,
		std::shared_ptr<resource> uri_resource)
		: socket_(std::move(socket)),
		is_sync_(is_sync),
		uri_handlers_(uri_resource)
	{
		std::cout << "----------------------------------------------------------CONNECTION STARTS------------------------------------------------------" << std::endl;
	}

	~http_connection() {
		
		std::cout << "REQUEST :\n "<< 
			"METHOD :" << request_.method_string().to_string() <<"\n"<<
			"TARGET :" << request_.target().to_string()<<"\n"<<
			"DATA :" << boost::beast::buffers_to_string(request_.body().data()) << std::endl;
		std::cout << "REPONSE : \n "<< 
			"RESULT :" << response_.result_int() << "\n" << 
			"DATA :" << boost::beast::buffers_to_string(response_.body().data()) << std::endl;
		std::cout << "-----------------------------------------------------------CONNECTION ENDS-------------------------------------------------------" << std::endl;
	}

	// Initiate the asynchronous operations associated with the connection.
	void
		start()
	{
		read_request();
		if (is_sync_ == false)
			check_deadline();
		else
		{
			auto self = shared_from_this();
			self->deadline_.cancel();
		}
	}

private:
	bool is_sync_;
	
	std::shared_ptr<resource> uri_handlers_;

	// The socket for the currently connected client.
	tcp::socket socket_;

	// The buffer for performing reads.
	boost::beast::flat_buffer buffer_{ 8192 };

	// The request message.
	http::request<http::dynamic_body> request_;

	// The response message.
	http::response<http::dynamic_body> response_;

	// The timer for putting a deadline on connection processing.
	boost::asio::basic_waitable_timer<std::chrono::steady_clock> deadline_{
		socket_.get_executor().context(), std::chrono::seconds(60) };

	// Asynchronously receive a complete request message.
	void
		read_request()
	{
		auto self = shared_from_this();
		if (is_sync_)
		{
			boost::system::error_code ec;
			std::size_t bytes_transferred = http::read(socket_, buffer_, request_, ec);
			boost::ignore_unused(bytes_transferred);
			if (!ec)
				self->process_request();
			else
				self->socket_.close(ec);
		}
		else {
			http::async_read(
				socket_,
				buffer_,
				request_,
				[self](boost::beast::error_code ec,
					std::size_t bytes_transferred)
			{
				boost::ignore_unused(bytes_transferred);
				if (!ec)
					self->process_request();
			});
		}
	}

	// Determine what needs to be done with the request message.
	void
		process_request()
	{
		response_.version(request_.version());
		response_.keep_alive(false);

		switch (request_.method())
		{
		case http::verb::get:
			response_.result(http::status::ok);
			response_.set(http::field::server,"Beast");
			create_response(uri_handlers_->get_relative_uri_map_);
			break;
		case http::verb::post:
			response_.result(http::status::created);
			response_.set(http::field::server, "Beast");
			create_response(uri_handlers_->post_relative_uri_map_);
			break;
		default:
			// We return responses indicating an error if
			// we do not recognize the request method.
			response_.result(http::status::bad_request);
			response_.set(http::field::content_type,"text/plain");
			boost::beast::ostream(response_.body())
				<< "Invalid request-method '"
				<< request_.method_string().to_string()
				<< "'";
			break;
		}
		write_response();
	}

	// Construct a response message based on the program state.
	void
		create_response(std::map<regex_orderable, std::map<boost::beast::http::verb, handler_f_>>& resources)
	{
		response_.set(http::field::content_type, "application/json;charset=UTF-8");
		//response_.set(http::field::content_language, "es");
		bool found = false;
		for (auto &regex_method : resources) {
			auto it = regex_method.second.find(request_.method());
			if (it != regex_method.second.end())
			{
				std::smatch sm_res;
				std::string target_uri = request_.target().to_string();
				if (std::regex_match(target_uri, sm_res, regex_method.first)) {
					handler_f_ callable_func = it->second;
					callable_func(response_, request_);
					found = true;
				}
			}
		}
		if (!found)
		{
			response_.result(http::status::not_found);
			response_.set(http::field::content_type, "text/json");
			boost::beast::ostream(response_.body()) << "API not supported\r\n";
		}
	}



	// Asynchronously transmit the response message.
	void
		write_response()
	{
		auto self = shared_from_this();

		response_.set(http::field::content_length, response_.body().size());
		if (is_sync_)
		{
			boost::beast::error_code ec;
			http::write(socket_, response_, ec);
			if (!ec)
			{
				self->socket_.shutdown(tcp::socket::shutdown_send, ec);
			}
		}
		else
		{
			http::async_write(
				socket_,
				response_,
				[self](boost::beast::error_code ec, std::size_t)
			{
				self->socket_.shutdown(tcp::socket::shutdown_send, ec);
				self->deadline_.cancel();
			});
		}
	}

	// Check whether we have spent enough time on this connection.
	void
		check_deadline()
	{
		auto self = shared_from_this();
		deadline_.async_wait(
				[self](boost::beast::error_code ec)
			{
				if (!ec)
				{
					// Close socket to cancel any outstanding operation.
					self->socket_.close(ec);
				}
			});
	}
};