// RexGlueSuite - Shared extraction safety utilities
//
// Common functions used the RexGlueSuite
#pragma once

#include <filesystem>
#include <string>
#include <algorithm>
#include <cstdint>
#include <optional>
#include <functional>
#include <format>
#include "picosha2.h"

namespace RexGlueSuite {
    struct ProcessInfo {
      std::filesystem::path _basePath; 
      std::function<void()> on_success = []{};
      std::function<void(std::string err)> on_error = [](std::string){};
      std::function<void(std::string log)> on_log = [](std::string){};
    };

    class ExtractionRules {
      public:
        //Prevent unauthorized directory travel
        static bool IsPathSafe(const std::filesystem::path& fullPath){
          auto name = fullPath.string();
          if (name.find("..") != std::string::npos ||
            name.find("\\..") != std::string::npos ||
            name.find("../") != std::string::npos) {
            return false;
          }
          return true;
        }

        // Calculate available disk space and required available disk space
        // Adds a buffer that's 5% of the total extraction size, or 512mb at the least.
        static bool HasRequiredSpace( std::filesystem::path _basePath, uint64_t totalBytes, std::function<void(std::string err)> on_error, std::string msgCaller ="rexglue_suite_extractionrules"){
          std::error_code ec;
          auto space = std::filesystem::space(_basePath, ec);
          uint64_t requiredSpace = totalBytes + std::max<uint64_t>(512ull * 1024 * 1024, static_cast<uint64_t>(totalBytes * 0.05));
          if (!ec && space.available < requiredSpace ) {
            on_error(std::format("[{}] Not enough disk space: {} bytes is required, have {} available", msgCaller, requiredSpace, space.available));
            return false;
          }
          return true;
        }

        //RexGlue deprecated SHA256 as of v0.10.0, so it's been reimplemented.
        static std::string sha256(std::string_view data) {
          std::string hash256_hex;
          picosha2::hash256_hex_string(data.begin(), data.end(), hash256_hex);
          return hash256_hex;
        }
    };
}