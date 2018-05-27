// rest_service_using_beast.cpp : Defines the entry point for the console application.

//
// Copyright (c) 2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//
#pragma once
#include "stdafx.h"
#include "config.h"
#include "http_connection.h"

//http - 80
//https - 443
//proxy -8080
//port > 2000
class http_server {
public:
	http_server(unsigned int port = 80):config_(port){}
	void start() {
		if (!io_service_) {
			io_service_ = std::make_shared<boost::asio::io_service>();
		}

		if (io_service_->stopped())
			io_service_->reset();


		tcp::endpoint endpoint;
		if (config_.address_.size() > 0)
			endpoint = boost::asio::ip::tcp::endpoint(ip::address::from_string(config_.address_), config_.port_);
		else
			endpoint = tcp::endpoint(tcp::v4(), config_.port_);

		if (!acceptor_)
			acceptor_ = std::unique_ptr<tcp::acceptor>(new tcp::acceptor(*io_service_));

		socket_ = std::unique_ptr<tcp::socket>(new tcp::socket(*io_service_));
		acceptor_->open(endpoint.protocol());
		acceptor_->set_option(boost::asio::socket_base::reuse_address(config_.reuse_address_));
		acceptor_->bind(endpoint);
		acceptor_->listen();
		start_listen();
		io_service_->run();
	}
	config config_;
private:
	// "Loop" forever accepting new connections.
	void start_listen() {

		if (config_.is_sync_)
		{
			for (;;)
			{
				acceptor_->accept(*socket_);
				handle_connection();
			}
		}
		else
		{
			acceptor_->async_accept(*socket_,
				[&](boost::beast::error_code ec)
			{
				if (!ec)
					std::make_shared<http_connection>(std::move(*socket_), config_.is_sync_,resources_, resources_specialized_)->start();
				start_listen();
			});
		}	
	}
	void handle_connection() {
		std::make_shared<http_connection>(std::move(*socket_), config_.is_sync_, resources_, resources_specialized_)->start();
	}
	std::unique_ptr<tcp::acceptor> acceptor_;
	std::shared_ptr<boost::asio::io_service> io_service_;
	std::unique_ptr<tcp::socket> socket_;
	//URI Routing Changes
public:
	void set_handler(std::string uri_regex, boost::beast::http::verb method, handler_f_ func) {
		resources_[uri_regex][method] = func;
	}

	void set_get_handler(std::string uri_regex, handler_f_ func) {
		resources_[uri_regex][boost::beast::http::verb::get] = func;
	}

	void set_handler(std::string& uri_regex, boost::beast::http::verb& method, handler_f_special& func) {
		resources_specialized_[uri_regex][method] = func;
	}
private:
	std::map<regex_orderable, std::map<boost::beast::http::verb, handler_f_>> resources_;
	std::map<regex_orderable, std::map<boost::beast::http::verb, handler_f_special>> resources_specialized_;
};




