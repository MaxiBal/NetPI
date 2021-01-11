#define IS_IPV4 false
#define IS_IPV6 true
#define NETPI_VERBOSE false

#include "socket.h"

void _on_connect()
{
	std::cout << "This is the server socket's on connect function!" << std::endl;
}

int main(void)
{

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