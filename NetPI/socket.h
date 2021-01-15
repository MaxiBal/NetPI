#pragma once

#include <thread>
#include <vector>
#include <sstream>
#include <functional>
#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <thread>
#include <chrono>
#include <map>


#ifndef MSG_CONFIRM
#define MSG_CONFIRM 1
#endif

/* Configure default settings */

#ifndef DEFAULT_LOCALHOST
#define DEFAULT_LOCALHOST true
#endif

#ifndef IS_IPV4
#define IS_IPV4 true
#endif

#ifndef IS_IPV6
#define IS_IPV6 false
#endif

#ifndef NETPI_VERBOSE
#define NETPI_VERBOSE false
#endif

#ifndef PORT
#define PORT 65015
#endif

/* Configures the max length of the client queue */

#ifndef CLIENT_QUEUE_LEN
#define CLIENT_QUEUE_LEN 10
#endif

/* Sets up local computer's ip address */

#if IS_IPV4
sockaddr_in this_addr;
static void configure_ip()
{
	this_addr.sin_family = AF_INET;
	this_addr.sin_addr = INADDR_ANY;
	this_addr.sin_port = PORT;
}

#endif

#if IS_IPV6
sockaddr_in6 this_addr;
static void configure_ip()
{
	this_addr.sin6_family = AF_INET6;
	this_addr.sin6_addr = in6addr_any;
	this_addr.sin6_port = htons(PORT);
}
#endif

static int _error(const char* msg)
{
	perror(msg);
	return EXIT_FAILURE;
}

#ifdef __cplusplus

static std::vector<std::string> split(std::string s, std::string delimiter)
{
	std::vector<std::string> return_vec;

	size_t pos = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos) 
	{
		token = s.substr(0, pos);
		return_vec.push_back(token);
		s.erase(0, pos + delimiter.length());
	}

	return_vec.push_back(s);

	return return_vec;
}

static std::vector<std::string> split_first(std::string s, std::string delimiter)
{
	size_t pos = s.find(delimiter);

	return std::vector<std::string>{s.substr(0, pos), s.substr(pos, s.size())};
}

static void print_map(std::map<std::string, std::string> m)
{
	for (std::map<std::string, std::string>::iterator i = m.begin(); i != m.end(); ++i)
	{
		std::cout << i->first << ": " << i->second << "\n";
	}
}

static std::map<std::string, std::string> map_request(const std::string& total_request)
{
	std::istringstream ss(total_request);
	std::string line;
	std::map<std::string, std::string> parsed_request{};

	while (getline(ss, line))
	{
		std::vector<std::string> parsed_line = split(line, ": ");

		if (parsed_line.size() >= 2)
		{
			parsed_request.insert(std::make_pair(parsed_line[0], parsed_line[1]));
		}
	}
	return parsed_request;
}



#endif

namespace netpi 
{
	struct socket_event
	{

		socket_event() = default;
		socket_event(std::function<void()> act)
		{
			socket_action = act;
		}

		// The socket_event's action
		std::function<void()> socket_action;

		/* Copy constructor*/
		socket_event(const socket_event& other)
		{
			socket_action = other.socket_action;
		}

		/* Copying another socket_event*/
		void operator=(const socket_event& other) { socket_action = other.socket_action; }

		/* Operators for calling socket_action */
		void operator()() 
		{
			socket_action();
		}

		operator bool()
		{
			return (bool)socket_action;
		}
	};

	

	struct request
	{
		bool hand_shaked = true;
		int status_code;
		char* status_message;
		char* origin;
		std::string connection;
		int cache_control = 0;
		bool upgrade_insecure_requests = false;

		std::string user_agent;
		std::string client_user_agent;
		std::string host;
		const char* lang;
		std::string target; // Describes the ip the client tried to request from
		std::string accept;

		std::string data;

		const char* read()
		{
			return data.c_str();
		}

	};

	/* Outlines a response */
	struct response
	{
		bool readable;
		int status_code;
		char* status_message;
		std::string data;

		void send(std::string d)
		{
			data = data + d;
		}

		void send(const char* d)
		{
			data = data + std::string(d);
		}
	};

	/* Standard callback for the server socket */
	/* Callback function should take in netpi::request and return a netpi::response */
	struct callback
	{
		callback() = default;
		callback(std::function<netpi::response(netpi::request)> act)
		{
			socket_action = act;
		}

		std::function<netpi::response(netpi::request)> socket_action;

		void operator=(const callback& other) { socket_action = other.socket_action; }
		void operator()(netpi::request r) { socket_action(r); }

	};

	// Manages one route
	struct route
	{
		const char* route_name; // Example: "/foo"
		const netpi::response send_on_request; // Server's response on connection
	}; // route

	// Manages a set of routes and/or routers
	struct router
	{
		const char* router_name; // Example: "/foo"
		std::vector<route> child_routes; // Example: "/foo/bar"
		std::vector<router> child_routers;

		// Sets a route as a part of the router
		void on_(route r)
		{
			child_routes.push_back(r);
		}

		// Sets a router as a part of the router
		void on_(router r)
		{
			child_routers.push_back(r);
		}
	}; // router

	struct server_socket_options
	{
		bool secure = false; // Decides if the socket encrypts its data before sending it
	}; // struct server_socket_options

#ifdef __cplusplus
	static request parse_request(std::map<std::string, std::string> req)
	{
		request r;

		print_map(req);

		r.hand_shaked = true;

		if (req.find("User-Agent") != req.end())
		{
			r.user_agent =req["User-Agent"];
		}

		if (req.find("Accept") != req.end())
		{
			r.accept = req["Accept"].c_str();
		}

		if (req.find("Client's user-agent") != req.end())
		{
			r.client_user_agent = req["Client's user-agent"];
		}

		if (req.find("Connection") != req.end())
		{
			r.connection = req["Connection"];
		}

		if (req.find("Host") != req.end())
		{
			
		}

		return r;
	}
#endif
	class server_socket
	{
	private:
		int listen_sock_fd = -1, client_sock_fd = -1;

		struct sockaddr_in6 server_addr, client_addr;
		socklen_t client_addr_len;

		char str_addr[INET6_ADDRSTRLEN];
		int ret, flag;

		char ch = 'Y';

		bool _active = false;
		bool _connected = false;

		router server_router;

	public:
		server_socket_options options;

		socket_event on_connect;
		socket_event on_end;

		void add_route(route r)
		{
			server_router.on_(r);
		}

		void add_router(router r)
		{
			server_router.on_(r);
		}

		int listen_(callback callback_)
		{
			configure_ip();

			server_addr = this_addr;

			_active = true;

			listen_sock_fd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
			if (listen_sock_fd == -1) {
				perror("socket()");
				return EXIT_FAILURE;
			} // listen_sock_fd == -1

			/* Set socket to reuse address */
			flag = 1;
			ret = setsockopt(listen_sock_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
			if (ret == -1) {
				return _error("setsockopt() failed");
			} // ret == -1

			/* Bind address and socket together */
			ret = bind(listen_sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
			if (ret == -1) {
				close(listen_sock_fd);
				return _error("bind() failed");
			} // ret == -1

			/* Create listening queue (client requests) */
			ret = listen(listen_sock_fd, CLIENT_QUEUE_LEN);
			if (ret == -1) {
				
				close(listen_sock_fd);
				return _error("listen failed");

			} // ret == -1

			client_addr_len = sizeof(client_addr);


			int code = 0;
			while (true)
			{
				client_sock_fd = accept(listen_sock_fd,
					(struct sockaddr*)&client_addr,
					&client_addr_len);
				if (client_sock_fd == -1) {
					
					close(listen_sock_fd);
					return _error("accept() failed");
				} // client_sock_fd == -1

				inet_ntop(AF_INET6, &(client_addr.sin6_addr),
					str_addr, sizeof(str_addr));
				if (NETPI_VERBOSE)
					printf("New connection from: %s:%d ...\n",
						str_addr,
						ntohs(client_addr.sin6_port));

				if (on_connect) on_connect.socket_action();

				/* Wait for data from client */
				const unsigned int MAX_BUF_LENGTH = 4096;
				std::vector<char> buffer(MAX_BUF_LENGTH);
				std::string rcv;
				int bytesReceived = 0;
				do {
					bytesReceived = recv(client_sock_fd, &buffer[0], buffer.size(), 0);
					// append string from buffer.
					if (bytesReceived == -1) {
						// error 
					}
					else {
						rcv.append(buffer.cbegin(), buffer.cend());
					}
				} while (bytesReceived == MAX_BUF_LENGTH);

				if (ret == -1) {
					perror("read()");
					close(client_sock_fd);
					continue;
				} // ret == -1

				/* Parsing the request to make a request object */

				
				std::map<std::string, std::string> parsed_request = map_request(rcv);

				netpi::request req = parse_request(parsed_request);

				callback_(req);

				/* Send response to client */
				ret = write(client_sock_fd, &ch, 1);
				if (ret == -1) {
					perror("write()");
					close(client_sock_fd);
					continue;
				} // ret == -1

				/* Do TCP teardown */
				ret = close(client_sock_fd);
				if (ret == -1) {
					perror("close()");
					client_sock_fd = -1;
				} // ret == -1

				if (on_end)
					on_end();

				if (NETPI_VERBOSE) printf("Connection closed\n");

			} // while (true)
			printf("Thread exiting with code %i", code);
			return code;
		} // listen_

		bool is_connected()
		{
			return _connected;
		} // connected

		bool active()
		{
			return _active;
		} // active
	}; // server_socket

	class client_socket 
	{
	private:
		response res;
		// defaults to localhost
#if IS_IPV6
		struct sockaddr_in6 server_addr;
#endif
#if IS_IPV4
		struct sockaddr_in server_addr;
#endif

	public:

		// Configures the default to localhost
		client_socket()
		{
			if (IS_IPV6 && DEFAULT_LOCALHOST)
			{
				// configures default to localhost
				server_addr.sin6_family = AF_INET6;
				inet_pton(AF_INET6, "::1", &server_addr.sin6_addr);
				server_addr.sin6_port = htons(PORT);
			}
		}

		int send_(const char* send_data)
		{
			int sock_fd = -1;
			int ret;
			

			/* Create socket for communication with server */
			if (IS_IPV6)
				sock_fd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
			else
				sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			if (sock_fd == -1) {
				perror("socket()");
				return EXIT_FAILURE;
			} // sock_fd == -1

			/* Connect to server running on localhost */
			

			/* Try to do TCP handshake with server */
			ret = connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
			if (ret == -1) {
				perror("connect()");
				close(sock_fd);
				return EXIT_FAILURE;
			} // ret == -1

			/* Send data to server */
			send(sock_fd,  send_data, std::string(send_data).size(), MSG_CONFIRM);
			if (ret == -1) {
				perror("write");
				close(sock_fd);
				return EXIT_FAILURE;
			} // ret == -1

			/* Wait for data from server */
			ret = read(sock_fd, &res.data, 1);
			if (ret == -1) {
				perror("read()");
				close(sock_fd);
				return EXIT_FAILURE;
			} // ret == -1

			printf("Received %c from server\n", res.data);

			/* DO TCP teardown */
			ret = close(sock_fd);
			if (ret == -1) {
				perror("close()");
				return EXIT_FAILURE;
			} // ret == -1

			return EXIT_SUCCESS;
		} // listen_

#if IS_IPV6
		void configure_ip(struct sockaddr_in6& other)
		{
			server_addr = other;
		}
#endif
#if IS_IPV4
		void configure_ip(struct sockaddr_in& other)
		{
			server_addr = other;
		}
#endif
	}; // client_socket
} // namespace