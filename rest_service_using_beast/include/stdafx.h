// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "../targetver.h"

#include <stdio.h>
#include <tchar.h>

//#define BOOST_ALL_DYN_LINK

// TODO: reference additional headers your program requires here

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <regex>
//#include "cpprest/asyncrt_utils.h"

//using namespace utility;
namespace ip = boost::asio::ip;         // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>


typedef std::function< void(http::response<http::dynamic_body>&,
	http::request<http::dynamic_body>&)>  handler_f_;
typedef std::function< void(http::response<http::dynamic_body>&,
	http::request<http::dynamic_body>&,
	std::map<std::string, std::string>&)>  handler_f_special;


#define URI(REGEX_STRING) #REGEX_STRING
#define CREATE_GET_ENDPOINT(REGEX_STRING, FUNC_HANDLER) server.set_handler(std::string(REGEX_STRING),boost::beast::http::verb::get,std::bind(FUNC_HANDLER, std::placeholders::_1, std::placeholders::_2))

class regex_orderable : public std::regex {
	std::string str;

public:
	regex_orderable(const char *regex_cstr) : std::regex(regex_cstr), str(regex_cstr) {}
	regex_orderable(std::string regex_str) : std::regex(regex_str), str(std::move(regex_str)) {}
	bool operator<(const regex_orderable &rhs) const noexcept {
		return str < rhs.str;
	}
};