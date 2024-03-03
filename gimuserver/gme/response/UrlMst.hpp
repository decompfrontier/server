#pragma once

#include "../GmeRequest.hpp"

RESPONSE_NS_BEGIN
struct UrlMst : public IResponse
{
	explicit UrlMst() : id(1) {}

	const char* getGroupName() const override { return "At7Gny2V"; }

	uint32_t id;
	std::string officialSite, noticeUrl, contactUrl,
		friendRefeerUrl, appliDlUrl, appliDlAndroidUrl,
		famiAppSiteUrl, twitterSiteUrl, facebookSiteUrl,
		transferSiteUrl,
		appBankSiteUrl,
		loblSiteUrl,
		loblSchemaUrl,
		appliSommelierUrl,
		creditUrl,
		gameGiftUrl,
		agreementUrl,
		agreementOfficialUrl,
		legalfundSettlementUrl,
		specificTradeUrl,
		diaPossessionUrl,
		lobiRecHelpUrl,
		lobiAgreementUrl,
		gachaContentsUrl,
		multiArchiveUrl;

protected:
	void SerializeFields(Json::Value& v, size_t i) const override
	{
		v["ut3nd0yV"] = std::to_string(id);
		v["ME4h3vDn"] = officialSite;
		v["cH0Kdchp"] = noticeUrl;
		v["UMRh8tS2"] = contactUrl;
		v["R4Dk2FYs"] = friendRefeerUrl;
		v["E1oFw94b"] = appliDlUrl;
		v["D0gMyJ3o"] = appliDlAndroidUrl;
		v["UWb9B6sm"] = famiAppSiteUrl;
		v["HcIt7G8y"] = facebookSiteUrl;
		v["13kI62fP"] = twitterSiteUrl;
		v["Rf63vLu7"] = transferSiteUrl;
		v["e4qEPM8j"] = appBankSiteUrl;
		v["t9jT3QKC"] = loblSiteUrl;
		v["Yh6MiF8f"] = loblSchemaUrl;
		v["25oCPAM7"] = appliSommelierUrl;
		v["iVgm9n6v"] = creditUrl;
		v["wT6cF35V"] = gameGiftUrl;
		v["3f9T5d6G"] = agreementUrl;
		v["fNud6c7r"] = agreementOfficialUrl;
		v["z6q1aUXE"] = legalfundSettlementUrl;
		v["AZ2mCbU1"] = specificTradeUrl;
		v["9Nrt0SPY"] = diaPossessionUrl;
		v["tK24myuj"] = lobiRecHelpUrl;
		v["6Qj8YZtE"] = lobiAgreementUrl;
		v["qlO1kVZZ"] = gachaContentsUrl;
		v["R10T3gFJ"] = multiArchiveUrl;
	}
};
RESPONSE_NS_END
