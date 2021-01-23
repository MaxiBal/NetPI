#define IS_IPV4 false
#define IS_IPV6 true

#include "socket.h"


/* Doesn't mix well with http */
netpi::response on_req(netpi::request req)
{

	netpi::response r;

	std::cout << "Data: " << req.data << std::endl;

	/* Can curl into the open socket and get response */
	r.send_line( "Hello cURL!");

	return r;
}

int main(void)
{

	/* Testing the sockets */

	netpi::server_socket so;

	netpi::callback s_event;
	s_event.socket_action = std::function<netpi::response(netpi::request)>(on_req);

	std::thread t([&]() 
	{
			so.listen_(s_event);
	});

	std::cout << "Socket listening on Port " << PORT << std::endl;

	/* Connect to the server socket at localhost:65015 */
	/* Currently communication is one way (server -> client) */

	netpi::client_socket client;

	netpi::send_packet packet;
	packet.data = "Hello testing!";

	client.send_(packet);

	t.join();
	return 0;
}