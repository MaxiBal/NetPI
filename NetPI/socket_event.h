#pragma once
#include <functional>

// Base struct for all socket events - socket events can hold one action
namespace netpi {
	struct socket_event
	{

		socket_event() = default;
		socket_event(std::function<void()> act)
		{
			socket_action = act;
		}

		// The socket_event's action
		std::function<void()> socket_action;

		socket_event(const socket_event& other)
		{
			socket_action = other.socket_action;
		}
		socket_event& operator=(const socket_event& other) { socket_action = other.socket_action; }
		void operator()(const socket_event& other) { socket_action = other.socket_action;  }
	};
}
