//#include "handlers/NgwordCheckRequestHandler.hpp"
//#include "handlers/CreateUserRequestHandler.hpp"
//#include "handlers/InitializeRequestHandler.hpp"
//#include "handlers/TutorialUpdateRequestHandler.hpp"
//#include "handlers/URLMstRequestHandler.hpp"
#include "GmeController.hpp"
#include "handlers/BundlePurchaseRequestHandler.hpp"
#include "handlers/BundleCategoryRefreshRequestHandler.hpp"
#include "handlers/BundlePurchaseIapRequestHandler.hpp"
#include "handlers/SummonerJournalTaskRewardsRequestHandler.hpp"
#include "handlers/SummonerJournalInfoRequestHandler.hpp"
#include "handlers/SummonerJournalMilestoneRewardsRequestHandler.hpp"
#include "handlers/ChallengeArenaStartRequestHandler.hpp"
#include "handlers/ChallengeArenaShopAddItemRequestHandler.hpp"
#include "handlers/ChallengeArenaShopInfoRequestHandler.hpp"
#include "handlers/ChallengeArenaResetInfoRequestHandler.hpp"
#include "handlers/ChallengeArenaTChestReceivedRequestHandler.hpp"
#include "handlers/ChallengeArenaRankingRequestHandler.hpp"
#include "handlers/ChallengeArenaRankingRewardRequestHandler.hpp"
#include "handlers/ChallengeArenaBattleEndRequestHandler.hpp"
#include "handlers/ChallengeArenaTeamEditRequestHandler.hpp"
#include "handlers/ChallengeArenaBattleStartRequestHandler.hpp"
#include "handlers/DailyRewardClaimRequestHandler.hpp"
#include "handlers/TierTourneyClaimRewardRequestHandler.hpp"
#include "handlers/TierTourneyFriendListRequestHandler.hpp"
#include "handlers/TierTourneyRewardMstRequestHandler.hpp"
#include "handlers/UserTournamentInfoRequestHandler.hpp"
#include "handlers/VortexArenaTopRequestHandler.hpp"
#include "handlers/VortexArenaBattleStartRequestHandler.hpp"
#include "handlers/VortexArenaMatchingRequestHandler.hpp"
#include "handlers/VortexArenaBattleEndRequestHandler.hpp"
#include "handlers/VortexArenaRetireRequestHandler.hpp"
#include "handlers/VortexArenaLeaderboardRequestHandler.hpp"
#include "handlers/VortexArenaClaimDailyRequestHandler.hpp"
#include "handlers/VortexArenaClaimRequestHandler.hpp"
#include "handlers/DailyTaskClaimRewardRequestHandler.hpp"
#include "handlers/AchievementAcceptRequestHandler.hpp"
#include "handlers/AchievementDeliverRequestHandler.hpp"
#include "handlers/AchievementRewardReceiveRequestHandler.hpp"
#include "handlers/AchievementTradeRequestHandler.hpp"
#include "handlers/AreaRequestHandler.hpp"
#include "handlers/ArenaBattleEndRequestHandler.hpp"
#include "handlers/ArenaBattleRestartRequestHandler.hpp"
#include "handlers/ArenaBattleStartRequestHandler.hpp"
#include "handlers/ArenaFriendListRequestHandler.hpp"
#include "handlers/ArenaInfoRequestHandler.hpp"
#include "handlers/ArenaMatchingRequestHandler.hpp"
#include "handlers/ArenaRetireRequestHandler.hpp"
#include "handlers/ArenaStartRequestHandler.hpp"
#include "handlers/BadgeInfoRequestHandler.hpp"
#include "handlers/BuyCoinRequestHandler.hpp"
#include "handlers/BuyCoinStartRequestHandler.hpp"
#include "handlers/CampaignReceiptRequestHandler.hpp"
#include "handlers/ChallengeBaseRequestHandler.hpp"
#include "handlers/ChallengeMissionStartRequestHandler.hpp"
#include "handlers/ChallengeRankRewardRequestHandler.hpp"
#include "handlers/ChallengeRankingRequestHandler.hpp"
#include "handlers/ChallengeStartRequestHandler.hpp"
#include "handlers/ChallengeUserInfoRequestHandler.hpp"
#include "handlers/ChronologyRequestHandler.hpp"
#include "handlers/ControlCenterEnterRequestHandler.hpp"
#include "handlers/DebugUpdateRequestHandler.hpp"
#include "handlers/DeckEditRequestHandler.hpp"
#include "handlers/DungeonEventUpdateRequestHandler.hpp"
#include "handlers/DungeonKeyReceiptRequestHandler.hpp"
#include "handlers/DungeonKeyUseRequestHandler.hpp"
#include "handlers/FixGiftInfoRequestHandler.hpp"
#include "handlers/FriendAgreeRequestHandler.hpp"
#include "handlers/FriendApplyRequestHandler.hpp"
#include "handlers/FriendDeleteRequestHandler.hpp"
#include "handlers/FriendFavoriteRequestHandler.hpp"
#include "handlers/FriendGetAgreeRequestHandler.hpp"
#include "handlers/FriendGetRequestHandler.hpp"
#include "handlers/FriendRandomSearchRequestHandler.hpp"
#include "handlers/FriendRefuseRequestHandler.hpp"
#include "handlers/FriendSearchRequestHandler.hpp"
#include "handlers/GachaActionRequestHandler.hpp"
#include "handlers/GachaFixActionRequestHandler.hpp"
#include "handlers/GachaListRequestHandler.hpp"
#include "handlers/GetAchievementInfoRequestHandler.hpp"
#include "handlers/GetDistributeDungeonKeyInfoRequestHandler.hpp"
#include "handlers/GetGiftInfoRequestHandler.hpp"
#include "handlers/GetPlayerInfoRequestHandler.hpp"
#include "handlers/GetScenarioPlayingInfoRequestHandler.hpp"
#include "handlers/GetUserInfoRequestHandler.hpp"
#include "handlers/InitializeRequest2Handler.hpp"
#include "handlers/InvitationReceiptRequestHandler.hpp"
#include "handlers/ItemEditRequestHandler.hpp"
#include "handlers/ItemFavoriteRequestHandler.hpp"
#include "handlers/ItemMixRequestHandler.hpp"
#include "handlers/ItemSellRequestHandler.hpp"
#include "handlers/ItemSphereEqpRequestHandler.hpp"
#include "handlers/ItemUseTimeLimitRequestHandler.hpp"
#include "handlers/MissionContinueRequestHandler.hpp"
#include "handlers/MissionEndRequestHandler.hpp"
#include "handlers/MissionRestartRequestHandler.hpp"
#include "handlers/MissionStartRequestHandler.hpp"
#include "handlers/NoticeListRequestHandler.hpp"
#include "handlers/NoticeReadUpdateRequestHandler.hpp"
#include "handlers/NoticeUpdateRequestHandler.hpp"
#include "handlers/PresentListRequestHandler.hpp"
#include "handlers/PresentReceiptRequestHandler.hpp"
#include "handlers/SPChallengeMissionStartRequestHandler.hpp"
#include "handlers/SPChallengeRankingRequestHandler.hpp"
#include "handlers/SPChallengeUserInfoRequestHandler.hpp"
#include "handlers/SetPurchaseInfoRequestHandler.hpp"
#include "handlers/ShopUseRequestHandler.hpp"
#include "handlers/TieupCampaignInfoRequestHandler.hpp"
#include "handlers/TownFacilityUpdateRequestHandler.hpp"
#include "handlers/TownUpdateRequestHandler.hpp"
#include "handlers/TransferCodeCheckRequestHandler.hpp"
#include "handlers/TransferCodeIssueRequestHandler.hpp"
#include "handlers/TransferRequestHandler.hpp"
#include "handlers/TrialDeckEditRequestHandler.hpp"
#include "handlers/TrialDeckGetRequestHandler.hpp"
#include "handlers/UnitEvoRequestHandler.hpp"
#include "handlers/UnitFavoriteRequestHandler.hpp"
#include "handlers/UnitMixRequestHandler.hpp"
#include "handlers/UnitSellRequestHandler.hpp"
#include "handlers/UpdateEventInfoRequestHandler.hpp"
#include "handlers/UpdateInfoLightRequestHandler.hpp"
#include "handlers/UpdateInfoRequestHandler.hpp"
#include "handlers/UpdatePermitPlaceInfoRequestHandler.hpp"
#include "handlers/CampaignBattleEndRequestHandler.hpp"
#include "handlers/CampaignBattleStartRequestHandler.hpp"
#include "handlers/CampaignDeckEditRequestHandler.hpp"
#include "handlers/CampaignDeckGetRequestHandler.hpp"
#include "handlers/CampaignEndRequestHandler.hpp"
#include "handlers/CampaignItemEditRequestHandler.hpp"
#include "handlers/CampaignMissionGetRequestHandler.hpp"
#include "handlers/CampaignRestartRequestHandler.hpp"
#include "handlers/CampaignSaveRequestHandler.hpp"
#include "handlers/CampaignStartRequestHandler.hpp"
#include "handlers/ColosseumBattleEndRequestHandler.hpp"
#include "handlers/ColosseumBattleRestartRequestHandler.hpp"
#include "handlers/ColosseumBattleStartRequestHandler.hpp"
#include "handlers/ColosseumDeckEditRequestHandler.hpp"
#include "handlers/ColosseumFriendListRequestHandler.hpp"
#include "handlers/ColosseumGetRewardInfoRequestHandler.hpp"
#include "handlers/ColosseumRewardReceiptRequestHandler.hpp"
#include "handlers/FrontierGateContinueRequestHandler.hpp"
#include "handlers/FrontierGateEndRequestHandler.hpp"
#include "handlers/FrontierGateInfoRequestHandler.hpp"
#include "handlers/FrontierGateRankingRequestHandler.hpp"
#include "handlers/FrontierGateRestartRequestHandler.hpp"
#include "handlers/FrontierGateRetryRequestHandler.hpp"
#include "handlers/FrontierGateSaveRequestHandler.hpp"
#include "handlers/FrontierGateStartRequestHandler.hpp"
#include "handlers/MinigameCardEndRequestHandler.hpp"
#include "handlers/MinigameFriendListRequestHandler.hpp"
#include "handlers/MinigameNameEntryRequestHandler.hpp"
#include "handlers/MinigameRankingInfoRequestHandler.hpp"
#include "handlers/MinigameSmashEndRequestHandler.hpp"
#include "handlers/MinigameStartRequestHandler.hpp"
#include "handlers/RaidAgreementRequestHandler.hpp"
#include "handlers/RaidCampRestEndRequestHandler.hpp"
#include "handlers/RaidCampRestGetRequestHandler.hpp"
#include "handlers/RaidCampRestStartRequestHandler.hpp"
#include "handlers/RaidEnterRequestHandler.hpp"
#include "handlers/RaidExitRequestHandler.hpp"
#include "handlers/RaidFrlendSelectRequestHandler.hpp"
#include "handlers/RaidGetChatLogRequestHandler.hpp"
#include "handlers/RaidGetPlayerInfoRequestHandler.hpp"
#include "handlers/RaidItemDeleteRequestHandler.hpp"
#include "handlers/RaidItemEditRequestHandler.hpp"
#include "handlers/RaidItemMixRequestHandler.hpp"
#include "handlers/RaidLimitedUseItemRequestHandler.hpp"
#include "handlers/RaidMissionBattleEndRequestHandler.hpp"
#include "handlers/RaidMissionBattleRestartRequestHandler.hpp"
#include "handlers/RaidMissionBattleStartRequestHandler.hpp"
#include "handlers/RaidMissionEndRequestHandler.hpp"
#include "handlers/RaidMissionExitRequestHandler.hpp"
#include "handlers/RaidMissionHarvestRequestHandler.hpp"
#include "handlers/RaidMissionInfoLightRequestHandler.hpp"
#include "handlers/RaidMissionInfoRequestHandler.hpp"
#include "handlers/RaidMissionSelectRequestHandler.hpp"
#include "handlers/RaidMissionStartRequestHandler.hpp"
#include "handlers/RaidMissionUseItemRequestHandler.hpp"
#include "handlers/RaidQuickStartRequestHandler.hpp"
#include "handlers/RaidRoomDissolveRequestHandler.hpp"
#include "handlers/RaidRoomFriendGetRequestHandler.hpp"
#include "handlers/RaidRoomInfoRequestHandler.hpp"
#include "handlers/RaidRoomKickRequestHandler.hpp"
#include "handlers/RaidRoomListRequestHandler.hpp"
#include "handlers/RaidRoonItemEditRequestHandler.hpp"
#include "handlers/RaidSendChatRequestHandler.hpp"
#include "handlers/RaidShopUseRequestHandler.hpp"
#include "handlers/RaidUpDataRoomInfoRequestHandler.hpp"
#include "handlers/RaidUpDataUserReadyRequestHandler.hpp"
#include "handlers/RaidUpDateCashRequestHandler.hpp"
#include "handlers/RaidUpScenarioInfoRequestHandler.hpp"
#include "handlers/RaidUpdateBlackListRequestHandler.hpp"
#include "handlers/RaidUpdateProgressRequestHandler.hpp"
#include "handlers/RaidUserInfoRequestHandler.hpp"
#include "handlers/RaidWorloInfoRequestHandler.hpp"
#include "handlers/SlotActionRequestHandler.hpp"
#include "handlers/SummonerMixRequestHandler.hpp"
#include "handlers/SummonerSkillGetRequestHandler.hpp"
#include "handlers/UserSummonerInfoEditRequestHandler.hpp"
#include "handlers/FeSkillGetRequestHandler.hpp"
#include "handlers/CollabRewardRequestHandler.hpp"
#include "handlers/EventTokenExchangeInfoRequestHandler.hpp"
#include "handlers/EventTokenExchangePurchaseRequestHandler.hpp"
#include "handlers/EventTokenInfoRequestHandler.hpp"
#include "handlers/FGPlusGuildRankingRequestHandler.hpp"
#include "handlers/FGPlusGuildRewardClaimRequestHandler.hpp"
#include "handlers/GRGuardianDetailHandler.hpp"
#include "handlers/GachaActionRequest_SGHandler.hpp"
#include "handlers/GuildBattleScoreInfoRequestHandler.hpp"
#include "handlers/GuildBoardInfoRequestHandler.hpp"
#include "handlers/GuildBoardPostRequestHandler.hpp"
#include "handlers/GuildCreateRequestHandler.hpp"
#include "handlers/GuildDeckActivateRequestHandler.hpp"
#include "handlers/GuildDeckEditRequestHandler.hpp"
#include "handlers/GuildGuardianSummonContributeRequestHandler.hpp"
#include "handlers/GuildInfoRequestHandler.hpp"
#include "handlers/GuildInviteManageRequestHandler.hpp"
#include "handlers/GuildJoinRequestHandler.hpp"
#include "handlers/GuildJoinedListRequestHandler.hpp"
#include "handlers/GuildMemberGuildInfoRequestHandler.hpp"
#include "handlers/GuildMemberUpdateRequestHandler.hpp"
#include "handlers/GuildRaidBattleInfoRequestHandler.hpp"
#include "handlers/GuildRaidBattleLogListRequestHandler.hpp"
#include "handlers/GuildRaidBattleLogSetRequestHandler.hpp"
#include "handlers/GuildRaidBattleOutpostRelocateRequestHandler.hpp"
#include "handlers/GuildRaidBattleProgressRequestHandler.hpp"
#include "handlers/GuildRaidFriendSelectRequestHandler.hpp"
#include "handlers/GuildRaidGuardSetupRequestHandler.hpp"
#include "handlers/GuildRaidInfoRequestHandler.hpp"
#include "handlers/GuildRaidJoinRoomRequestHandler.hpp"
#include "handlers/GuildRaidMemberDeployRequestHandler.hpp"
#include "handlers/GuildRaidMissionBattleEndRequestHandler.hpp"
#include "handlers/GuildRaidMissionBattleStartRequestHandler.hpp"
#include "handlers/GuildRaidRankingResultRequestHandler.hpp"
#include "handlers/GuildRaidRoomBattleStartRequestHandler.hpp"
#include "handlers/GuildRaidRoomCreateRequestHandler.hpp"
#include "handlers/GuildRaidRoomInfoRequestHandler.hpp"
#include "handlers/GuildRaidRoomListRequestHandler.hpp"
#include "handlers/GuildRaidRoomMemberKickRequestHandler.hpp"
#include "handlers/GuildRaidRoomPartSkillSetRequestHandler.hpp"
#include "handlers/GuildRaidRoomPartSkillListRequestHandler.hpp"
#include "handlers/GuildRaidRoomUpdatedRequestHandler.hpp"
#include "handlers/GuildRankingDetailRequestHandler.hpp"
#include "handlers/GuildRankingRequestHandler.hpp"
#include "handlers/GuildPreviousRankingRequestHandler.hpp"
#include "handlers/GuildRecomendedMemberRequestHandler.hpp"
#include "handlers/GuildResearchInfoRequestHandler.hpp"
#include "handlers/GuildRoundBattleSummaryRequestHandler.hpp"
#include "handlers/GuildRoundSumarryRequestHandler.hpp"
#include "handlers/GuildSearchRequestHandler.hpp"
#include "handlers/GuildShopUseRequestHandler.hpp"
#include "handlers/GuildTradeRequestHandler.hpp"
#include "handlers/GuildUpdateRequestHandler.hpp"
#include "handlers/GuildUpgradeContRequestHandler.hpp"
#include "handlers/GuildUpgradeRequestHandler.hpp"
#include "handlers/GuilldBattleMoveRequestHandler.hpp"
#include "handlers/GuildCampRequestHandler.hpp"
#include "handlers/HomeInfoRequestHandler.hpp"
#include "handlers/InboxMessageManageRequestHandler.hpp"
#include "handlers/MultiGachaIapPurchaseIapRequestHandler.hpp"
#include "handlers/MysteryBoxClaimRequestHandler.hpp"
#include "handlers/MysteryBoxListRequestHandler.hpp"
#include "handlers/RaidFriendRemoveRequestHandler.hpp"
#include "handlers/SGChatLogInfoListRequestHandler.hpp"
#include "handlers/TutorialSkipRequestHandler.hpp"
#include "handlers/UnitOmniEvoRequestHandler.hpp"
#include "handlers/UserEnteredFeatureRequestHandler.hpp"
#include "handlers/UserGemShardInfoRequestHandler.hpp"
#include "handlers/UserLoginCampaignInfoRequestHandler.hpp"
#include "handlers/VideoAdCompleteRequestHandler.hpp"
#include "handlers/VideoAdSlotsClaimBonusRequestHandler.hpp"
#include "handlers/VideoAdSlotsCompleteRequestHandler.hpp"
#include "handlers/VideoAdSlotsStandInfoRefreshRequestHandler.hpp"
#include "handlers/VideoAdsSlotActionRequestHandler.hpp"
#include "handlers/DbbBondRequestHandler.hpp"
#include "handlers/UnitBondBoostRequestHandler.hpp"
#include "handlers/BannerClickRequestHandler.hpp"
#include "handlers/DailyLoginRequestHandler.hpp"
#include "handlers/UnitSelectorGachaTicketRequestHandler.hpp"

#define REGISTER(name) InitializeHandler(std::make_shared<Handler::##name##Handler>())

void GmeController::InitializeHandlers()
{
    //REGISTER(NgwordCheckRequest);                 //Checks the users name is appropriate on game first play flag + Associated to name assignment
    //REGISTER(CreateUserRequest);                  //Creates a new user every time called, disabled for testing purposes
    //REGISTER(InitializeRequest);                  //Disabled due to duplicate class. Refer to Initialize Request 2 below
    //REGISTER(TutorialUpdateRequest);
    //REGISTER(URLMstRequest);
    REGISTER(BundlePurchaseRequest);
    REGISTER(BundleCategoryRefreshRequest);
    REGISTER(BundlePurchaseIapRequest);
    REGISTER(SummonerJournalTaskRewardsRequest);
    REGISTER(SummonerJournalInfoRequest);
    REGISTER(SummonerJournalMilestoneRewardsRequest);
    REGISTER(ChallengeArenaStartRequest);
    REGISTER(ChallengeArenaShopAddItemRequest);
    REGISTER(ChallengeArenaShopInfoRequest);
    REGISTER(ChallengeArenaResetInfoRequest);
    REGISTER(ChallengeArenaTChestReceivedRequest);
    REGISTER(ChallengeArenaRankingRequest);
    REGISTER(ChallengeArenaRankingRewardRequest);
    REGISTER(ChallengeArenaBattleEndRequest);
    REGISTER(ChallengeArenaTeamEditRequest);
    REGISTER(ChallengeArenaBattleStartRequest);
    REGISTER(DailyRewardClaimRequest);
    REGISTER(TierTourneyClaimRewardRequest);
    REGISTER(TierTourneyFriendListRequest);
    REGISTER(TierTourneyRewardMstRequest);
    REGISTER(UserTournamentInfoRequest);
    REGISTER(VortexArenaTopRequest);
    REGISTER(VortexArenaBattleStartRequest);
    REGISTER(VortexArenaMatchingRequest);
    REGISTER(VortexArenaBattleEndRequest);
    REGISTER(VortexArenaRetireRequest);
    REGISTER(VortexArenaLeaderboardRequest);
    REGISTER(VortexArenaClaimDailyRequest);
    REGISTER(VortexArenaClaimRequest);
    REGISTER(DailyTaskClaimRewardRequest);
    REGISTER(AchievementAcceptRequest);
    REGISTER(AchievementDeliverRequest);
    REGISTER(AchievementRewardReceiveRequest);
    REGISTER(AchievementTradeRequest);
    REGISTER(AreaRequest);
    REGISTER(ArenaBattleEndRequest);
    REGISTER(ArenaBattleRestartRequest);
    REGISTER(ArenaBattleStartRequest);
    REGISTER(ArenaFriendListRequest);
    REGISTER(ArenaInfoRequest);
    REGISTER(ArenaMatchingRequest);
    REGISTER(ArenaRetireRequest);
    REGISTER(ArenaStartRequest);
    REGISTER(BadgeInfoRequest);
    REGISTER(BuyCoinRequest);
    REGISTER(BuyCoinStartRequest);
    REGISTER(CampaignReceiptRequest);
    REGISTER(ChallengeBaseRequest);
    REGISTER(ChallengeMissionStartRequest);
    REGISTER(ChallengeRankRewardRequest);
    REGISTER(ChallengeRankingRequest);
    REGISTER(ChallengeStartRequest);
    REGISTER(ChallengeUserInfoRequest);
    REGISTER(ChronologyRequest);
    REGISTER(ControlCenterEnterRequest);
	REGISTER(DebugUpdateRequest);
    REGISTER(DeckEditRequest);
    REGISTER(DungeonEventUpdateRequest);
    REGISTER(DungeonKeyReceiptRequest);
    REGISTER(DungeonKeyUseRequest);
    REGISTER(FixGiftInfoRequest);
    REGISTER(FriendAgreeRequest);
    REGISTER(FriendApplyRequest);
    REGISTER(FriendDeleteRequest);
    REGISTER(FriendFavoriteRequest);
    REGISTER(FriendGetAgreeRequest);
    REGISTER(FriendGetRequest);
    REGISTER(FriendRandomSearchRequest);
    REGISTER(FriendRefuseRequest);
    REGISTER(FriendSearchRequest);
    REGISTER(GachaActionRequest);
    REGISTER(GachaFixActionRequest);
    REGISTER(GachaListRequest);
    REGISTER(GetAchievementInfoRequest);
    REGISTER(GetDistributeDungeonKeyInfoRequest);
    REGISTER(GetGiftInfoRequest);
    REGISTER(GetPlayerInfoRequest);
    REGISTER(GetScenarioPlayingInfoRequest);
    REGISTER(GetUserInfoRequest);
	REGISTER(InitializeRequest2);
    REGISTER(InvitationReceiptRequest);
    REGISTER(ItemEditRequest);
    REGISTER(ItemFavoriteRequest);
    REGISTER(ItemMixRequest);
    REGISTER(ItemSellRequest);
    REGISTER(ItemSphereEqpRequest);
    REGISTER(ItemUseTimeLimitRequest);
    REGISTER(MissionContinueRequest);
    REGISTER(MissionEndRequest);
    REGISTER(MissionRestartRequest);
    REGISTER(MissionStartRequest);
    REGISTER(NoticeListRequest);
    REGISTER(NoticeReadUpdateRequest);
    REGISTER(NoticeUpdateRequest);
    REGISTER(PresentListRequest);
    REGISTER(PresentReceiptRequest);
    REGISTER(SPChallengeMissionStartRequest);
    REGISTER(SPChallengeRankingRequest);
    REGISTER(SPChallengeUserInfoRequest);
    REGISTER(SetPurchaseInfoRequest);
    REGISTER(ShopUseRequest);
    REGISTER(TieupCampaignInfoRequest);
    REGISTER(TownFacilityUpdateRequest);
    REGISTER(TownUpdateRequest);
    REGISTER(TransferCodeCheckRequest);
    REGISTER(TransferCodeIssueRequest);
    REGISTER(TransferRequest);
    REGISTER(TrialDeckEditRequest);
    REGISTER(TrialDeckGetRequest);
    REGISTER(UnitEvoRequest);
    REGISTER(UnitFavoriteRequest);
    REGISTER(UnitMixRequest);
    REGISTER(UnitSellRequest);
    REGISTER(UpdateEventInfoRequest);
    REGISTER(UpdateInfoLightRequest);
    REGISTER(UpdateInfoRequest);
    REGISTER(UpdatePermitPlaceInfoRequest);
    REGISTER(CampaignBattleEndRequest);
    REGISTER(CampaignBattleStartRequest);
    REGISTER(CampaignDeckEditRequest);
    REGISTER(CampaignDeckGetRequest);
    REGISTER(CampaignEndRequest);
    REGISTER(CampaignItemEditRequest);
    REGISTER(CampaignMissionGetRequest);
    REGISTER(CampaignRestartRequest);
    REGISTER(CampaignSaveRequest);
    REGISTER(CampaignStartRequest);
    REGISTER(ColosseumBattleEndRequest);
    REGISTER(ColosseumBattleRestartRequest);
    REGISTER(ColosseumBattleStartRequest);
    REGISTER(ColosseumDeckEditRequest);
    REGISTER(ColosseumFriendListRequest);
    REGISTER(ColosseumGetRewardInfoRequest);
    REGISTER(ColosseumRewardReceiptRequest);
    REGISTER(FrontierGateContinueRequest);
    REGISTER(FrontierGateEndRequest);
    REGISTER(FrontierGateInfoRequest);
    REGISTER(FrontierGateRankingRequest);
    REGISTER(FrontierGateRestartRequest);
    REGISTER(FrontierGateRetryRequest);
    REGISTER(FrontierGateSaveRequest);
    REGISTER(FrontierGateStartRequest);
    REGISTER(MinigameCardEndRequest);
    REGISTER(MinigameFriendListRequest);
    REGISTER(MinigameNameEntryRequest);
    REGISTER(MinigameRankingInfoRequest);
    REGISTER(MinigameSmashEndRequest);
    REGISTER(MinigameStartRequest);
    REGISTER(RaidAgreementRequest);
    REGISTER(RaidCampRestEndRequest);
    REGISTER(RaidCampRestGetRequest);
    REGISTER(RaidCampRestStartRequest);
    REGISTER(RaidEnterRequest);
    REGISTER(RaidExitRequest);
    REGISTER(RaidFrlendSelectRequest);
    REGISTER(RaidGetChatLogRequest);
    REGISTER(RaidGetPlayerInfoRequest);
    REGISTER(RaidItemDeleteRequest);
    REGISTER(RaidItemEditRequest);
    REGISTER(RaidItemMixRequest);
    REGISTER(RaidLimitedUseItemRequest);
    REGISTER(RaidMissionBattleEndRequest);
    REGISTER(RaidMissionBattleRestartRequest);
    REGISTER(RaidMissionBattleStartRequest);
    REGISTER(RaidMissionEndRequest);
    REGISTER(RaidMissionExitRequest);
    REGISTER(RaidMissionHarvestRequest);
    REGISTER(RaidMissionInfoLightRequest);
    REGISTER(RaidMissionInfoRequest);
    REGISTER(RaidMissionSelectRequest);
    REGISTER(RaidMissionStartRequest);
    REGISTER(RaidMissionUseItemRequest);
    REGISTER(RaidQuickStartRequest);
    REGISTER(RaidRoomDissolveRequest);
    REGISTER(RaidRoomFriendGetRequest);
    REGISTER(RaidRoomInfoRequest);
    REGISTER(RaidRoomKickRequest);
    REGISTER(RaidRoomListRequest);
    REGISTER(RaidRoonItemEditRequest);
    REGISTER(RaidSendChatRequest);
    REGISTER(RaidShopUseRequest);
    REGISTER(RaidUpDataRoomInfoRequest);
    REGISTER(RaidUpDataUserReadyRequest);
    REGISTER(RaidUpDateCashRequest);
    REGISTER(RaidUpScenarioInfoRequest);
    REGISTER(RaidUpdateBlackListRequest);
    REGISTER(RaidUpdateProgressRequest);
    REGISTER(RaidUserInfoRequest);
    REGISTER(RaidWorloInfoRequest);
    REGISTER(SlotActionRequest);
    REGISTER(SummonerMixRequest);
    REGISTER(SummonerSkillGetRequest);
    REGISTER(UserSummonerInfoEditRequest);
    REGISTER(FeSkillGetRequest);
    REGISTER(CollabRewardRequest);
    REGISTER(EventTokenExchangeInfoRequest);
    REGISTER(EventTokenExchangePurchaseRequest);
    REGISTER(EventTokenInfoRequest);
    REGISTER(FGPlusGuildRankingRequest);
    REGISTER(FGPlusGuildRewardClaimRequest);
    REGISTER(GRGuardianDetail);
    REGISTER(GachaActionRequest_SG);
    REGISTER(GuildBattleScoreInfoRequest);
    REGISTER(GuildBoardInfoRequest);
    REGISTER(GuildBoardPostRequest);
    REGISTER(GuildCreateRequest);
    REGISTER(GuildDeckActivateRequest);
    REGISTER(GuildDeckEditRequest);
    REGISTER(GuildGuardianSummonContributeRequest);
    REGISTER(GuildInfoRequest);
    REGISTER(GuildInviteManageRequest);
    REGISTER(GuildJoinRequest);
    REGISTER(GuildJoinedListRequest);
    REGISTER(GuildMemberGuildInfoRequest);
    REGISTER(GuildMemberUpdateRequest);
    REGISTER(GuildRaidBattleInfoRequest);
    REGISTER(GuildRaidBattleLogListRequest);
    REGISTER(GuildRaidBattleLogSetRequest);
    REGISTER(GuildRaidBattleOutpostRelocateRequest);
    REGISTER(GuildRaidBattleProgressRequest);
    REGISTER(GuildRaidFriendSelectRequest);
    REGISTER(GuildRaidGuardSetupRequest);
    REGISTER(GuildRaidInfoRequest);
    REGISTER(GuildRaidJoinRoomRequest);
    REGISTER(GuildRaidMemberDeployRequest);
    REGISTER(GuildRaidMissionBattleEndRequest);
    REGISTER(GuildRaidMissionBattleStartRequest);
    REGISTER(GuildRaidRankingResultRequest);
    REGISTER(GuildRaidRoomBattleStartRequest);
    REGISTER(GuildRaidRoomCreateRequest);
    REGISTER(GuildRaidRoomInfoRequest);
    REGISTER(GuildRaidRoomListRequest);
    REGISTER(GuildRaidRoomMemberKickRequest);
    REGISTER(GuildRaidRoomPartSkillSetRequest);
    REGISTER(GuildRaidRoomPartSkillListRequest);
    REGISTER(GuildRaidRoomUpdatedRequest);
    REGISTER(GuildRankingDetailRequest);
    REGISTER(GuildRankingRequest);
    REGISTER(GuildPreviousRankingRequest);
    REGISTER(GuildRecomendedMemberRequest);
    REGISTER(GuildResearchInfoRequest);
    REGISTER(GuildRoundBattleSummaryRequest);
    REGISTER(GuildRoundSumarryRequest);
    REGISTER(GuildSearchRequest);
    REGISTER(GuildShopUseRequest);
    REGISTER(GuildTradeRequest);
    REGISTER(GuildUpdateRequest);
    REGISTER(GuildUpgradeContRequest);
    REGISTER(GuildUpgradeRequest);
    REGISTER(GuilldBattleMoveRequest);
    REGISTER(GuildCampRequest);
    REGISTER(HomeInfoRequest);
    REGISTER(InboxMessageManageRequest);
    REGISTER(MultiGachaIapPurchaseIapRequest);
    REGISTER(MysteryBoxClaimRequest);
    REGISTER(MysteryBoxListRequest);
    REGISTER(RaidFriendRemoveRequest);
    REGISTER(SGChatLogInfoListRequest);
    REGISTER(TutorialSkipRequest);
    REGISTER(UnitOmniEvoRequest);
    REGISTER(UserEnteredFeatureRequest);
    REGISTER(UserGemShardInfoRequest);
    REGISTER(UserLoginCampaignInfoRequest);
    REGISTER(VideoAdCompleteRequest);
    REGISTER(VideoAdSlotsClaimBonusRequest);
    REGISTER(VideoAdSlotsCompleteRequest);
    REGISTER(VideoAdSlotsStandInfoRefreshRequest);
    REGISTER(VideoAdsSlotActionRequest);
    REGISTER(DbbBondRequest);
    REGISTER(UnitBondBoostRequest);
    REGISTER(BannerClickRequest);
    REGISTER(DailyLoginRequest);
    REGISTER(UnitSelectorGachaTicketRequest);
}
