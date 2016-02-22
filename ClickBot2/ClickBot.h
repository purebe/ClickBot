// ClickBot.h

#pragma once

#include <Windows.h>
#include <fstream>
#include <vector>

#include "StateFunctions.h"

struct Frame
{
	POINT mouseState;
	bool keyboardState[256];
};

class ClickBot
{
private:
	const unsigned int CHUNK_SIZE;

	unsigned int ticker;
	unsigned int max_ticker;
	unsigned int chunk;
	unsigned int max_chunk;

	bool loop;
	bool playing, recording;

	INPUT kybdInput, mouseInput;
	std::vector<Frame*> frames;

public:
	ClickBot();
	~ClickBot();

	void BeginRec();
	void StopRec();

	void BeginPlay();
	void StopPlay();

	void RecordTick();
	void PlayTick();

	void SetLoop(bool loop) { this->loop = loop; }
	bool IsLooping() { return loop; }
	bool IsPlaying() { return playing; }
	bool IsRecording() { return recording; }
};