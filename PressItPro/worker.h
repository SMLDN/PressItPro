#pragma once
#include <unordered_map>
#include <windows.h>


void WorkerInit();

void OnVirtKeyEvent(DWORD virtKey);

void AddToMapKey(WORD scanCode, std::string skillKeys);

void SetDelay(int d);