#pragma once

#include "packets/gme.hpp"
#include "packets/gumi_live.hpp"

namespace BfCrypt
{
	/**
	* Constructs a SREE body with a body JSON
	* @param[in] input_json Input JSON to use as the request body
	* @return true in case the construction succeeded, otherwise false
	*/
	std::optional<SREE> BuildSREE(std::string_view input_json);

	/**
	* Constructs a GME body with a body JSON
	* @param[in] input_json Input JSON to use as the request body
	* @param[in] key Cryptation key
	* @return true in case the construction succeeded, otherwise false
	*/
	std::optional<GmeBody> BuildGME(std::string_view input_json, std::string_view key);

	/**
	* Reads a GME request and decodes it
	* @param[in] root Root JSON to decode
	* @param[in] key Cryptation key
	* @return Decoded JSON
	*/
	std::optional<std::string> ReadGME(const GmeAction& root, std::string_view key);
}
