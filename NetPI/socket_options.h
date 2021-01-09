#pragma once

namespace netpi
{
	struct server_socket_options
	{
		bool secure = false; // Decides if the socket encrypts its data before sending it
		bool using_headers = false; // Setting headers
	}; // struct server_socket_options
} // namespace