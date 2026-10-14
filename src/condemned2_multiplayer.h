// condemned2recomp - ReXGlue Recompiled Project
//
// Condemned 2 Multiplayer

#pragma once

#include <cstdint>
#include <mutex>
#include <queue>
#include <string>

#include <condemned2_hooks.h>

namespace Condemned2{
    enum XUSER_SIGNIN_STATE {
        eXUserSigninState_NotSignedIn      = 0,
        eXUserSigninState_SignedInLocally  = 1,
        eXUserSigninState_SignedInToLive   = 2,
    };
    enum XUSER_SIGNIN_ERROR {
        XUSER_INVALID      = 0x80070057,
        XUSER_NOT_EXIST  = 0x00000525,
    };
    #pragma pack(push, 1)
    struct XUSER_SIGNIN_INFO {
        uint64_t xuid;              
        uint32_t dwInfoFlags;       
        uint32_t UserSigninState;   
        uint32_t dwGuestNumber;     
        uint32_t dwSponsorUserIndex;
        char szUserName[16];    
    };
    #pragma pack(pop)
    class Condemned2Multiplayer  {
      public:
        explicit Condemned2Multiplayer(Condemned2::Condemned2Hook *hook);
        ~Condemned2Multiplayer();

        Condemned2::Condemned2Hook *GetHook() { return _hook; }
      private:
        Condemned2::Condemned2Hook *_hook;
   };
}  // namespace Condemned2