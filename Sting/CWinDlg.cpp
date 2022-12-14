#include "CWinDlg.h"
#include "AppDlg.h"

CWinDlg* CWinDlg::ptr = NULL;
//TCHAR szTextsArrParent[5][TEXT_SIZE];		// Тексты
HWND hParentWin/*, hEdit, hProgress*/;

CWinDlg::CWinDlg(void)
{
	ptr = this;
}

CWinDlg::~CWinDlg(void)
{

}

void CWinDlg::Cls_OnClose(HWND hwnd)
{
	EndDialog(hwnd, 0);
	EndDialog(hParentWin, 0);
}

BOOL CWinDlg::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	//_tcscpy_s(szTextsArrParent[0], TEXT("We hold these truths to be self evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness."));
	//_tcscpy_s(szTextsArrParent[1], TEXT("Lucas goes to school every day of the week. He has many subjects to go to each school day, English, art, science, mathematics, gym, and history. His mother packs a big backpack full of books and lunch for Lucas. His first class is English, and he likes that teacher very much.His English teacher says that he is a good pupil, which Lucas knows means that she thinks he is a good student."));
	//_tcscpy_s(szTextsArrParent[2], TEXT("When I was a child, I spake as a child, I understood as a child, I thought as a child, but when I became a man, I put away childish things. For now we see through a glass, darkly, but then face to face."));
	//_tcscpy_s(szTextsArrParent[3], TEXT("Move him into the sun Gently its touch awoke him once, At home, whispering of fields unsown. Always it woke him, even in France, Until this morning and this snow. If anything might rouse him now The kind old sun will know."));
	//_tcscpy_s(szTextsArrParent[4], TEXT("If you can keep your head when all about you Are losing theirs and blaming it on you, If you can trust yourself when all men doubt you, But make allowance for their doubting too. If you can wait and not be tired by waiting."));
	return TRUE;
}

void CWinDlg::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{

}

BOOL CALLBACK CWinDlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	hParentWin = GetParent(hwnd);
	//hEdit = GetDlgItem(hParent, IDC_EDIT1);
	//hProgress = GetDlgItem(hParent, IDC_PROGRESS1);
	//TCHAR textEditParent[TEXT_SIZE];

	if (LOWORD(wParam) == IDC_WIN_EXIT)
	{
		EndDialog(hwnd, 0);
		EndDialog(hParentWin, 0);
	}
	else if (LOWORD(wParam) == IDC_WIN_RANDOM)
	{
		//srand(time(NULL));
		//SetWindowText(hEdit, szTextsArrParent[(rand() % 5)]);

		//SendMessage(hProgress, PBM_SETPOS, 0, 0);
		////_tcscpy_s(textEdit, TEXT("123lkj"));
		//GetWindowText(hParent, textEditParent, TEXT_SIZE);
		//int textEditLength = _tcslen(textEditParent);
		//
		//SendMessage(hProgress, PBM_SETPOS, 0, 0);
		//// Устанавливаю диапазон прогресс бара
		//SendMessage(hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, textEditLength));


		EndDialog(hwnd, 0);
	}
	//else if (LOWORD(wParam) == IDC_WIN_RESTART)
	//{
	//	EndDialog(hParent, 0);
	//	EndDialog(hwnd, 0);
	//}
	/*if (LOWORD(wParam) == IDC_WIN_EXIT)
	{*/
	//	EndDialog(hwnd, 0);
	//	EndDialog(hParent, 0);
	//}
	//if (LOWORD(wParam) == IDC_WIN_RESTART)
	//{
	//	/*srand(time(NULL));
	//	_tcscpy_s(textEdit, szTextsArr[(rand() % 5)]);

	//	_tcscpy_s(currentText, textEdit);
	//	uMistakes = 0;
	//	swprintf_s(textMistakes, TEXT("%d"), uMistakes);
	//	SetWindowText(hMistakes, textMistakes);
	//	uProgressbarPosition = 0;
	//	SendMessage(hProgress, PBM_SETPOS, uProgressbarPosition, 0);
	//	SetWindowText(hEditText, textEdit);
	//	EndDialog(hwnd, 0);*/
	//}

	switch (message)
	{
		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
		HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, ptr->Cls_OnCommand);
	}
	return FALSE;
}

