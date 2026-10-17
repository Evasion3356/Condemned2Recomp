// condemned2recomp - ReXGlue Recompiled Project
//
// Condemned2 Updater App. It just replaces the main exe after it downloads updates!

#include <cstdint>
#include <string>
#include <cstdio>
#include <filesystem>
#include <thread>
#include <chrono>
#if defined(_WIN32)
  #include <windows.h>
#elif defined(__linux__)
  #include <unistd.h>
  #include <limits.h>
#endif
#include <suite/zip_extractor.h>

std::filesystem::path exe_dir()
{
#if defined(_WIN32)
    wchar_t buf[32768];
    DWORD n = GetModuleFileNameW(nullptr, buf, (DWORD)(sizeof(buf)/sizeof(buf[0])));
    if (n == 0 || n == (DWORD)(sizeof(buf)/sizeof(buf[0]))) return {};
    return std::filesystem::path(buf, buf + n).parent_path();
#elif defined(__linux__)
    char buf[PATH_MAX];
    ssize_t n = readlink("/proc/self/exe", buf, sizeof(buf)-1);
    if (n <= 0) return {};
    buf[n] = '\0';
    return std::filesystem::path(buf).parent_path();
#else
    return std::filesystem::current_path();
#endif
}

static uint64_t parse_pid_arg(int argc, char** argv)
{
    for (int i = 1; i + 1 < argc; ++i) {
        if (std::string(argv[i]) == "--pid") {
            try { return std::stoull(argv[i + 1]); }
            catch (...) { return 0; }
        }
    }
    return 0;
}

static bool wait_for_pid(uint64_t pid)
{
    if (pid == 0) return true;
#if defined(_WIN32)
    HANDLE h = OpenProcess(SYNCHRONIZE, FALSE, (DWORD)pid);
    if (!h) return false;
    DWORD r = WaitForSingleObject(h, INFINITE);
    CloseHandle(h);
    return r == WAIT_OBJECT_0;
#elif defined(__linux__)
    const auto procPath = std::filesystem::path("/proc") / std::to_string(pid);
    while (std::filesystem::exists(procPath)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    return true;
#else
    (void)pid;
    return true;
#endif
}

static bool relaunch_app(const std::filesystem::path& appPath, const std::filesystem::path& workingDir)
{
#if defined(_WIN32)
    std::wstring exe = appPath.wstring();
    std::wstring cmd = L"\"" + exe + L"\"";
    STARTUPINFOW si{};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi{};
    BOOL createdProcess = CreateProcessW( nullptr, cmd.data(), nullptr, nullptr, FALSE, 0, nullptr, workingDir.wstring().c_str(), &si, &pi);
    if (!createdProcess) {
        std::fwprintf(stderr, L"CreateProcessW failed for %ls (err=%lu)$\n$", exe.c_str(), GetLastError());
        return false;
    }
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return true;
#elif defined(__linux__)
    pid_t child = fork();
    if (child == 0) {
        (void)chdir(workingDir.string().c_str());
        std::string exe = appPath.string();
        execl(exe.c_str(), exe.c_str(), (char*)nullptr);
        _exit(127);
    }
    return child > 0;
#else
    (void)appPath;
    (void)workingDir;
    return false;
#endif
}

int main(int argc, char** argv){
    const uint64_t pid = parse_pid_arg(argc, argv);
    if (pid != 0) {
        if (!wait_for_pid(pid)) {
            std::fprintf(stderr, "Warning!!! Failed to wait for pid %llu$\n$", (unsigned long long)pid);
        }
    }
    bool updated = false;
    const auto exeDir  = exe_dir();
    const auto zipPath = exeDir / "pendingUpdate.zip";
    if (std::filesystem::exists(zipPath)){
        if (RexGlueSuite::ZipExtractor::ExtractZipFile(zipPath, exeDir, {})) {
            std::error_code ec;
            std::filesystem::remove(zipPath, ec);
            updated = true;
        }
    }
    if (updated) {
        // Adjust name if your main exe is named differently
#if defined(_WIN32)
        const auto mainExe = exeDir / "condemned2recomp.exe";
#else
        const auto mainExe = exeDir / "condemned2recomp";
#endif
        relaunch_app(mainExe, exeDir);
    }
    return 0;
}