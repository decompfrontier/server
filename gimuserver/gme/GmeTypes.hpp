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
