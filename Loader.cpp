#include <iostream>
#include <stdexcept>
#include <Windows.h>

const wchar_t* WINDOW_CLASS = L"GLFW30";
const wchar_t* WINDOW_TITLE = L"Project Zomboid";

class HookManager {
public:
	HookManager(const wchar_t* dllPattern)
		: dllName(FindPrototypeDLL(dllPattern)), hookHandle(nullptr), hookSet(false) {}

	void SetHook() {
		HMODULE dll = LoadDll();
		HOOKPROC funcAddress = GetHookFunction(dll);
		HWND hwnd = FindTargetWindow();

		DWORD tid = GetWindowThreadProcessId(hwnd, nullptr);
		hookHandle = SetWindowsHookEx(WH_GETMESSAGE, funcAddress, dll, tid);
		if (hookHandle == nullptr) {
			FreeLibrary(dll);
			DisplayErrorMessage("Couldn't set the hook with SetWindowsHookEx.");
		}
		else {
			hookSet = true;
		}
	}

	void UnhookAndFree() {
		if (hookHandle != nullptr) {
			UnhookWindowsHookEx(hookHandle);
			hookHandle = nullptr;
		}
		if (!dllName.empty()) {
			FreeLibrary(GetModuleHandle(dllName.c_str()));
			dllName.clear();
		}
		hookSet = false;
	}

	bool IsHookSet() const {
		return hookSet;
	}

	~HookManager() {
		UnhookAndFree();
	}

private:
	std::wstring dllName;
	HHOOK hookHandle;
	bool hookSet;

	std::wstring FindPrototypeDLL(const wchar_t* dllPattern) {
		std::wstring dllName;
		WIN32_FIND_DATA findData;
		HANDLE hFind = FindFirstFile(dllPattern, &findData);
		if (hFind != INVALID_HANDLE_VALUE) {
			dllName = findData.cFileName;
			FindClose(hFind);
		}
		return dllName;
	}

	HMODULE LoadDll() {
		HMODULE dll = LoadLibraryEx(dllName.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
		if (dll == nullptr) {
			DisplayErrorMessage("The DLL could not be found.");
		}
		return dll;
	}

	HOOKPROC GetHookFunction(HMODULE dll) {
		HOOKPROC funcAddress = reinterpret_cast<HOOKPROC>(GetProcAddress(dll, "setWindowsHook"));
		if (funcAddress == nullptr) {
			DisplayErrorMessage("The function was not found.");
			FreeLibrary(dll);
		}
		return funcAddress;
	}

	HWND FindTargetWindow() {
		HWND hwnd = FindWindowW(WINDOW_CLASS, nullptr);
		if (hwnd == nullptr) {
			DisplayErrorMessage("Could not find target window.");
		}
		return hwnd;
	}

	void DisplayErrorMessage(const std::string& message) {
		std::cerr << "Error: " << message << " (Code: " << GetLastError() << ")" << std::endl;
		throw std::runtime_error(message);
	}
};

void HideConsoleWindow() {
	HWND consoleWindow = GetConsoleWindow();
	ShowWindow(consoleWindow, SW_HIDE);
}

void WaitForWindowClose() {
	while (FindWindowW(nullptr, WINDOW_TITLE) != nullptr) {
		Sleep(1000);
	}
}

int main() {
	try {
		HookManager hookManager(L"Prototype*.dll");
		hookManager.SetHook();

		if (hookManager.IsHookSet()) {
			std::cout << "[»] DLL injected successfully!" << std::endl;

			Sleep(2000);
			HideConsoleWindow();
			WaitForWindowClose();
		}

		return EXIT_SUCCESS;
	}
	catch (const std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
