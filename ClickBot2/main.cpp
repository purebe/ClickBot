#include <Windows.h>
#include <string>

#include "StateFunctions.h"
#include "ClickBot.h"

#pragma comment(lib, "winmm")

struct rect
{
	rect(int x, int y, int w, int h)
	{
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
	}

	int x, y, w, h;
};

const wchar_t g_szClassName[] = L"ClickBot";

const int IDC_BEGINREC           = 101;
const int IDC_STOPREC            = 102;
const int IDC_BEGINPLAY          = 103;
const int IDC_STOPPLAY           = 104;
const int IDC_SAVE               = 105;
const int IDC_LOAD               = 106;
const int IDC_LOOP               = 107;
const int IDC_RANDOMIZEMOUSEMOVE = 108;

const int HTKEY_BEGINREC         = 201;
const int HTKEY_STOPREC          = 202;
const int HTKEY_BEGINPLAY        = 203;
const int HTKEY_STOPPLAY         = 204;

const int REC_TIMER              = 1;
const int PLAY_TIMER             = 2;

const int VK_R_KEY               = 0x52;
const int VK_T_KEY               = 0x54;
const int VK_F_KEY               = 0x46;
const int VK_G_KEY               = 0x47;

ClickBot cb;
MMRESULT recordTimerId, playTimerId, prerecordTimerId;

void BeginRecording();
void StopRecording();
void BeginPlaying();
void StopPlaying();

bool CreateHotkey(HWND hwnd, int id, int modifier, int key)
{
	int ret = RegisterHotKey(hwnd, id, modifier, key);

	return (ret != 0);
}

HWND CreateControl(std::wstring type, long style, long flags, std::wstring label, rect rc, int id, HFONT font, HWND hwnd)
{
	HWND handle = CreateWindowEx(style, type.c_str(), label.c_str(), WS_CHILD | WS_VISIBLE | flags, rc.x, rc.y, rc.w, rc.h, hwnd, (HMENU)id, GetModuleHandle(NULL), NULL);
	SendMessage(handle, WM_SETFONT, (WPARAM)font, MAKELPARAM(FALSE, 0));

	return handle;
}

HWND CreateButton(std::wstring label, rect rc, int id, HFONT font, HWND hwnd)
{
	return CreateControl(L"BUTTON", WS_EX_CLIENTEDGE, 0, label, rc, id, font, hwnd);
}

HWND CreateCheckbox(std::wstring label, rect rc, int id, HFONT font, HWND hwnd)
{
	return CreateControl(L"BUTTON", 0, BS_AUTOCHECKBOX, label, rc, id, font, hwnd);
}

HWND CreateStatic(std::wstring label, rect rc, HFONT font, HWND hwnd)
{
	return CreateControl(L"STATIC", 0, 0, label, rc, 0, font, hwnd);
}

HWND CreateLabel(std::wstring label, rect rc, HFONT font, HWND hwnd)
{
	return CreateControl(L"STATIC", WS_EX_STATICEDGE, 0, label, rc, 0, font, hwnd);
}

void CreateWindowLayout(HWND hwnd)
{
	HFONT hFont;
	hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

	CreateButton(L"Begin Recording", rect(4, 5, 120, 25), IDC_BEGINREC, hFont, hwnd);
	CreateButton(L"Stop Recording", rect(4, 35, 120, 25), IDC_STOPREC, hFont, hwnd);
	CreateButton(L"Begin Playing", rect(127, 5, 120, 25), IDC_BEGINPLAY, hFont, hwnd);
	CreateButton(L"Stop Playing", rect(127, 35, 120, 25), IDC_STOPPLAY, hFont, hwnd);
	CreateButton(L"Save", rect(4, 65, 120, 25), IDC_SAVE, hFont, hwnd);
	CreateButton(L"Load", rect(127, 65, 120, 25), IDC_LOAD, hFont, hwnd);
	CreateLabel(L"BEGIN REC: CTRL-R", rect(4, 95, 120, 18), hFont, hwnd);
	CreateLabel(L"STOP REC: CTRL-T", rect(127, 95, 120, 18), hFont, hwnd);
	CreateLabel(L"BEGIN PLAY: CTRL-F", rect(4, 125, 120, 18), hFont, hwnd);
	CreateLabel(L"STOP PLAY: CTRL-G", rect(127, 125, 120, 18), hFont, hwnd);
	CreateCheckbox(L"Loop", rect(9, 155, 45, 25), IDC_LOOP, hFont, hwnd);
}

void RegisterHotkeys(HWND hwnd)
{
	bool beginRec  = CreateHotkey(hwnd, HTKEY_BEGINREC, MOD_CONTROL, VK_R_KEY);
	bool stopRec   = CreateHotkey(hwnd, HTKEY_STOPREC, MOD_CONTROL, VK_T_KEY);
	bool beginPlay = CreateHotkey(hwnd, HTKEY_BEGINPLAY, MOD_CONTROL, VK_F_KEY);
	bool stopPlay  = CreateHotkey(hwnd, HTKEY_STOPPLAY, MOD_CONTROL, VK_G_KEY);
	
	std::wstring status(L"Could not register\n\n");
	if (!beginRec)
	{
		status += L"`Begin Recording`\n";
	}
	if (!stopRec)
	{
		status += L"`Stop Recording`\n";
	}
	if (!beginPlay)
	{
		status += L"`Begin Play`\n";
	}
	if (!stopPlay)
	{
		status += L"`Stop Play`\n";
	}
	status += L"\nhotkey(s)!";

	if (!beginRec || !stopRec || !beginPlay || !stopPlay)
	{
		MessageBox(NULL, status.c_str(), L"Error registering hotkey", MB_OK);
	}
}

void UnregisterHotkeys(HWND hwnd)
{
   	UnregisterHotKey(hwnd, HTKEY_BEGINPLAY);
   	UnregisterHotKey(hwnd, HTKEY_STOPREC);
   	UnregisterHotKey(hwnd, HTKEY_BEGINPLAY);
	UnregisterHotKey(hwnd, HTKEY_STOPPLAY);
}

void CALLBACK prerecordTimer(UINT uTimerID, UINT uMsg, DWORD_PTR param, DWORD_PTR dw1, DWORD_PTR dw2)
{
	bool keys[256];
	ReadKeyboard(keys);
	if (!keys[VK_R_KEY])
	{
		BeginRecording();
		timeKillEvent(prerecordTimerId);
	}
}

void CALLBACK recordTimer(UINT uTimerID, UINT uMsg, DWORD_PTR param, DWORD_PTR dw1, DWORD_PTR dw2)
{
	cb.RecordTick();
}

void CALLBACK playTimer(UINT uTimerID, UINT uMsg, DWORD_PTR param, DWORD_PTR dw1, DWORD_PTR dw2)
{
	cb.PlayTick();
	if (!cb.IsPlaying())
	{
		StopPlaying();
	}
}

void BeginPreRecord()
{
	prerecordTimerId = timeSetEvent(1, 0, prerecordTimer, 0, TIME_PERIODIC);
}

void BeginRecording()
{
	cb.BeginRec();
	recordTimerId = timeSetEvent(1, 0, recordTimer, 0, TIME_PERIODIC);
}

void StopRecording()
{
	cb.StopRec();
	timeKillEvent(recordTimerId);
}

void BeginPlaying()
{
	cb.BeginPlay();
	playTimerId = timeSetEvent(1, 0, playTimer, 0, TIME_PERIODIC);
}

void StopPlaying()
{
	cb.StopPlay();
	timeKillEvent(playTimerId);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		CreateWindowLayout(hwnd);
		RegisterHotkeys(hwnd);
		break;
	case WM_HOTKEY:
		switch (LOWORD(wParam))
		{
		case HTKEY_BEGINREC:
			BeginPreRecord();
			break;
		case HTKEY_STOPREC:
			StopRecording();
			break;
		case HTKEY_BEGINPLAY:
			BeginPlaying();
			break;
		case HTKEY_STOPPLAY:
			StopPlaying();
			break;
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BEGINREC:
			BeginRecording();
			break;
		case IDC_STOPREC:
			StopRecording();
			break;
		case IDC_BEGINPLAY:
			BeginPlaying();
			break;
		case IDC_STOPPLAY:
			StopPlaying();
			break;
		case IDC_LOOP:
			if (cb.IsLooping())
			{
				cb.SetLoop(false);
			}
			else
			{
				cb.SetLoop(true);
			}
			break;
		}
		break;
	case WM_CLOSE:
		UnregisterHotkeys(hwnd);

		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		UnregisterHotkeys(hwnd);

		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

	//Step 1: Registering the Window Class
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Window Registration Failed!", L"Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// Step 2: Creating the Window
	hwnd = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		g_szClassName,
		L"ClickBot",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 270, 450,
		NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)
	{
		MessageBox(NULL, L"Window Creation Failed!", L"Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	// Step 3: The Message Loop
	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}