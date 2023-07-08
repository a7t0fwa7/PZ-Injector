#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <Windows.h>

const char* CONFIG_FILE_NAME = "config.ini";
const std::string DEFAULT_DLL_PATTERN = "Prototype*.dll";
const std::string DEFAULT_WINDOW_TITLE = "Project Zomboid";

class Configuration {
public:
	Configuration() {
		if (!LoadConfiguration()) {
			CreateDefaultConfiguration();
		}
	}

	const std::string& GetDLLPattern() const {
		return dllPattern;
	}

	const std::string& GetWindowTitle() const {
		return windowTitle;
	}

private:
	std::string dllPattern;
	std::string windowTitle;

	bool LoadConfiguration() {
		std::ifstream configFile(CONFIG_FILE_NAME);
		if (configFile.is_open()) {
			std::string line;
			while (std::getline(configFile, line)) {
				if (!line.empty() && line[0] != '#' && line.find('=') != std::string::npos) {
					size_t separatorPos = line.find('=');
					std::string key = line.substr(0, separatorPos);
					std::string value = line.substr(separatorPos + 1);
					if (key == "DllPattern") {
						dllPattern = value;
					}
					else if (key == "WindowTitle") {
						windowTitle = value;
					}
				}
			}
			return true;
		}
		return false;
	}

	void CreateDefaultConfiguration() {
		std::ofstream configFile(CONFIG_FILE_NAME);
		if (configFile.is_open()) {
			configFile << "DllPattern=" << DEFAULT_DLL_PATTERN << std::endl;
			configFile << "WindowTitle=" << DEFAULT_WINDOW_TITLE << std::endl;
			dllPattern = DEFAULT_DLL_PATTERN;
			windowTitle = DEFAULT_WINDOW_TITLE;
		}
		else {
			throw std::runtime_error("Error: Failed to create default configuration file.");
		}
	}
};

class HookManager {
public:
	HookManager(const std::string& dllPattern, const std::string& windowTitle)
		: dllPattern(dllPattern), windowTitle(windowTitle), hookHandle(nullptr), hookSet(false) {}

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
		hookSet = false;
	}

	bool IsHookSet() const {
		return hookSet;
	}

	~HookManager() {
		UnhookAndFree();
	}

private:
	std::string dllPattern;
	std::string windowTitle;
	HHOOK hookHandle;
	bool hookSet;

	HMODULE LoadDll() {
		std::wstring wideDllPattern(dllPattern.begin(), dllPattern.end());
		WIN32_FIND_DATA findData;
		HANDLE hFind = FindFirstFile(wideDllPattern.c_str(), &findData);
		if (hFind != INVALID_HANDLE_VALUE) {
			FindClose(hFind);
			std::wstring wideDllName(findData.cFileName);
			HMODULE dll = LoadLibraryEx(wideDllName.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
			if (dll == nullptr) {
				DisplayErrorMessage("The DLL could not be found.");
			}
			return dll;
		}
		else {
			DisplayErrorMessage("No DLLs matching the pattern found.");
			return nullptr;
		}
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
		HWND hwnd = FindWindowW(nullptr, std::wstring(windowTitle.begin(), windowTitle.end()).c_str());
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
	while (FindWindowW(nullptr, L"Project Zomboid") != nullptr) {
		Sleep(1000);
	}
}

int main() {
	try {
		Configuration config;
		HookManager hookManager(config.GetDLLPattern(), config.GetWindowTitle());
		hookManager.SetHook();

		if (hookManager.IsHookSet()) {
			std::cout << "DLL injected successfully <3" << std::endl;

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
