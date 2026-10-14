// condemned2recomp - ReXGlue Recompiled Project
//
// Condemned 2 Multiplayer
//
// Manages user logins!

#include <rex/rex_app.h>

#include <rex/cvar.h>
#include <rex/hook.h>
#include <rex/runtime.h>
#include <rex/memory/utils.h>
#include <rex/system/xmemory.h>

#include <string>
#include <sstream>
#include <unordered_map>
#include <format>

#include <condemned2_multiplayer.h>
#include <condemned2_hooks.h>

REXCVAR_DEFINE_STRING(condemned2_multiplayer_playername, "Ethan Thomas", "Condemned 2/Multiplayer", "Display name shown in-game");

//Rex Cvars
namespace Condemned2 {
    namespace { constexpr uint64_t kXuid = 0xDEADBEAFBADF00D1ull;}

    Condemned2Multiplayer* g_condemned2_multiplayer = nullptr;
    Condemned2Multiplayer::Condemned2Multiplayer(Condemned2::Condemned2Hook *hook) : _hook(hook) {
        REXLOG_INFO("[condemned2cheats] Starting...");
        g_condemned2_multiplayer = this;
    }
    Condemned2Multiplayer::~Condemned2Multiplayer() = default;

    uint32_t XamUserGetSigninState(uint32_t dwUserIndex) {
        if (dwUserIndex == 0){
            return eXUserSigninState_SignedInToLive;
        }

        return eXUserSigninState_NotSignedIn;
    }

    static_assert(sizeof(XUSER_SIGNIN_INFO) == 40, "XUSER_SIGNIN_INFO layout mismatch");
    uint32_t XamUserGetSigninInfo(uint32_t dwUserIndex, uint32_t dwFlags, XUSER_SIGNIN_INFO* pInfo){
        if (!pInfo){
            return XUSER_INVALID;
        }

        if (dwUserIndex != 0){
            return XUSER_NOT_EXIST;
        }

        memset(pInfo, 0, sizeof(*pInfo));
        pInfo->xuid            = kXuid;
        pInfo->dwInfoFlags     = 0;
        pInfo->UserSigninState = eXUserSigninState_SignedInToLive;
        pInfo->dwGuestNumber   = 0;
        pInfo->dwSponsorUserIndex = 0;
        std::string userName = REXCVAR_GET(condemned2_multiplayer_playername);
        strncpy_s(pInfo->szUserName, userName.c_str(), sizeof(pInfo->szUserName) - 1);

        return 0;
    }

    uint32_t XUserCheckPrivilege(uint32_t dwUserIndex, uint32_t dwPrivilege, uint32_t* pfResult){
        *pfResult = 1;
        return 0;
    }

    REX_HOOK(__imp__XamUserCheckPrivilege, XUserCheckPrivilege);
    REX_HOOK(__imp__XamUserGetSigninState, XamUserGetSigninState);
    REX_HOOK(__imp__XamUserGetSigninInfo, XamUserGetSigninInfo);

    //REX_HOOK(__imp__NetDll_XNetGetTitleXnAddr, NetDll_XNetGetTitleXnAddr);
    //REX_HOOK(__imp__NetDll_XNetRegisterKey, NetDll_XNetRegisterKey);
    //REX_HOOK(__imp__NetDll_XNetGetConnectStatus, NetDll_XNetGetConnectStatus);
}  // namespace Condemned2