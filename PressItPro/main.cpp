//#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>
#include "press_it.h"
#include "worker.h"

HWND hookHwnd;


DWORD hookedFunctionKey[25] = { 0 };
DWORD currentPage = 0;


bool IsSwitchPage(const DWORD& wVirtKey) {
	return wVirtKey >= VK_F1 && wVirtKey <= VK_F24;

}


bool NeedToPress() {
	for (int i = 0; i < sizeof(hookedFunctionKey) / sizeof(*hookedFunctionKey); ++i) {
		if (hookedFunctionKey[i] == 0) {
			return false;
		}
		if (hookedFunctionKey[i] == currentPage) {
			return true;
		}
	}

	return false;
}

bool NeedToHandle() {
	return hookHwnd == GetForegroundWindow();
}

LRESULT CALLBACK PressItHook(
	int nCode,
	WPARAM wParam,
	LPARAM lParam
) {
	KBDLLHOOKSTRUCT* kbdStruct = (KBDLLHOOKSTRUCT*)lParam;
	DWORD wVirtKey = kbdStruct->vkCode;
	auto r = CallNextHookEx(NULL, nCode, wParam, lParam);
	if (wParam == WM_KEYDOWN && NeedToHandle()) {
	
		if (IsSwitchPage(wVirtKey)) {
			//std::cout << "switchpage" << std::endl;
			currentPage = wVirtKey;
		}
		else if (NeedToPress()) {
			OnVirtKeyEvent(wVirtKey);
		//auto it = triggerScanCodeMap.find(wVirtKey);
		//
		//if (it != triggerScanCodeMap.end()) {
		//	pressKey(it->second);
		//}
		}
	}
	return r;

}

void AddToHookedFunctionKey(WORD fKey) {
	for (int i = 0; i < sizeof(hookedFunctionKey) / sizeof(*hookedFunctionKey); ++i) {
		DWORD vKey = MapVirtualKey(fKey, MAPVK_VSC_TO_VK);
		if (vKey == hookedFunctionKey[i]) {
			return;
		}
		else if (hookedFunctionKey[i] == 0) {
			hookedFunctionKey[i] = vKey;
			return;
		}

	}
}



void LoadLine(std::string line) {
	std::size_t posDelay = line.find("delay:");
	if (posDelay != std::string::npos) {
		SetDelay(std::stoi(line.substr(posDelay + 6)));
		return;
	}

	std::size_t pos = line.find("=");
	if (pos == std::string::npos) {
		return;
	}

	std::string targetSkillBarPage = line.substr(0, pos);
	std::string skillKeys = line.substr(pos + 1);

	auto it = functionKeyScanCodeMap.find(targetSkillBarPage);

	if (it != functionKeyScanCodeMap.end()) {
		AddToMapKey(it->second, skillKeys);
		AddToHookedFunctionKey(it->second);
	}

}

bool LoadConfig() {
	std::ifstream fs("PressIt.ini");

	if (fs.is_open()) {
		std::string line;
		while (std::getline(fs, line)) {
			LoadLine(line);

			//std::cout << line << std::endl;
		}

		fs.close();
		return true;
	}

	//std::cout << GetLastError();

	return false;
}

DWORD GetGameProcessId(const HWND& hwnd) {
	DWORD processId = 0;
	GetWindowThreadProcessId(hwnd, &processId);
	return processId;
}

int main() {

	if (!LoadConfig()) {
		return -1;
	}

	WorkerInit();

	const char* title = "FxMain";
	hookHwnd = FindWindowExA(0, NULL, title, 0);
	DWORD process = GetGameProcessId(hookHwnd);


	HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD_LL, PressItHook, NULL, 0);

	if (hHook) {
		while (GetMessage(NULL, hookHwnd, 0, 0));
	}

	return 0;
}
