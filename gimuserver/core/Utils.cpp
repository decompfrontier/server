#include "Utils.hpp"
#include <drogon/drogon.h>

using namespace drogon;

static const char alphanum[] =
"0123456789"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz";

trantor::LogStream& operator<<(trantor::LogStream& stream, const std::unordered_map<std::string, std::string, drogon::utils::internal::SafeStringHash>& map)
{
	for (const auto& [k, v] : map)
	{
		stream << "key=" << k << "\tvalue=" << v << "\n";
	}
	return stream;
}

void Utils::DumpInfoToDrogon(const drogon::HttpRequestPtr& rq, const std::string& ip)
{
	LOG_TRACE << ip << "->request dump: " << rq->getPath() << " (" << rq->getMethodString() << ")\n"
		<< "Cookies:\n" << rq->getCookies()
		<< "Parameters:\n" << rq->getParameters()
		<< "Headers:\n" << rq->getHeaders()
		<< "Body:\n" << rq->getBody()
	;
}

std::string Utils::GetDrogonBindHostname()
{
	return app().getListeners()[0].toIpPort();
}

std::string Utils::GetDrogonHttpBindHostname()
{
	std::string q = "http://";
	q += GetDrogonBindHostname();
	q += "/";
	return q;
}

std::string Utils::RandomUserID()
{
	std::string r;

	for (int i = 0; i < 8; i++)
		r += alphanum[rand() % (sizeof(alphanum) - 1)];

	return r;
}

std::string Utils::RandomAccountID()
{
	std::string r;

	for (int i = 0; i < 8; i++)
		r += alphanum[rand() % (sizeof(alphanum) - 1)];

	return r;
}