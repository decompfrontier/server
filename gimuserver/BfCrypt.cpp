#include "BfCrypt.hpp"

#include <json/writer.h>

#include <trantor/utils/Logger.h>

#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/base64.h>

constexpr auto SREE_KEY = "7410958164354871";
constexpr auto SREE_IV = "Bfw4encrypedPass";

using namespace CryptoPP;

std::string BfCrypt::CryptSREE(const Json::Value& v)
{
	Json::StreamWriterBuilder b;
	b["indentation"] = "";
	b["commentStyle"] = "None";
	b["emitUTF8"] = true;

	const auto str = Json::writeString(b, v);

	std::string tmp = "", output = "";

	try
	{
		CBC_Mode<AES>::Encryption e;
		e.SetKeyWithIV((const byte*)SREE_KEY, strlen(SREE_KEY), (byte*)SREE_IV);
		StringSource ss(str, true, new StreamTransformationFilter(e, new StringSink(tmp), StreamTransformationFilter::NO_PADDING));

		Base64Encoder b64;
		b64.Put((const byte*)tmp.data(), tmp.size());
		b64.MessageEnd();

		auto len = b64.MaxRetrievable();
		if (len)
		{
			output.resize(len);
			b64.Get((byte*)&output[0], output.size());
		}
	}
	catch (const Exception& ex)
	{
		LOG_ERROR << "Unable to crypt SREE request: " << ex.what();
		output = "";
	}

	return output;
}
