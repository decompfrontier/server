#pragma once

#include <json/value.h>
#include <unordered_map>

class GmeCoder
{
public:
	inline const std::string& GetFromDecName(const std::string& dec) const { return m_data_crypt.at(dec); }
	inline const std::string& GetFromEncName(const std::string& enc) const { return m_data_decrypt.at(enc); }
	inline const std::string& GetAESKeyByEncName(const std::string& req) const { return m_keys.at(m_reqinfo_decrypt.at(req)); }
	inline const std::string& GetAESKeyByDecName(const std::string& req) const { return m_keys.at(req); }
	inline const std::string& GetRequestNameFromEncName(const std::string& rq) const { return m_reqinfo_decrypt.at(rq); }
	inline const std::string& GetRequestNameFromDecName(const std::string& rq) const { return m_reqinfo_crypt.at(rq); }
	void Init();

	static GmeCoder& Instance() { return m_coder; }

private:
	void SetupFields();
	void SetupKeys();
	inline void SetupKey(const std::string& dec_req, const std::string& enc_req, const std::string& key)
	{
		m_keys.insert_or_assign(dec_req, key);
		m_reqinfo_crypt.insert_or_assign(dec_req, enc_req);
		m_reqinfo_decrypt.insert_or_assign(enc_req, dec_req);
	}

	inline void SetupField(const std::string& dec, const std::string& enc)
	{
		m_data_crypt.insert_or_assign(dec, enc);
		m_data_decrypt.insert_or_assign(enc, dec);
	}

	std::unordered_map<std::string, std::string> m_data_crypt;
	std::unordered_map<std::string, std::string> m_data_decrypt;

	std::unordered_map<std::string, std::string> m_reqinfo_crypt;
	std::unordered_map<std::string, std::string> m_reqinfo_decrypt;
	std::unordered_map<std::string, std::string> m_keys;

	static GmeCoder m_coder;
};

#define JENC(x) GmeCoder::Instance().GetFromDecName(x)
