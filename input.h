#pragma once
#include <Windows.h>

inline void set_key_pressed(char key, bool pressed) {
	INPUT key_press = { 0 };
	key_press.type = INPUT_KEYBOARD;
	key_press.ki.wVk = VkKeyScanEx(key, GetKeyboardLayout(NULL)) & 0xFF;
	key_press.ki.wScan = 0;
	key_press.ki.dwExtraInfo = 0;
	key_press.ki.dwFlags = (pressed ? 0 : KEYEVENTF_KEYUP);
    SendInput(1, &key_press, sizeof (INPUT));
}
