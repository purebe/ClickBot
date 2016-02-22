// ClickBot.cpp

#include "ClickBot.h"

ClickBot::ClickBot() : CHUNK_SIZE(60000), chunk(0), ticker(0), max_chunk(0), max_ticker(0), loop(false), playing(false), recording(false)
{
	frames.push_back(new Frame[CHUNK_SIZE]);

	kybdInput.type = INPUT_KEYBOARD;
	kybdInput.ki.wScan = 0;
	kybdInput.ki.time = 0;
	kybdInput.ki.dwExtraInfo = 0;

	mouseInput.type = INPUT_MOUSE;
	mouseInput.mi.mouseData = XBUTTON1;
	mouseInput.mi.dwExtraInfo = 0;
	mouseInput.mi.time = 0;
	mouseInput.mi.dx = 0;
	mouseInput.mi.dy = 0;
}

ClickBot::~ClickBot()
{
	for (Frame *frame : frames)
	{
		delete [] frame;
	}
}

void ClickBot::BeginRec()
{
	bool keys[256];
	ReadKeyboard(keys);
	chunk = 0;
	ticker = 0;
	max_chunk = 0;
	max_ticker = 0;
	recording = true;
}

void ClickBot::StopRec()
{
	recording = false;
}

void ClickBot::BeginPlay()
{
	chunk = 0;
	ticker = 0;
	playing = true;
}

void ClickBot::StopPlay()
{
	playing = false;
}

void ClickBot::RecordTick()
{
	if (playing)
	{
		return;
	}

	if (ticker >= CHUNK_SIZE)
	{
		frames.push_back(new Frame[CHUNK_SIZE]);
		chunk++;
		max_chunk++;
		ticker = 0;
		max_ticker = 0;
	}

	GetCursorPos(&frames[chunk][ticker].mouseState);
	ReadKeyboard(frames[chunk][ticker].keyboardState);
	ticker++;
	max_ticker++;
}

void ClickBot::PlayTick()
{
	if (!playing || recording)
	{
		return;
	}

	if (ticker >= CHUNK_SIZE)
	{
		chunk++;
		ticker = 0;
	}
	if (chunk == max_chunk)
	{
		if (ticker >= max_ticker)
		{
			if (loop)
			{
				BeginPlay();
			}
			else
			{
				playing = false;
				return;
			}
		}
	}

	SetCursorPos(frames[chunk][ticker].mouseState.x, frames[chunk][ticker].mouseState.y);

	if (chunk == 0 && ticker == 0)
	{
		SetKeyboard(nullptr, frames[chunk][ticker].keyboardState, kybdInput);
		SetMouse(nullptr, frames[chunk][ticker].keyboardState, mouseInput);
	}
	else if (chunk > 0 && ticker == 0)
	{
		SetKeyboard(frames[chunk - 1][CHUNK_SIZE - 1].keyboardState, frames[chunk][ticker].keyboardState, kybdInput);
		SetMouse(frames[chunk - 1][CHUNK_SIZE - 1].keyboardState, frames[chunk][ticker].keyboardState, mouseInput);
	}
	else
	{
		SetKeyboard(frames[chunk][ticker - 1].keyboardState, frames[chunk][ticker].keyboardState, kybdInput);
		SetMouse(frames[chunk][ticker - 1].keyboardState, frames[chunk][ticker].keyboardState, mouseInput);
	}

	ticker++;
}