#pragma once

#ifndef BUF_LEN
#define BUF_LEN 1024
#endif

namespace netpi {
	struct socket_response 
	{
		bool readable;
		bool auto_destroy; // Decides whether or not the socket should self-destroy after it is done with it's operations
		bool destroyed; // Whether or not the socket has been destroyed
		int status_code;
		char* status_message;
		char data[BUF_LEN];
		char* read()
		{
			return (char*) "a";
		}


	};
}