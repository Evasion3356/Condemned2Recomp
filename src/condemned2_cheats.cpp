// condemned2recomp - ReXGlue Recompiled Project
//
// Condemned 2 Graphics
//
// Manages cheats and QoL improvements

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

#include <condemned2_cheats.h>
#include <condemned2_hooks.h>

REX_EXTERN(__imp__Unlockables_Levels);
REX_EXTERN(__imp__Unlockables_GunEfficiency);
REX_EXTERN(__imp__Unlockables_Upgrades);

//Cheats
REXCVAR_DEFINE_BOOL(condemned2_cheat_fps_mode, false, "Condemned 2/Unlockables", "FPS Mode ( Must be enabled before entering menu!!! )");
REXCVAR_DEFINE_BOOL(condemned2_cheat_unlock_all_levels, false, "Condemned 2/Unlockables", "Unlocks every level!");
REXCVAR_DEFINE_BOOL(condemned2_cheat_unlock_upgrades, false, "Condemned 2/Unlockables", "Unlocks all upgrades!");
REXCVAR_DEFINE_BOOL(condemned2_cheat_top_gun_accuracy, false, "Condemned 2/Unlockables", "Gives player 100% gun efficiency!");
//QoL
REXCVAR_DEFINE_BOOL(condemned2_cheat_skip_intro_movies, false, "Condemned 2/QoL Improvements", "Skips intro movies.");
REXCVAR_DEFINE_BOOL(condemned2_cheat_skip_ingame_movies, false, "Condemned 2/QoL Improvements", "Skips in-game movies.");

REXCVAR_DEFINE_BOOL(condemned2_hud_show_weapon_stats, false, "Condemned 2/HUD Display", "Disables weapon display when looking at weapons.");

//Rex Cvars
namespace Condemned2 {
    Condemned2Cheats* g_condemned2_cheats = nullptr;
    Condemned2Cheats::Condemned2Cheats(Condemned2::Condemned2Hook *hook) : _hook(hook) {
        REXLOG_INFO("[condemned2cheats] Starting...");
        g_condemned2_cheats = this;
    }
    Condemned2Cheats::~Condemned2Cheats() = default;

    void Condemned2Cheats::InitializeCheats(){
        // Unlockables!
        rex::cvar::RegisterChangeCallback("condemned2_cheat_unlock_all_levels", [&](std::string_view name, std::string_view new_value) {
            SetHasAllLevelsUnlocked(new_value == "true");
        });
        SetHasAllLevelsUnlocked( std::stod( Condemned2::Condemned2Hook::GetSafeString( rex::cvar::GetFlagByName("condemned2_cheat_unlock_all_levels") ) ) );

        rex::cvar::RegisterChangeCallback("condemned2_cheat_unlock_upgrades", [&](std::string_view name, std::string_view new_value) {
            SetHasAllUpgrades(new_value == "true");
        });
        SetHasAllUpgrades( std::stod( Condemned2::Condemned2Hook::GetSafeString( rex::cvar::GetFlagByName("condemned2_cheat_unlock_upgrades") ) ) );

        rex::cvar::RegisterChangeCallback("condemned2_cheat_top_gun_accuracy", [&](std::string_view name, std::string_view new_value) {
            SetHasTopGunEfficiency(new_value == "true");
        });
        SetHasTopGunEfficiency( std::stod( Condemned2::Condemned2Hook::GetSafeString( rex::cvar::GetFlagByName("condemned2_cheat_top_gun_accuracy") ) ) );

        // FPS Mode ( Unlimited Ammo + Starting Firearm )
        _hook->AddHook({"condemned2_cheat_fps_mode", 0, 0.0, 0.0, 1.0, false, [&](std::string_view name, std::string_view new_value) {
             _hook->SendCommandCmd("FPSMode",  std::stod( Condemned2::Condemned2Hook::GetSafeString( new_value ) ) ); 
        }});
        _hook->SendCommandCmd("FPSMode",  std::stod( Condemned2::Condemned2Hook::GetSafeString( rex::cvar::GetFlagByName("condemned2_cheat_fps_mode") ) ) ); 

        // Skip Intro Movies ( BUG: Skips movies, but breaks at start screen!? )
        _hook->AddHook({"condemned2_cheat_skip_intro_movies", 0, 0.0, 0.0, 1.0, false, [&](std::string_view name, std::string_view new_value) {
            _hook->SendCommandCmd("SkipSplashScreens",  std::stod( Condemned2::Condemned2Hook::GetSafeString( new_value ) ) ); //SkipSplashScreens
        }});
        _hook->SendCommandCmd("SkipSplashScreens",  std::stod( Condemned2::Condemned2Hook::GetSafeString( rex::cvar::GetFlagByName("condemned2_cheat_skip_intro_movies") ) ) ); 
    
        // Skip In-Game Movies
        _hook->AddHook({"condemned2_cheat_skip_ingame_movies", 0, 0.0, 0.0, 1.0, false, [&](std::string_view name, std::string_view new_value) {
            _hook->SendCommandCmd("SkipIngameMovies",  std::stod( Condemned2::Condemned2Hook::GetSafeString( new_value ) ) ); //SkipIngameMovies
        }});
        _hook->SendCommandCmd("SkipIngameMovies",  std::stod( Condemned2::Condemned2Hook::GetSafeString( rex::cvar::GetFlagByName("condemned2_cheat_skip_ingame_movies") ) ) ); 
    }
 
    REX_HOOK_RAW(Unlockables_Levels) { 
        if( g_condemned2_cheats->HasAllLevelsUnlocked() ){
            return;
        }
        __imp__Unlockables_Levels(ctx, base);
    }

    REX_HOOK_RAW(Unlockables_GunEfficiency) {
        if( g_condemned2_cheats->HasTopGunEfficiency() ){
            ctx.f1.f32 = 100.0f;
            return;
        }
        __imp__Unlockables_GunEfficiency(ctx, base);
    }

    REX_HOOK_RAW(Unlockables_Upgrades) {
        if(  g_condemned2_cheats->HasAllUpgrades() ){
            return;
        }
        __imp__Unlockables_Upgrades(ctx, base);
    }
}  // namespace Condemned2