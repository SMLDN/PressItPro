#include "worker.h"
//#include "iostream"
#include "thread"



DWORD lastVirtKey = 0;
std::unordered_map<DWORD, WORD> triggerScanCodeMap = {};
std::thread t;
INPUT inputs;
int delay = 90;

void press(const WORD scanCode) {
	//std::cout << "pressed" << scanCode << std::endl;
	inputs.ki.dwFlags = KEYEVENTF_SCANCODE;
	inputs.ki.wScan = scanCode;
	SendInput(1, &inputs, sizeof(INPUT));

	inputs.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
	SendInput(1, &inputs, sizeof(INPUT));
}


void PressKey() {
	auto it = triggerScanCodeMap.find(lastVirtKey);

	if (it != triggerScanCodeMap.end()) {
		auto s = it->second;
		press(s);
		press(s);
		//press(s);
	}
}

void Init() {
	while (1) {
		std::this_thread::sleep_for(std::chrono::microseconds(delay));
		if (lastVirtKey) {
			PressKey();

			lastVirtKey = 0;
		}
	}
}

void WorkerInit() {
	inputs.type = INPUT_KEYBOARD;
	inputs.ki.wVk = 0;
	inputs.ki.time = 0;
	inputs.ki.dwExtraInfo = 0;

	t = std::thread(Init);
}


void OnVirtKeyEvent(DWORD virtKey) {
	lastVirtKey = virtKey;
}


void AddToMapKey(WORD scanCode, std::string skillKeys) {
	std::size_t pos = skillKeys.find(",");

	if (pos == std::string::npos) {
		DWORD sKey = VkKeyScanExA(skillKeys[0], LoadKeyboardLayout(L"00000409", 0x00000001));
		triggerScanCodeMap.insert(std::pair<DWORD, WORD>(sKey, scanCode));
	}
	else {
		std::string skillKey = skillKeys.substr(0, pos);
		DWORD sKey = VkKeyScanExA(skillKey[0], LoadKeyboardLayout(L"00000409", 0x00000001));
		triggerScanCodeMap.insert(std::pair<DWORD, WORD>(sKey, scanCode));
		AddToMapKey(scanCode, skillKeys.substr(pos + 1));
	}

}

void SetDelay(int d) {
	//std::cout << d;
	delay = d;
}