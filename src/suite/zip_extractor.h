// condemned2recomp - ReXGlue Recompiled Project
//
// A simple ZIP extractor, typically used for downloads!

#pragma once

#include <zip.h>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <atomic>
#include <thread>
#include <algorithm>
#include <cwctype>
#include <string>
#include <suite/extraction_rules.h>

namespace RexGlueSuite {
    class ZipExtractor {
        public:
            static bool ExtractZipFile(const std::filesystem::path& archive, const std::filesystem::path& destination, ProcessInfo _processInfo, bool ignoreUpdater = false )
            {
                int error = 0;
                std::string errorStr;
                zip_t* zip = zip_open(archive.string().c_str(), ZIP_RDONLY, &error);
                if (!zip) {
                    zip_error_t ze;
                    zip_error_init_with_code(&ze, error);
                    errorStr = std::format("Failed to open ZIP: {}", zip_error_strerror(&ze) );
                    zip_error_fini(&ze);
                    _processInfo.on_error(errorStr);
                    return false;
                }

                std::error_code ec;
                std::filesystem::create_directories(destination, ec);
                if (ec) {
                    errorStr = std::format("Failed to create destination: {}", ec.message()  );
                    zip_close(zip);
                    _processInfo.on_error(errorStr);
                    return false;
                }

                bool success = true;
                for (zip_int64_t i = 0; i < zip_get_num_entries(zip, 0); ++i) {
                    const char* name = zip_get_name(zip, i, 0);
                    if (!name) {
                        success = false;
                        break;
                    }
                    std::filesystem::path relative(name);
                    if (relative.is_absolute() || relative.has_root_name()) {
                        success = false;
                        break;
                    }
                    for (const auto& part : relative) {
                        if (part == "..") {
                            success = false;
                            break;
                        }
                    }
                    if (!success) break;

                    //Do not extract the updater.
                    std::wstring filename = relative.filename().wstring();
                    std::transform(
                        filename.begin(),
                        filename.end(),
                        filename.begin(),
                        [](wchar_t ch) {
                            return static_cast<wchar_t>(std::towlower(ch));
                        });
                    if(!ignoreUpdater){
                        if (filename.find(L"_updater.exe") != std::wstring::npos) {
                            continue;
                        }
                    }

                    std::filesystem::path output = destination / relative;
                    if (name[0] == '\0') continue;
                    if (relative.filename().empty()) {
                        std::filesystem::create_directories(output, ec);
                        if (ec) {
                            success = false;
                            break;
                        }
                        continue;
                    }
                    std::filesystem::create_directories(output.parent_path(), ec);
                    if (ec) {
                        success = false;
                        break;
                    }
                    zip_file_t* file = zip_fopen_index(zip, i, 0);
                    if (!file) {
                        success = false;
                        break;
                    }
                    FILE* out = nullptr;
            #ifdef _WIN32
                    _wfopen_s(&out, output.c_str(), L"wb");
            #else
                    out = std::fopen(output.c_str(), "wb");
            #endif
                    if (!out) {
                        zip_fclose(file);
                        success = false;
                        break;
                    }
                    char buffer[8192];
                    zip_int64_t bytes;
                    while ((bytes = zip_fread(file, buffer, sizeof(buffer))) > 0) {
                        if (std::fwrite(buffer, 1, static_cast<size_t>(bytes), out)
                            != static_cast<size_t>(bytes)) {
                            success = false;
                            break;
                        }
                    }
                    if (bytes < 0) success = false;
                    std::fclose(out);
                    zip_fclose(file);
                    if (!success) break;
                }
                zip_close(zip);
                return success;
            }
        };
}