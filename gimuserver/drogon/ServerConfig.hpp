#pragma once

/*!
* Server configuration.
*/
struct ServerConfig
{
	/*!
	* Initial level.
	*/
	uint32_t initialLevel;

	/*!
	* Initial ZEL.
	*/
	uint32_t initialZel;

	/*!
	* Initial Karma.
	*/
	uint32_t initialKarma;

	/*!
	* Initial brave coins.
	*/
	uint32_t initialBraveCoins;

	/*!
	* Frame-rate cap delivered to the offline-proxy client at startup. 0
	* disables the client-side cap. Read from plugins[0].config.server.fps_cap
	* in deploy/config.json; defaults to 60 if absent.
	*/
	uint32_t fpsCap;
};
