#pragma once

#include "GmeTypes.hpp"

#include <unordered_map>
#include <json/value.h>

class GmeHandler;
typedef bool(GmeHandler::*GmeHandlerFnc)(const Json::Value& body);

class GmeHandler
{
public:
	GmeHandler();

	auto GetErrorId() const { return m_errID; }
	auto GetErrorContinueOp() const { return m_errOP; }
	const auto& GetErrorMsg() const { return m_errMsg; }
	const auto& GetSuccessData() const { return m_out; }

	bool Handle(std::string req, const Json::Value& body);

private:
	void InitHandlers();
	inline void Register(std::string name, GmeHandlerFnc fnc) { m_handlers.insert_or_assign(name, fnc); }

	std::string m_errMsg;
	ErrorID m_errID;
	ErrorOperation m_errOP;
	Json::Value m_out;
	std::unordered_map<std::string, GmeHandlerFnc> m_handlers;

	// handlers
	
};
