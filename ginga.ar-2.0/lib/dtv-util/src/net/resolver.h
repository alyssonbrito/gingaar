#pragma once

#include "../log.h"
#include "sockaddr.h"
#include <vector>

#ifdef _WIN32
#	include <winsock2.h>
#	include <ws2tcpip.h>
#else
#	include <arpa/inet.h>
#	include <netdb.h>
#endif

namespace util {
namespace net {

inline bool resolve( const std::string &host, const std::string &port, std::vector<util::net::SockAddr> &addresses ) {
	struct addrinfo *result = NULL;

	int error = ::getaddrinfo( host.c_str(), port.c_str(), NULL, &result );
	if (error != 0) {
		LERROR( "resolver", "getaddrinfo failed: error=%s", gai_strerror(error) );
		return false;
	} else {
		for ( struct addrinfo *res = result; res != NULL; res = res->ai_next ) {
			addresses.push_back( util::net::SockAddr( *res->ai_addr ) );
		}
	}
	::freeaddrinfo( result );

	return true;
}

}	//	net
}	//	util
