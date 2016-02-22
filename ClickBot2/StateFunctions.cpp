#include "StateFunctions.h"

void ReadKeyboard(bool* keys)
{
	for (int i = 0; i < 256; ++i)
	{
		keys[i] = false;

		if (GetAsyncKeyState(i) & 0x8000)
		{
			keys[i] = true;
		}
	}
}

void SetKeyboard(bool* prevKeys, bool* keys, INPUT &input)
{
	// 0-7 VK codes are related to the mouse
	for (int i = 8; i < 256; ++i)
	{
		if (prevKeys != nullptr)
		{
			if (prevKeys[i] && !keys[i])
			{
				input.ki.wVk = i;
				input.ki.dwFlags = KEYEVENTF_KEYUP;

				SendInput(1, &input, sizeof(INPUT));
			}
			else if (!prevKeys[i] && keys[i])
			{
				input.ki.wVk = i;
				input.ki.dwFlags = 0;

				SendInput(1, &input, sizeof(INPUT));
			}
		}
		else if (keys[i])
		{
			input.ki.wVk = i;
			input.ki.dwFlags = 0;

			SendInput(1, &input, sizeof(INPUT));
		}
	}
}

void SetMouse(bool* prevKeys, bool* keys, INPUT &input)
{
	// VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_XBUTTON1, VK_XBUTTON2
	SendMouse(VK_LBUTTON, MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP, prevKeys, keys, input);
	SendMouse(VK_RBUTTON, MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP, prevKeys, keys, input);
	SendMouse(VK_MBUTTON, MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP, prevKeys, keys, input);
}

void SendMouse(int vkCode, int downCode, int upCode, bool* prevKeys, bool* keys, INPUT &input)
{
	if (keys[vkCode] && prevKeys == nullptr)
	{
		input.mi.dwFlags = downCode;
		SendInput(1, &input, sizeof(INPUT));
	}
	else if (prevKeys != nullptr)
	{
		if (prevKeys[vkCode] && !keys[vkCode])
		{
			input.mi.dwFlags = upCode;
			SendInput(1, &input, sizeof(INPUT));
		}
		else if (!prevKeys[vkCode] && keys[vkCode])
		{
			input.mi.dwFlags = downCode;
			SendInput(1, &input, sizeof(INPUT));
		}
	}
}