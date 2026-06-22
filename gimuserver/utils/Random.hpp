#pragma once

#include <cstdlib>
#include <cstdint>
#include <string>

/*!
* Allowed alphanumeric characters for random.
*/
static constexpr const char Random_alphanum[] =
	"0123456789"
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz";

/*!
* Generates a random ID.
*/
static std::string RandomId()
{
	std::string r;

	for (int i = 0; i < 8; i++)
		r += Random_alphanum[rand() % (sizeof(Random_alphanum) - 1)];

	return r;
}

/*!
* Generates a random unsigned integer in the inclusive range [min, max].
*/
static uint32_t RandomUInt(const uint32_t min, const uint32_t max)
{
	return min + (std::rand() % (max - min + 1));
}
