#include "Utils.hpp"
#include "System.hpp"

#include <drogon/drogon.h>

#include <fstream>
#include <filesystem>

#include <date.h>

using namespace drogon;

static constexpr const char alphanum[] =
	"0123456789"
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz";

static trantor::LogStream& operator<<(trantor::LogStream& stream, const std::unordered_map<std::string, std::string, drogon::utils::internal::SafeStringHash>& map)
{
	for (const auto& [k, v] : map)
	{
		stream << "key=" << k << "\tvalue=" << v << "\n";
	}
	return stream;
}

void Utils::DumpInfoToDrogon(const drogon::HttpRequestPtr& rq, std::string_view ip)
{
	LOG_TRACE << ip << "->request dump: " << rq->getPath() << " (" << rq->getMethodString() << ")\n"
		<< "Cookies:\n" << rq->getCookies()
		<< "Parameters:\n" << rq->getParameters()
		<< "Headers:\n" << rq->getHeaders()
		<< "Body:\n" << rq->getBody()
	;
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

void Utils::AppendJsonReqToFile(std::string_view json, std::string_view group)
{
	if (!System::Instance().SysConfig().log.enable)
		return;

	const auto now = std::chrono::system_clock::now();
	std::ostringstream filename;
	filename << System::Instance().SysConfig().log.request_path
		<< "/"
		<< group
		<< "_";
	date::to_stream(filename, "%d_%m_%Y-%H_%M_%S", now);
	filename << ".json";

	std::ofstream of(filename.str(), std::ofstream::out);
	of << json;
	of.close();
}

void Utils::AppendJsonResToFile(std::string_view json, std::string_view group)
{
	if (!System::Instance().SysConfig().log.enable)
		return;

	const auto now = std::chrono::system_clock::now();
	std::ostringstream filename;
	filename << System::Instance().SysConfig().log.response_path
		<< "/"
		<< group
		<< "_";
	date::to_stream(filename, "%d_%m_%Y-%H_%M_%S", now);
	filename << ".json";

	std::ofstream of(p, std::ofstream::out);
	of << json;
	of.close();
}

void Utils::AddMissingDlcFile(std::string_view file)
{
	if (!System::Instance().SysConfig().log.enable)
		return;

	const auto& p = System::Instance().SysConfig().log.dlc_error_file;	
	std::ofstream of(p, std::ofstream::out | std::ofstream::app);
	of << file << std::endl;
	of.close();
}
