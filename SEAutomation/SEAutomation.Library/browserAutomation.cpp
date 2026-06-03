#include "pch.h"
#include "browserAutomation.h"
#include <Windows.h>
#include <shellapi.h>
#include <string>
#include <thread>
#include <iostream>

namespace
{
	void Log(const std::wstring& msg)
	{
		std::wcout << L"[INFO] " << msg << std::endl;
	}

	void SleepMs(int ms)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	}

	void KeyEvent(WORD vk, bool keyUp = false)
	{
		INPUT in{};
		in.type = INPUT_KEYBOARD;
		in.ki.wVk = vk;
		if (keyUp) in.ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, &in, sizeof(INPUT));
	}

	void KeyPress(WORD vk, int delay = 50)
	{
		KeyEvent(vk, false);
		KeyEvent(vk, true);
		SleepMs(delay);
	}

	void KeyCombo(WORD mod, WORD key, int delay = 100)
	{
		KeyEvent(mod, false);
		KeyEvent(key, false);

		KeyEvent(key, true);
		KeyEvent(mod, true);

		SleepMs(delay);
	}

	HWND WaitForWindow(const wchar_t* cls, int timeoutMs = 10000)
	{
		HWND hwnd = nullptr;
		int waited = 0;

		while (!hwnd && waited < timeoutMs)
		{
			hwnd = FindWindowW(cls, nullptr);
			if (hwnd) break;

			SleepMs(250);
			waited += 250;
		}

		return hwnd;
	}

	void Focus(HWND hwnd)
	{
		if (!hwnd) return;

		ShowWindow(hwnd, SW_RESTORE);
		SetForegroundWindow(hwnd);
		SleepMs(300);
	}

	void TypeAsciiLike(const std::wstring& text)
	{
		for (wchar_t c : text)
		{
			SHORT vk = VkKeyScanW(c);
			if (vk == -1) continue;

			BYTE key = LOBYTE(vk);
			bool shift = vk & 0x0100;

			if (shift) KeyEvent(VK_SHIFT, false);

			KeyEvent(key, false);
			KeyEvent(key, true);

			if (shift) KeyEvent(VK_SHIFT, true);

			SleepMs(8);
		}
	}

	void PressAltF4()
	{
		INPUT in[4]{};

		in[0].type = INPUT_KEYBOARD;
		in[0].ki.wVk = VK_MENU;

		in[1].type = INPUT_KEYBOARD;
		in[1].ki.wVk = VK_F4;

		in[2].type = INPUT_KEYBOARD;
		in[2].ki.wVk = VK_F4;
		in[2].ki.dwFlags = KEYEVENTF_KEYUP;

		in[3].type = INPUT_KEYBOARD;
		in[3].ki.wVk = VK_MENU;
		in[3].ki.dwFlags = KEYEVENTF_KEYUP;

		SendInput(4, in, sizeof(INPUT));
	}
}

bool BrowserAutomation::Execute()
{
	Log(L"Step 0: Terminating existing Edge processes...");
	system("taskkill /IM msedge.exe /F >nul 2>&1");
	SleepMs(2000);

	Log(L"Step 1: Launching Microsoft Edge...");
	auto res = ShellExecuteW(nullptr, L"open", L"msedge.exe", nullptr, nullptr, SW_SHOW);

	if ((int)res <= 32)
	{
		Log(L"ERROR: Failed to launch Edge.");
		return false;
	}

	SleepMs(3000);

	Log(L"Step 2: Waiting for Edge window...");
	HWND hwnd = WaitForWindow(L"Chrome_WidgetWin_1", 8000);

	if (!hwnd)
	{
		Log(L"ERROR: Edge window not found.");
		return false;
	}

	Log(L"Step 3: Focusing Edge window...");
	Focus(hwnd);

	Log(L"Step 4: Opening address bar (Ctrl + L)...");
	KeyCombo(VK_CONTROL, 'L', 200);

	Log(L"Step 5: Navigating to settings page...");
	TypeAsciiLike(L"edge://settings/privacy/services/search");
	KeyPress(VK_RETURN, 300);

	SleepMs(3500);

	Log(L"Step 6: Re-focusing Edge...");
	Focus(hwnd);

	Log(L"Step 7: Exiting address bar (ESC)...");
	KeyPress(VK_ESCAPE, 200);

	Log(L"Step 8: Navigating UI (TAB x4)...");
	for (int i = 0; i < 4; i++)
		KeyPress(VK_TAB, 200);

	Log(L"Step 9: Opening dropdown...");
	KeyPress(VK_RETURN, 300);

	Log(L"Step 10: Selecting browser option...");
	int dropDownIndex = 1;							//Alter this in IDA
	for (int i = 0; i < dropDownIndex; i++)
	{
		KeyPress(VK_DOWN, 100);
	}

	Log(L"Step 10: Pressing enter to save browser option...");
	KeyPress(VK_RETURN, 100);

	Log(L"Step 11: Closing Edge...");
	SleepMs(2000);
	PressAltF4();

	Log(L"Automation completed successfully.");
	return true;
}