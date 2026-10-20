// condemned2recomp - ReXGlue Recompiled Project
//
// Condemned 2 Graphics
//
// Handles various graphics hooks

#include <rex/rex_app.h>

#include <rex/cvar.h>
#include <rex/hook.h>
#include <rex/runtime.h>
#include <rex/memory/utils.h>
#include <rex/system/xmemory.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <limits>

#include <condemned2_hooks.h>
#include <condemned2_assets.h>

REX_EXTERN(__imp__Game_LoadExtResource);
REX_EXTERN(__imp__Game_InitWorldModel);
REX_EXTERN(__imp__Game_InitWorldObject);
REX_EXTERN(__imp__Game_LoadStartingBundle);
REX_EXTERN(__imp__Game_ProcessBundle);
REX_EXTERN(__imp__Game_ReadAsset);

//Rex Cvars
namespace Condemned2 {
    Condemned2Assets* g_condemned2_assets = nullptr;
    Condemned2Assets::Condemned2Assets(Condemned2::Condemned2Hook *hook) : _hook(hook) {
        REXLOG_INFO("[Condemned2Assets] Starting...");
        g_condemned2_assets = this;
    }
    Condemned2Assets::~Condemned2Assets() = default;

    void Condemned2Assets::InitializeAssets(){
        
    }
 
    REX_HOOK_RAW(Game_ReadAsset){
        uint32_t self = ctx.r3.u64, name_g = ctx.r4.u32, unk = ctx.r5.u64; 
        const char* p = g_condemned2_assets->GetHook()->GetMemory()->TranslateVirtual<const char*>(name_g); 
        REXLOG_INFO("[Game_ReadAsset] this={} lr={:#x} name_address={} name={}", self, ctx.lr, name_g, p);
        bool isBundle = g_condemned2_assets->IsAdditionalBundle(p);
        if(g_condemned2_assets->IsAdditionalBundle(p)){
            g_condemned2_assets->SetCurrentBundle(p);
        }
        __imp__Game_ReadAsset(ctx, base);

        //Load bundles containing desired assets!
        if (p != nullptr && std::strstr(p, "_") == nullptr && g_condemned2_assets->IsAdditionalBundle(p) ){
            const std::vector<std::string> bundlePaths = {
                R"(Worlds\FC\fc_brawler_0000.bndl)",
                R"(Worlds\FC\fc_practice_0000.bndl)",
                R"(Worlds\SP\Mission_02\M02_0005.bndl)",
                R"(Worlds\SP\Mission_04\M04_0000.bndl)",
                R"(Worlds\SP\Mission_05\M05_0000.bndl)",
                R"(Worlds\SP\Mission_06\M06_0000.bndl)",
                R"(Worlds\SP\Mission_07\M07_0000.bndl)",
                R"(Worlds\SP\Mission_08\M08_0000.bndl)",
                R"(Worlds\SP\Mission_09\M09_0000.bndl)",
                R"(Worlds\SP\Mission_10\M10_0000.bndl)"
            };
            for (const std::string& bundlePath : bundlePaths)
            {
                if( g_condemned2_assets->IsResourceLoaded(bundlePath) )
                    continue;

                ctx.r3.u32 = self;
                ctx.r4.u32 = g_condemned2_assets->GetHook()->AllocateHeapForString(bundlePath.c_str());
                ctx.r5.u64 = unk;
                g_condemned2_assets->AddLoadedResource(bundlePath);
                g_condemned2_assets->SetLoadingBundle(true);
                __imp__Game_ReadAsset(ctx, base);
            }
        }
    }

    //Resources verified as bundles are parsed and loaded.
    REX_HOOK_RAW(Game_LoadExtResource){
        uint32_t self = ctx.r3.u32, name_g = ctx.r4.u32; 
        const char* p = g_condemned2_assets->GetHook()->GetMemory()->TranslateVirtual<const char*>(name_g); 
        if(  g_condemned2_assets->IsLoadingBundle() && ( g_condemned2_assets->IsResourceLoaded(p) || std::strstr(p, "arsenal") == nullptr ) ){
            ctx.r3.u32 = 0;
        }else{
            REXLOG_INFO("[Game_LoadExtResource] this={} lr={:#x} name_address={} name={}", self, ctx.lr, name_g, p);
            g_condemned2_assets->AddLoadedResource(p);
            __imp__Game_LoadExtResource(ctx, base);
        }
    }

    REX_HOOK_RAW(Game_LoadStartingBundle) {
        REXLOG_INFO("[Game_LoadStartingBundle] lr={:#x}", ctx.lr);
        __imp__Game_LoadStartingBundle(ctx, base);
    }

    REX_HOOK_RAW(Game_ProcessBundle) {
        REXLOG_INFO("[Game_ProcessBundle] lr={:#x}", ctx.lr);
        __imp__Game_ProcessBundle(ctx, base);
        g_condemned2_assets->SetLoadingBundle(false);
    }

    REX_HOOK_RAW(Game_InitWorldModel) {
        __imp__Game_InitWorldModel(ctx, base);
    }

    REX_HOOK_RAW(Game_InitWorldObject) {
        uint32_t name_g = ctx.r4.u32;
        const char* name_h = g_condemned2_assets->GetHook()->GetMemory()->TranslateVirtual<const char*>(name_g);
        REXLOG_INFO("[condemned2_hooks][Game_InitWorldObject] this={:#x} name_address={:#x} name={}", ctx.r3.u32, name_g, name_h);
        __imp__Game_InitWorldObject(ctx, base);
    }
}  // namespace Condemned2