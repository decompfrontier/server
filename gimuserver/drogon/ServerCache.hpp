#pragma once

/*!
* Cache of the server
*/
class ServerCache final : public trantor::NonCopyable
{
public:
	/*!
	* Setup the server cache
	* @param[in] serverObj Configuration object of the plugin
	*/
	void Setup(const Json::Value& serverObj);

	/*!
	* Gets the DLS JSON
	* @return DLS string
	*/
	inline const auto& dls() const { return m_dls; }

private:
	/*!
	* DLS cached JSON
	*/
	std::string m_dls;
};
