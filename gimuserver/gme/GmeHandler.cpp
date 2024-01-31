#include "GmeHandler.hpp"

GmeHandler::GmeHandler()
	: m_errID(ErrorID::No)
	, m_errOP(ErrorOperation::Continue)
{
	InitHandlers();
}

bool GmeHandler::Handle(std::string req, const Json::Value& body)
{
	auto it = m_handlers.find(req);

	if (it == m_handlers.end())
	{
		m_errID = ErrorID::Yes;
		m_errOP = ErrorOperation::Close;
		m_errMsg = "Unsupported method! Please report this bug with the following information:\nRequest: " + req;
		return false;
	}

	return (this->*it->second)(body);
}

void GmeHandler::InitHandlers()
{

}
