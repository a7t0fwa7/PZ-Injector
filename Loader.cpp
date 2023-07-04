#include <iostream>
#include <Windows.h>

const wchar_t* WINDOW_CLASS = L"GLFW30";
const wchar_t* WINDOW_TITLE = L"Project Zomboid";

HHOOK g_hookHandle = nullptr;

void HideConsoleWindow() {
    HWND consoleWindow = GetConsoleWindow();
    ShowWindow(consoleWindow, SW_HIDE);
}

void DisplayErrorMessage(const std::string& message) {
    std::cout << "Error: " << message << " (Code: " << GetLastError() << ")" << std::endl;
    system("pause");
}

std::wstring FindPrototypeDLL() {
    std::wstring dllName;

    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(L"Prototype*.dll", &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
        dllName = findData.cFileName;
        FindClose(hFind);
    }

    return dllName;
}

int main() {
    std::wstring dllName = FindPrototypeDLL();
    if (dllName.empty()) {
        DisplayErrorMessage("No DLL starting with 'Prototype' found.");
        return EXIT_FAILURE;
    }

    HWND hwnd = FindWindowW(WINDOW_CLASS, nullptr);
    if (hwnd == nullptr) {
        DisplayErrorMessage("Could not find target window.");
        return EXIT_FAILURE;
    }

    DWORD tid = GetWindowThreadProcessId(hwnd, nullptr);
    if (tid == 0) {
        DisplayErrorMessage("Could not get thread ID of the target window.");
        return EXIT_FAILURE;
    }

    HMODULE dll = LoadLibraryEx(dllName.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
    if (dll == nullptr) {
        DisplayErrorMessage("The DLL could not be found.");
        return EXIT_FAILURE;
    }

    HOOKPROC funcAddress = reinterpret_cast<HOOKPROC>(GetProcAddress(dll, "setWindowsHook"));
    if (funcAddress == nullptr) {
        DisplayErrorMessage("The function was not found.");
        FreeLibrary(dll);
        return EXIT_FAILURE;
    }

    g_hookHandle = SetWindowsHookEx(WH_GETMESSAGE, funcAddress, dll, tid);
    if (g_hookHandle == nullptr) {
        DisplayErrorMessage("Couldn't set the hook with SetWindowsHookEx.");
        FreeLibrary(dll);
        return EXIT_FAILURE;
    }

    if (PostThreadMessage(tid, WM_NULL, 0, 0) == 0) {
        DisplayErrorMessage("Failed to post thread message.");
        UnhookWindowsHookEx(g_hookHandle);
        FreeLibrary(dll);
        return EXIT_FAILURE;
    }

    HideConsoleWindow();

    while (FindWindowW(nullptr, WINDOW_TITLE) != nullptr) {
        Sleep(1000);
    }

    UnhookWindowsHookEx(g_hookHandle);
    FreeLibrary(dll);

    return EXIT_SUCCESS;
}