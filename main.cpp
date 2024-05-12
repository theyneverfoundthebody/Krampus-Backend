#include <iostream>
#include <windows.h>
#include <TlHelp32.h>
#include <psapi.h>
#include <ctime>
#include <cstdlib>
#include <string>
#include <vector>
#include <WinINet.h>

#pragma comment(lib, "wininet.lib")

BOOL FreezeProcess(DWORD dwProcessId) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
    if (hProcess == NULL) {
        return FALSE;
    }

    BOOL bResult = DebugActiveProcess(dwProcessId);
    CloseHandle(hProcess);

    return bResult;
}

BOOL ThawProcess(DWORD dwProcessId) {
    return DebugActiveProcessStop(dwProcessId);
}

void FixError(const std::string& randomFile) {
    remove(randomFile.c_str());
}

int main() {
    std::cout << "Krampus Backend" << std::endl;

    std::cout << "Waiting for RobloxPlayerBeta.exe to start..." << std::endl;
    while (true) {
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(entry);
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
        if (Process32First(snapshot, &entry)) {
            do {
                if (std::wstring(entry.szExeFile) == L"RobloxPlayerBeta.exe") {
                    break;
                }
            } while (Process32Next(snapshot, &entry));
        }
        CloseHandle(snapshot);
        if (std::wstring(entry.szExeFile) == L"RobloxPlayerBeta.exe") {
            break;
        }
        Sleep(1000);
    }

    std::cout << "RobloxPlayerBeta.exe detected!" << std::endl;

    std::cout << "Simulating game loading..." << std::endl;
    for (int i = 1; i <= 10; ++i) {
        std::cout << "Loading game: " << i * 10 << "% complete" << std::endl;
        Sleep(500);
    }

    DWORD processId = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(entry);
        if (Process32First(snapshot, &entry)) {
            do {
                if (std::wstring(entry.szExeFile) == L"RobloxPlayerBeta.exe") {
                    processId = entry.th32ProcessID;
                    break;
                }
            } while (Process32Next(snapshot, &entry));
        }
        CloseHandle(snapshot);
    }

    if (processId != 0) {
        FreezeProcess(processId);
        Sleep(5000);
        ThawProcess(processId);
        std::cout << "RobloxPlayerBeta.exe unfrozen." << std::endl;
    }

    double errorChance = static_cast<double>(rand()) / RAND_MAX;
    if (errorChance < 0.3) {
        int errorType = rand() % 3;
        if (errorType == 0) {
            FixError("system_conflict.txt");
        } else if (errorType == 1) {
            FixError("required_file_not_found.txt");
        } else {
            FixError("permission_denied.txt");
        }
    } else if (errorChance < 0.6) {
        FixError("unknown_error.txt");
    } else {
        std::cout << "Loading cheat..." << std::endl;
        Sleep(2000);
        std::cout << "Cheat loaded successfully!" << std::endl;
    }

    HINTERNET hInternet = InternetOpen(L"Krampus Backend", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet) {
        HINTERNET hConnect = InternetConnect(hInternet, L"discord.com", INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
        if (hConnect) {
            HINTERNET hRequest = HttpOpenRequest(hConnect, L"POST", L"/api/webhooks/your-webhook-id/your-webhook-token", NULL, NULL, NULL, INTERNET_FLAG_SECURE, 0);
            if (hRequest) {
                LPCWSTR headers = L"Content-Type: application/json\r\n";
                LPCWSTR body = L"{\"content\":\"Cheat loaded successfully!\"}";
                if (HttpSendRequest(hRequest, headers, wcslen(headers), (LPVOID)body, wcslen(body))) {
                    std::cout << "Message sent to Discord webhook." << std::endl;
                } else {
                    std::cerr << "Failed to send message to Discord webhook." << std::endl;
                }
                InternetCloseHandle(hRequest);
            } else {
                std::cerr << "Failed to open HTTP request." << std::endl;
            }
            InternetCloseHandle(hConnect);
        } else {
            std::cerr << "Failed to connect to Discord server." << std::endl;
        }
        InternetCloseHandle(hInternet);
    } else {
        std::cerr << "Failed to open internet connection." << std::endl;
    }

    return 0;
}
