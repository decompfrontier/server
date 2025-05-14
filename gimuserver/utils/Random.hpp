#pragma once

#include <string>
#include <cstdlib>

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
