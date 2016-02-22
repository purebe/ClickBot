// StateFunctions.h

#pragma once

#include "Windows.h"

void ReadKeyboard(bool* keys);
void SetKeyboard(bool* prevKeys, bool* keys, INPUT &input);

void SetMouse(bool* prevKeys, bool* keys, INPUT &input);
void SendMouse(int vkCode, int downCode, int upCode, bool* prevKeys, bool* keys, INPUT &input);