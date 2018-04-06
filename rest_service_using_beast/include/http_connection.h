#include "stdafx.h"

class http_connection : public std::enable_shared_from_this<http_connection>
{
public:
	http_connection(tcp::socket socket)
		: socket_(std::move(socket))
	{
	}

	// Initiate the asynchronous operations associated with the connection.
	void
		start()
	{
		read_request();
		check_deadline();
	}

private:
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
			response_.set(http::field::server, "Beast");
			create_response();
			break;
		default:
			// We return responses indicating an error if
			// we do not recognize the request method.
			response_.result(http::status::bad_request);
			response_.set(http::field::content_type, "text/plain");
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
		create_response()
	{
		response_.set(http::field::content_type, "application/json;charset=UTF-8");
		response_.set(http::field::content_language, "es");
		if (request_.target() == "/count")
		{
			//std::wstring str = L" {\"name\" : \"El hardware inal�mbrico no autorizado se\"}";
			//boost::beast::ostream(response_.body()) << utility::conversions::to_utf8string(str);
			//std::wstring str2 = utility::conversions::to_string_t(utility::conversions::to_utf8string(str));

			std::string str = "Hello World";
			boost::beast::ostream(response_.body()) << str;
		}
		else if (request_.target() == "/time")
		{

		}
		else
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

		http::async_write(
			socket_,
			response_,
			[self](boost::beast::error_code ec, std::size_t)
		{
			self->socket_.shutdown(tcp::socket::shutdown_send, ec);
			self->deadline_.cancel();
		});
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