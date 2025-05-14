#pragma once

#include <gimuserver/packets/net_gme.hpp>

/*!
* Gets the hostname which the server was binded to.
* @return Binded hostname
*/
static inline std::string GetDrogonBindHostname() {
	return drogon::app().getListeners()[0].toIpPort(); // TODO: doesn't sound 100% ok
}

/*!
* Gets the HTTP URL of the binded hostname.
* @return HTTP hostname
*/
static inline std::string GetDrogonHttpBindHostname() {
	return "http://" + GetDrogonBindHostname() + "/";
}
