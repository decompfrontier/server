#pragma once

inline std::string GetDrogonBindHostname() {
	return drogon::app().getListeners()[0].toIpPort(); // TODO: doesn't sound 100% ok
}

inline std::string GetDrogonHttpBindHostname() {
	return "http://" + GetDrogonBindHostname() + "/";
}
