#pragma once

#include <thread>

#include "socket_event.h"

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

/* Configures the max length of the client queue */

#ifndef CLIENT_QUEUE_LEN
#define CLIENT_QUEUE_LEN 10
#endif


#include "response.h"
#include "route.h"
#include "socket_options.h"
#include "response.h"

namespace netpi {
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

	public:
		server_socket_options options;

		socket_event on_connect = std::function<void()>();
		socket_event on_end = std::function<void()>();

		router server_router;

		int listen_()
		{
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
				perror("setsockopt()");
				return EXIT_FAILURE;
			} // ret == -1
			
			// configure the server's address
			server_addr.sin6_family = AF_INET6;
			server_addr.sin6_addr = in6addr_any;
			server_addr.sin6_port = htons(PORT);

			/* Bind address and socket together */
			ret = bind(listen_sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
			if (ret == -1) {
				perror("bind()");
				close(listen_sock_fd);
				return EXIT_FAILURE;
			} // ret == -1

			/* Create listening queue (client requests) */
			ret = listen(listen_sock_fd, CLIENT_QUEUE_LEN);
			if (ret == -1) {
				perror("listen()");
				close(listen_sock_fd);
				return EXIT_FAILURE;
			} // ret == -1

			client_addr_len = sizeof(client_addr);


			int code = 0;
			while (true)
			{
				client_sock_fd = accept(listen_sock_fd,
					(struct sockaddr*)&client_addr,
					&client_addr_len);
				if (client_sock_fd == -1) {
					perror("accept()");
					close(listen_sock_fd);
					return EXIT_FAILURE;
				} // client_sock_fd == -1

				inet_ntop(AF_INET6, &(client_addr.sin6_addr),
					str_addr, sizeof(str_addr));
				printf("New connection from: %s:%d ...\n",
					str_addr,
					ntohs(client_addr.sin6_port));

				on_connect.socket_action();

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

				std::cout << rcv << std::endl;

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

				printf("Connection closed\n");
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
		socket_response res;
		// defaults to localhost
		struct sockaddr_in6 server_addr;

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
			sock_fd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
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

		void configure_ip(struct sockaddr_in6& other)
		{
			server_addr = other;
		}

	}; // client_socket
} // namespace