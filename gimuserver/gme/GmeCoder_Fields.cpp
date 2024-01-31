#include "GmeCoder.hpp"

// Sets up the field cryptation/decryptation
void GmeCoder::SetupFields()
{
	// gme exchange data
	SetupField("header", "F4q6i9xe");
	SetupField("body", "a3vSYuq2");
	SetupField("error", "b5PH6mZa");
	// gme exchange header
	SetupField("client_id", "aV6cLn3v");
	SetupField("request_id", "Hhgi79M1");
	SetupField("?_0", "4wij8ArG");
	SetupField("?_1", "j0Uszek2");
	// gme exchange error data
	SetupField("error_id", "3e9aGpus");
	SetupField("error_continue_operation", "iPD12YCr");
	SetupField("error_message", "ZC0msu2L");
	SetupField("?_2", "zcJeTx18");
	// gme exchange data
	SetupField("encrypted_json", "Kn51uR4Y");
	// initialize2 request / general data
	SetupField("device_ad_id", "6K7LToCD");
	SetupField("current_language", "h7LYasNK");
	SetupField("country_code", "hceYTcAK");
	SetupField("facebook_userid", "hvOa4oZ7");
	SetupField("target_os", "DFY3k6qp");
	SetupField("device_name", "iN7buP0j");
	SetupField("device_id", "Ma5GnU0H");
	SetupField("basic_info", "IKqx1Cn9");
	// url mst request
	SetupField("mst_data", "KeC10fuL");
	SetupField("mst_name", "moWQ30GH");
	SetupField("mst_local_version", "d2RFtP8T");
	SetupField("account_token", "iN7buP1i");
	// "nrg19RGe"
	// "90LWtVUN"
	// "K29dp2Q"
	// "h7eY3sAK"
	// "B5JQyV8j"
	// "LZ2PfNm4"
	// "4WSu1irc"
	// "iN7buP2h"
}
