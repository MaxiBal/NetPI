#include <wiringPi.h>

#ifndef PORT
#define PORT 65015
#endif

#define IS_IPV4 false
#define IS_IPV6 true
#define NETPI_VERBOSE true

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
#include <pthread.h>
#include "socket.h"

void _on_connect()
{
	std::cout << "This is the server socket's on connect function!" << std::endl;
}

int main(void)
{
	// wiringPiSetupSys();

	/* Testing the sockets */

	netpi::server_socket so;

	so.on_connect = std::function<void()>(_on_connect);

	std::thread t(&netpi::server_socket::listen_, so);

	std::cout << "Socket listening on Port " << PORT << std::endl;

	/* Connect to the server socket at localhost:65015 */
	/* Currently communication is one way (client -> server) */

	t.join();
	return 0;
}