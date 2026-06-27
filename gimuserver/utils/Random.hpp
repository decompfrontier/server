#pragma once

#include <chrono>
#include <cstdint>
#include <mutex>
#include <random>
#include <stdexcept>
#include <string>

/*!
* Allowed alphanumeric characters for random.
*/
inline constexpr const char Random_alphanum[] =
	"0123456789"
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz";

inline std::mt19937& RandomEngine()
{
	static std::mt19937 engine([] {
		std::random_device randomDevice;
		const auto now = static_cast<uint64_t>(
			std::chrono::high_resolution_clock::now().time_since_epoch().count());
		std::seed_seq seed{
			randomDevice(),
			randomDevice(),
			static_cast<uint32_t>(now),
			static_cast<uint32_t>(now >> 32),
		};

		return std::mt19937(seed);
	}());

	return engine;
}

inline std::mutex& RandomMutex()
{
	static std::mutex mutex;
	return mutex;
}

/*!
* Generates a random unsigned integer in the inclusive range [min, max].
*/
inline uint32_t RandomUInt(const uint32_t min, const uint32_t max)
{
	if (min > max)
	{
		throw std::invalid_argument("Invalid RandomUInt range");
	}

	std::lock_guard lock(RandomMutex());
	std::uniform_int_distribution<uint32_t> distribution(min, max);
	return distribution(RandomEngine());
}

/*!
* Generates a random ID.
*/
inline std::string RandomId()
{
	std::string r;

	for (int i = 0; i < 8; i++)
	{
		r += Random_alphanum[RandomUInt(0, sizeof(Random_alphanum) - 2)];
	}

	return r;
}
