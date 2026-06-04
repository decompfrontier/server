#include "App.hpp"
#include "Handlers.hpp"
#include "common.hpp"

#include <gimuserver/archive/UnitArchiver.hpp>
#include <gimuserver/db/PacketInterface.hpp>
#include <gimuserver/db/UserInfoService.hpp>
#include <gimuserver/db/UserUnitService.hpp>
#include <gimuserver/utils/Random.hpp>

#include <cstdint>
#include <optional>
#include <string>

namespace
{
constexpr uint32_t TutorialStarterUnitType = 1;

std::optional<uint32_t> getStarterUnit(uint32_t element)
{
	switch (element)
	{
	case 1:
		return 10011;
	case 2:
		return 20011;
	case 3:
		return 30011;
	case 4:
		return 40011;
	default:
		return std::nullopt;
	}
}
}

HANDLEF(NgwordCheck)
{
	co_return HandleResult::success("{}");
}

HANDLEF(CreateUser)
{
	CreateUserReq req = {};
	const auto& ec = glz::read_json(req, json);
	if (ec)
	{
		const auto error = glz::format_error(ec, json);
		LOG_ERROR << "CreateUserReq deserialization failed:\n"
			<< error << "\n"
			<< "Raw decrypted request JSON:\n"
			<< json;
		co_return HandleResult::error("Deserialization error", error);
	}

	const auto& handleName = req.login_info.handle_name;
	const auto& gumiUserId = req.login_info.gumi_live_userid;
	if (gumiUserId.empty())
	{
		co_return HandleResult::error(
			"Missing gumi_live_userid",
			"CreateUser request did not include gumi_live_userid");
	}
	if (handleName.empty())
	{
		co_return HandleResult::error(
			"Missing handle name",
			"CreateUser request did not include a handle name");
	}

	const auto starterUnitId = getStarterUnit(req.selected_element.element);
	if (!starterUnitId)
	{
		co_return HandleResult::error("Invalid tutorial starter element");
	}

	const auto unitRecord = UnitArchiver::instance().lookup(*starterUnitId);
	if (!unitRecord)
	{
		co_return HandleResult::error("Archive error", "Unable to find tutorial starter unit");
	}

	UserUnitInfo unit;
	if (!UnitArchiver::populatePacket(*unitRecord, TutorialStarterUnitType, unit))
	{
		co_return HandleResult::error("Archive error", "Unable to populate tutorial starter unit");
	}

	// GuestLogin should have established the Gumi Live user before CreateUser runs.
	std::string storedGumiUserId;
	CO_AWAIT_DB(gme::nonEmpty(UserInfoService::fetchCurrentGumiUser(
		theDb(),
		storedGumiUserId)));
	if (storedGumiUserId != gumiUserId)
	{
		co_return HandleResult::error(
			"Gumi Live user mismatch",
			"CreateUser gumi_live_userid does not match the active Gumi Live user");
	}

	std::string userId;
	CO_AWAIT_DB(UserInfoService::fetchUserForGumiUser(theDb(), gumiUserId, userId));
	if (!userId.empty())
	{
		co_return HandleResult::error(
			"User already exists",
			"CreateUser cannot create a second user for this Gumi Live user");
	}

	userId = RandomId();

	// We need to wrap these operations in a transaction to avoid an invalid
	// intermediate state.
	CO_AWAIT_DB(gme::runTransaction(
		theDb(),
		// Add the user in the userinfo table.
		[&](const auto transaction)
		{
			return gme::nonEmpty(UserInfoService::addUser(
				transaction,
				gumiUserId,
				userId,
				handleName));
		},
		// Update the tutorial status to mark as completed.
		[&](const auto transaction)
		{
			return gme::nonEmpty(PacketInterfaceFor<LoginInfoResp>::updateFromPacket(
				transaction,
				"userinfo",
				{
					{ "gumi_user_id", gumiUserId },
					{ "id", userId },
				},
				LoginInfoResp{ .tutorial_status = 12 },
				{ "tutorial_status" }));
		},
		// Add the starter unit to the user's collection.
		[&](const auto transaction)
		{
			return gme::nonEmpty(UserUnitService::addUnit(
				transaction,
				userId,
				unit));
		}));

	co_return HandleResult::success("{}");
}

HANDLEF(TutorialUpdate)
{
	co_return HandleResult::success("{}");
}
