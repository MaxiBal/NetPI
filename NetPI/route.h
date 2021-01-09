#pragma once

#include <vector>

namespace netpi 
{
	// Manages one route
	struct route
	{
		const char* route_name; // Example: "/foo"
		const char* send_on_request; // Server's response on connection
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
} // namespace