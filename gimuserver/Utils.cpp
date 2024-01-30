#include "Utils.hpp"

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
	LOG_TRACE << ip << "->request dump: " << rq->getPath() << "(" << rq->getMethodString() << ")\n"
		<< "Cookies:\n" << rq->getCookies()
		<< "Headers:\n" << rq->getHeaders()
		<< "Body:\n" << rq->getBody().data()
	;
}
