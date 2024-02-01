#include "BfCrypt.hpp"

#include <json/json.h>

#include <trantor/utils/Logger.h>

#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>

#include <drogon/utils/Utilities.h>

static const unsigned char SREE_KEY[] = { 0x37, 0x34, 0x31, 0x30, 0x39, 0x35, 0x38, 0x31, 0x36, 0x34, 0x33, 0x35, 0x34, 0x38, 0x37, 0x31 }; // 7410958164354871
static const unsigned char SREE_IV[] = { 0x42, 0x66, 0x77, 0x34, 0x65, 0x6E, 0x63, 0x72, 0x79, 0x70, 0x65, 0x64, 0x50, 0x61, 0x73, 0x73 }; // Bfw4encrypedPass

using namespace CryptoPP;

std::string BfCrypt::CryptSREE(const Json::Value& v)
{
	try
	{
		Json::StreamWriterBuilder b;
		b["indentation"] = "";
		b["commentStyle"] = "None";
		b["emitUTF8"] = true;

		auto str = Json::writeString(b, v);

		std::string tmp = "", output = "";

		CBC_Mode<AES>::Encryption e;
		e.SetKeyWithIV(SREE_KEY, sizeof(SREE_KEY), SREE_IV);

		StringSource ss(str, true, new StreamTransformationFilter(e, new StringSink(tmp), StreamTransformationFilter::ZEROS_PADDING));

		return drogon::utils::base64Encode((const unsigned char*)tmp.data(), tmp.size());
	}
	catch (const Exception& ex)
	{
		LOG_ERROR << "Unable to crypt SREE request: " << ex.what();
		return "";
	}
}

std::string BfCrypt::CryptGME(const Json::Value& v, const std::string& key)
{
	try
	{
		Json::StreamWriterBuilder b;
		b["indentation"] = "";
		b["commentStyle"] = "None";
		b["emitUTF8"] = true;

		auto str = Json::writeString(b, v);

		ECB_Mode<AES>::Encryption e;
		byte aeskey[AES::MIN_KEYLENGTH] = { 0x00 };
		// keys are setted this way because we know that some keys might not have the minimum requirement
#ifdef _MSC_VER
		memcpy_s(aeskey, _countof(aeskey), key.data(), std::min(key.size(), (size_t)sizeof(aeskey)));
#else
		memcpy(aeskey, key.data(), std::min(key.size(), (size_t)sizeof(aeskey)));
#endif
		e.SetKey(aeskey, sizeof(aeskey));

		std::string tmp;
		StringSource ss(str, true, new StreamTransformationFilter(e, new StringSink(tmp), StreamTransformationFilter::PKCS_PADDING));

		return drogon::utils::base64Encode((const unsigned char*)tmp.data(), tmp.size());
	}
	catch (const Exception& ex)
	{
		LOG_ERROR << "Unable to crypt GME request: " << ex.what();
		return "";
	}
}

void BfCrypt::DecryptGME(const std::string& in, const std::string& key, Json::Value& v)
{
	if (key.empty() || in.empty())
		return;

	try
	{
		std::string tmp = drogon::utils::base64Decode(in);

		ECB_Mode<AES>::Decryption e;
		byte aeskey[AES::MIN_KEYLENGTH] = { 0x00 };
#ifdef _MSC_VER
		memcpy_s(aeskey, _countof(aeskey), key.data(), std::min(key.size(), (size_t)sizeof(aeskey)));
#else
		memcpy(aeskey, key.data(), std::min(key.size(), (size_t)sizeof(aeskey)));
#endif
		e.SetKey(aeskey, sizeof(aeskey));

		std::string output;
		StringSource ss(tmp, true,
			new StreamTransformationFilter(e,
				new StringSink(output)
			) // StreamTransformationFilter
		); // StringSource

		Json::Reader r;
		Json::Value root;

		if (!r.parse(output, root))
			return;

		v = root;
	}
	catch (const Exception& ex)
	{
		LOG_ERROR << "Unable to decrypt GME request: " << ex.what();
		return;
	}
}
