#pragma once
#include "header.h"

DWORD WINAPI Thread_SPM(LPVOID lp);
DWORD WINAPI Thread_Timer(LPVOID lp);
LRESULT CALLBACK KeyboardProc(int idCode, WPARAM wParam, LPARAM lParam);

class AppDlg
{
public:
	AppDlg(void);
public:
	static AppDlg* ptr;
	~AppDlg(void);
	static BOOL CALLBACK DlgProc(HWND hWnd, UINT mes, WPARAM wp, LPARAM lp);
	BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
	void Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	
	void Cls_OnClose(HWND hwnd);
	void OnFont();
	void Cls_OnSize(HWND hwnd, UINT state, int cx, int cy);
	void OnTrayIcon(WPARAM wp, LPARAM lp); // обработчик пользовательского сообщени€
};
