#include "BfCrypt.hpp"

#include <trantor/utils/Logger.h>

#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>

#include <drogon/utils/Utilities.h>

static constexpr const unsigned char SREE_KEY[] = { 0x37, 0x34, 0x31, 0x30, 0x39, 0x35, 0x38, 0x31, 0x36, 0x34, 0x33, 0x35, 0x34, 0x38, 0x37, 0x31 }; // 7410958164354871
static constexpr const unsigned char SREE_IV[] = { 0x42, 0x66, 0x77, 0x34, 0x65, 0x6E, 0x63, 0x72, 0x79, 0x70, 0x65, 0x64, 0x50, 0x61, 0x73, 0x73 }; // Bfw4encrypedPass

using namespace CryptoPP;

std::optional<SREE> BfCrypt::BuildSREE(std::string_view input_json)
{
	try
	{
		std::string str = input_json.data();

		// add padding
		int p = str.size() % AES::BLOCKSIZE;
		for (int i = 0; i < (AES::BLOCKSIZE - p); i++)
			str += " ";

		std::string tmp = "", output = "";

		CBC_Mode<AES>::Encryption e;
		e.SetKeyWithIV(SREE_KEY, sizeof(SREE_KEY), SREE_IV);

		StringSource ss(str, true, new StreamTransformationFilter(e, new StringSink(tmp), StreamTransformationFilter::NO_PADDING));

		SREE sree{ drogon::utils::base64Encode((const unsigned char*)tmp.data(), tmp.size()) };
		return sree;
	}
	catch (const Exception& ex)
	{
		LOG_ERROR << "Unable to crypt SREE request: " << ex.what();
		return std::nullopt;
	}
}

std::optional<GmeBody> BfCrypt::BuildGME(std::string_view input_json, std::string_view key)
{
	try
	{
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
		StringSource ss(input_json.data(), true, new StreamTransformationFilter(e, new StringSink(tmp), StreamTransformationFilter::PKCS_PADDING));

		GmeBody body{ drogon::utils::base64Encode((const unsigned char*)tmp.data(), tmp.size()) };
		return body;
	}
	catch (const Exception& ex)
	{
		LOG_ERROR << "Unable to crypt GME request: " << ex.what();
		return std::nullopt;
	}
}

std::optional<std::string> BfCrypt::ReadGME(const GmeAction& root, std::string_view key)
{
	if (key.empty() || !root.body.has_value())
		return std::nullopt;

	const GmeBody& body = root.body.value();
	if (body.body.empty())
		return std::nullopt;

	try
	{
		std::string tmp = drogon::utils::base64Decode(body.body);

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

		return output;
	}
	catch (const Exception& ex)
	{
		LOG_ERROR << "Unable to decrypt GME request: " << ex.what();
		return std::nullopt;
	}
}
