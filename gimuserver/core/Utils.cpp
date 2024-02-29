#include "Utils.hpp"
#include "System.hpp"
#include <drogon/drogon.h>
#include <fstream>
#include <filesystem>

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

void Utils::AppendJsonReqToFile(const Json::Value& v, const std::string& group)
{
	std::string p = System::Instance().LogConfig().RequestPath;

	time_t rawtime;
	time(&rawtime);
	struct tm timeinfo;
	localtime_s(&timeinfo, &rawtime);
	auto ct = std::ctime(&rawtime);

	p += std::filesystem::path::preferred_separator;

	char buf[100];
	strftime(buf, sizeof(buf), "%d_%m_%Y-%H_%M_%S", &timeinfo);

	p += group;
	p += "_";
	p += buf;
	p += ".json";

	std::ofstream of(p, std::ofstream::out);
	of << v.toStyledString().c_str();
	of.close();
}

void Utils::AppendJsonResToFile(const Json::Value& v, const std::string& group)
{
	std::string p = System::Instance().LogConfig().ResponsePath;
	
	time_t rawtime;
	time(&rawtime);
	struct tm timeinfo;
	localtime_s(&timeinfo, &rawtime);
	auto ct = std::ctime(&rawtime);

	p += std::filesystem::path::preferred_separator;

	char buf[100];
	strftime(buf, sizeof(buf), "%d_%m_%Y-%H_%M_%S", &timeinfo);

	p += group;
	p += "_";
	p += buf;
	p += ".json";

	std::ofstream of(p, std::ofstream::out);
	of << v.toStyledString().c_str();
	of.close();
}

