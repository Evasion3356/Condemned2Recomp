// condemned2recomp - ReXGlue Recompiled Project
//
// Condemned 2 Asset Managers

#pragma once

#include <cctype>
#include <cstring>
#include <mutex>
#include <string>
#include <unordered_map>

#include <condemned2_hooks.h>

namespace Condemned2{
   class Condemned2Assets  {
      public:
        explicit Condemned2Assets(Condemned2::Condemned2Hook *hook);
        ~Condemned2Assets();

        void InitializeAssets();
        Condemned2::Condemned2Hook *GetHook() { return _hook; }
        void SetLoadingBundle( bool enable ) { isLoadingBundle = enable; }
        bool IsLoadingBundle() { return isLoadingBundle; }

        void SetCurrentBundle(std::string bundle) { currentBundle = bundle; }
        std::string GetCurrentBundle() { return currentBundle; }

        void AddLoadedResource( std::string res ){ loadedAssets[res] = currentBundle; }
        bool IsResourceLoaded( std::string res){return loadedAssets.find(res) != loadedAssets.end();}

        bool IsAdditionalBundle(const char* filename)
        {
            if (filename == nullptr)
                return false;

            while (*filename == '\\' || *filename == '/')
                ++filename;

            const char* extension = std::strrchr(filename, '.');
            if (extension == nullptr ||
                std::strcmp(extension, ".bndl") != 0) {
                return false;
            }

            if (std::strcmp(filename, "interface\\common.bndl") == 0 ||
                std::strcmp(filename, "Interface\\Extras.bndl") == 0 ||
                std::strcmp(filename, "Interface\\MPWorldLoad.bndl") == 0 ||
                std::strcmp(filename, "Interface\\Startup.bndl") == 0 ||
                std::strcmp(filename, "Interface\\Shell.bndl") == 0) {
                return false;
            }
            return true;
        }
      private:
        Condemned2::Condemned2Hook *_hook;
        std::unordered_map<std::string, std::string> loadedAssets;
        std::string currentBundle;
        bool isLoadingBundle = false;
   };
}  // namespace Condemned2