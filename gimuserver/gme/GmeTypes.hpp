#pragma once

// Type of error IDs
enum class ErrorID
{
    No = 0,
    Yes = 1,
    ForceClose = 3,
};

// Operation to do in case of an error
enum class ErrorOperation
{
    Retry = 2,
    Continue = 3,
    Close = 4,
    Close_2 = 5,
    ReturnToGame = 6,
    RaidError = 7,
    Continue_2 = 8,
    LogoutFacebook = 9,
    Close_3 = 10,
};


constexpr const auto GME_HEADER = "F4q6i9xe";
constexpr const auto GME_BODY = "a3vSYuq2";
constexpr const auto GME_ERROR = "b5PH6mZa";

constexpr const auto HEADER_CLIENT_ID = "aV6cLn3v";
constexpr const auto HEADER_REQUEST_ID = "Hhgi79M1";

constexpr const auto ERROR_ID = "3e9aGpus";
constexpr const auto ERROR_CONTINUE_OP = "iPD12YCr";
constexpr const auto ERROR_MESSAGE = "ZC0msu2L";
constexpr const auto ERROR_UNK_1 = "zcJeTx18";

constexpr const auto BODY_JSON = "Kn51uR4Y";
