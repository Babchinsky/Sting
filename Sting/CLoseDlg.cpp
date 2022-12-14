#include "CLoseDlg.h"
#include "AppDlg.h"

CLoseDlg* CLoseDlg::ptr = NULL;
HWND hParentLose;

CLoseDlg::CLoseDlg(void)
{
	ptr = this;
}

CLoseDlg::~CLoseDlg(void)
{

}

void CLoseDlg::Cls_OnClose(HWND hwnd)
{
	EndDialog(hwnd, 0);
	EndDialog(hParentLose, 0);
}

BOOL CLoseDlg::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	return TRUE;
}

void CLoseDlg::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{

}

BOOL CALLBACK CLoseDlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	hParentLose = GetParent(hwnd);


	if (LOWORD(wParam) == IDC_LOSE_EXIT)
	{
		EndDialog(hwnd, 0);
		EndDialog(hParentLose, 0);
	}
	else if (LOWORD(wParam) == IDC_LOSE_RANDOM)
	{
		EndDialog(hwnd, 0);
	}


	switch (message)
	{
		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
		HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, ptr->Cls_OnCommand);
	}
	return FALSE;
}

