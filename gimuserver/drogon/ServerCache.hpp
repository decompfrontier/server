#pragma once

/*!
* Simple holder of readed JSON files
*/
class ServerCache final : public trantor::NonCopyable
{
public:
	/*!
	* Setup the server cache
	* @param[in] serverObj Configuration object of the plugin
	*/
	void Setup(const Json::Value& serverObj);

private:
	/*!
	* DLS 
	*/
	std::string m_dls;
};
