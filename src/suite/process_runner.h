// condemned2recomp - ReXGlue Recompiled Project
//
// Process Runner
//
// Useful for running other applications or tools!

#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <format>
#include <fstream>
#include <set>
#include <unordered_set>
#include <cstring>
#include <atomic>
#include <thread>
#ifdef _WIN32
    #include <Windows.h>
#else
    #include <cerrno>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <unistd.h>
#endif
#include <suite/extraction_rules.h>
#include <suite/file_dialog.h>

namespace RexGlueSuite {
    class ProcessRunner {
      public:
        ProcessRunner() {}

        static std::wstring ProcessArg(const std::wstring& arg) {
            std::wstring result = L"\"";
            size_t slashes = 0;
            for (wchar_t c : arg) {
                if (c == L'\\') {
                    ++slashes;
                } else if (c == L'"') {
                    result.append(slashes * 2 + 1, L'\\');
                    result += L'"';
                    slashes = 0;
                } else {
                    result.append(slashes, L'\\');
                    slashes = 0;
                    result += c;
                }
            }
            result.append(slashes * 2, L'\\');
            result += L'"';
            return result;
        }

        static bool RunProcess(std::filesystem::path _appPath,std::vector<std::string> _args,ProcessInfo _processInfo) {
            try {
                if (!std::filesystem::is_regular_file(_appPath)) {
                    _processInfo.on_error("Executable not found: " + _appPath.string());
                    return false;
                }
                _processInfo.on_log("[rexglue_suite_process_runner] Starting process: " + _appPath.string());

            #ifdef _WIN32
                SECURITY_ATTRIBUTES sa{};
                sa.nLength = sizeof(sa);
                sa.bInheritHandle = TRUE;
                HANDLE readPipe = nullptr;
                HANDLE writePipe = nullptr;
                if (!CreatePipe(&readPipe, &writePipe, &sa, 0)) {
                    _processInfo.on_error("[rexglue_suite_process_runner] Failed to create pipe!!!");
                    return false;
                }

                if (!SetHandleInformation(readPipe, HANDLE_FLAG_INHERIT, 0)) {
                    CloseHandle(readPipe);
                    CloseHandle(writePipe);
                    _processInfo.on_error("[rexglue_suite_process_runner] Failed to configure pipe!!!");
                    return false;
                }

                STARTUPINFOW si{};
                si.cb = sizeof(si);
                si.dwFlags = STARTF_USESTDHANDLES;
                si.hStdOutput = writePipe;
                si.hStdError = writePipe;
                si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

                PROCESS_INFORMATION pi{};
                std::wstring command = ProcessArg(_appPath.wstring());
                for (const auto& arg : _args) {
                    command += L' ';
                    command += ProcessArg(FileDialog::ToWide(arg));
                }
                std::vector<wchar_t> commandBuffer(command.begin(), command.end());
                commandBuffer.push_back(L'\0');
                std::wstring workingDirectory;
                LPCWSTR cwd = nullptr;

                if (!_processInfo._basePath.empty()) {
                    workingDirectory = _processInfo._basePath.wstring();
                    cwd = workingDirectory.c_str();
                }

                BOOL created = CreateProcessW(_appPath.c_str(),commandBuffer.data(),nullptr, nullptr,TRUE,CREATE_NO_WINDOW,nullptr,cwd,&si, &pi);
                CloseHandle(writePipe);
                if (!created) {
                    DWORD code = GetLastError();
                    CloseHandle(readPipe);
                    _processInfo.on_error("[rexglue_suite_process_runner] Failed to launch process!!! Windows error: " + std::to_string(code));
                    return false;
                }

                char buffer[4096];
                DWORD bytesRead = 0;
                while (ReadFile(readPipe, buffer, sizeof(buffer), &bytesRead, nullptr) && bytesRead > 0) {
                    _processInfo.on_log("[rexglue_suite_process_runner] " + std::string(buffer, bytesRead));
                }

                CloseHandle(readPipe);
                WaitForSingleObject(pi.hProcess, INFINITE);
                DWORD exitCode = 1;
                GetExitCodeProcess(pi.hProcess, &exitCode);
                CloseHandle(pi.hThread);
                CloseHandle(pi.hProcess);
                if (exitCode != 0) {
                    _processInfo.on_error("[rexglue_suite_process_runner] Process exited with code " + std::to_string(exitCode));
                    return false;
                }
            #else
                int pipefd[2];
                if (pipe(pipefd) == -1) {
                    _processInfo.on_error("[rexglue_suite_process_runner] Failed to create output pipe!!!");
                    return false;
                }

                pid_t pid = fork();
                if (pid == -1) {
                    close(pipefd[0]);
                    close(pipefd[1]);
                    _processInfo.on_error("[rexglue_suite_process_runner] Failed to create child process!!!");
                    return false;
                }

                if (pid == 0) {
                    close(pipefd[0]);
                    if (dup2(pipefd[1], STDOUT_FILENO) == -1 ||
                        dup2(pipefd[1], STDERR_FILENO) == -1) {
                        _exit(126);
                    }
                    close(pipefd[1]);
                    if (!_processInfo._basePath.empty()) {
                        if (chdir(_processInfo._basePath.c_str()) == -1) {
                            _exit(126);
                        }
                    }
                    std::vector<char*> argv;
                    argv.reserve(_args.size() + 2);
                    std::string app = _appPath.string();
                    argv.push_back(app.data());
                    for (auto& arg : _args) argv.push_back(arg.data());
                    argv.push_back(nullptr);
                    execv(_appPath.c_str(), argv.data());
                    _exit(127);
                }
                close(pipefd[1]);

                char buffer[4096];
                ssize_t bytesRead;
                while ((bytesRead = read(
                    pipefd[0], buffer, sizeof(buffer))) > 0) {
                    log(std::string(buffer, bytesRead));
                }
                close(pipefd[0]);

                int status = 0;
                pid_t waited;
                do {
                    waited = waitpid(pid, &status, 0);
                } while (waited == -1 && errno == EINTR);

                if (waited == -1) {
                    _processInfo.on_error("[rexglue_suite_process_runner] Failed to wait for child process!!!");
                    return false;
                }

                if (!WIFEXITED(status) ||
                    WEXITSTATUS(status) != 0) {
                    if (WIFEXITED(status)) {
                        _processInfo.on_error("[rexglue_suite_process_runner] Process exited with status " + std::to_string(WEXITSTATUS(status)));
                    } else {
                        _processInfo.on_error("[rexglue_suite_process_runner] Process terminated improperly.");
                    }
                    return false;
                }
            #endif
                _processInfo.on_log("[rexglue_suite_process_runner] Process completed.");
                if (_processInfo.on_success)
                    _processInfo.on_success();

                return true;
            }
            catch (const std::exception& e) {
                _processInfo.on_error(std::string("[rexglue_suite_process_runner] Process runner exception: ") + e.what());
                return false;
            }
        }
        static std::thread NewThread(std::filesystem::path _appPath, const std::vector<std::string>& _args, ProcessInfo _processInfo ){
            return std::thread (RunProcess, _appPath, _args, _processInfo ); 
        }
        static void NewThreadDetached(std::filesystem::path _appPath, const std::vector<std::string> _args, ProcessInfo _processInfo){
            std::thread([_appPath = std::move(_appPath), _args = std::move(_args), _processInfo = std::move(_processInfo)]() mutable { RunProcess(std::move(_appPath), std::move(_args), std::move(_processInfo)); }).detach();
        }
  };
}