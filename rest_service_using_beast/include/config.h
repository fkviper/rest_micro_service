#pragma once
#include "stdafx.h"

class config {
	friend class http_server;
	config(unsigned short port) noexcept : port_(port) {}
public:
	//Defaults to 80 for HTTP and 443 for HTTPS.
	unsigned short port_;

	/// If io_service is not set, number of threads that the server will use when start() is called.
	/// Defaults to 1 thread.
	std::size_t thread_pool_size = 1;

	/// IPv4 address in dotted decimal form or IPv6 address in hexadecimal notation.
	/// If empty, the address will be any address.
	std::string address_;
	/// Set to false to avoid binding the socket to an address that is already in use. Defaults to true.
	bool reuse_address_ = true;
	//set server to be sync or async.
	unsigned short is_sync_;
};
