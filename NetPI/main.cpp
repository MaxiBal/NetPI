#define IS_IPV4 false
#define IS_IPV6 true

#include "socket.h"

netpi::response on_req(netpi::request req)
{

	netpi::response r;

	/* Can curl into the open socket and get response */
	r.send( "Hello cURL!");

	return r;
}

int main(void)
{

	/* Testing the sockets */

	netpi::server_socket so;

	std::thread t([&]() 
	{
			netpi::callback s_event;
			s_event.socket_action = std::function<netpi::response(netpi::request)>(on_req);
			so.listen_(s_event);
	});

	std::cout << "Socket listening on Port " << PORT << std::endl;

	/* Connect to the server socket at localhost:65015 */
	/* Currently communication is one way (client -> server) */

	t.join();
	return 0;
}