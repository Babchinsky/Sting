#define WM_ICON WM_APP
#define ID_TRAYICON WM_USER
#include "AppDlg.h"
#include "CWinDlg.h"
#include "CLoseDlg.h"

AppDlg* AppDlg::ptr = NULL;

HHOOK hHook; // хук для отслеживания клавиатуры
HANDLE hndlTime; // поток таймера
HANDLE hndlSPM; // поток таймера

TCHAR textEdit[TEXT_SIZE];					// Текст в EditBox
int textLength;								// Длина текста
TCHAR currentText[TEXT_SIZE];				// Текст, который загружен. 
TCHAR szTextsArr[5][TEXT_SIZE];				// Тексты
TCHAR textMistakes[TEXT_SIZE];				// Текст для ошибок
TCHAR szEditTextKeyboardProc[TEXT_SIZE];	// Текст в EditBox для KeyboardProc

TCHAR textSPM[TEXT_SIZE];
int spm;

HWND hDialog;		// дескриптор нашего окна
HWND hEdit;			// дескриптор нашего EditBox
HWND hProgress;		// дескриптор нашего ProgressBar
HWND hMistakes;		// дескриптор нашего Static с ошибками
HWND hStaticTime;	// дескриптор нашего Static с временем
HWND hStaticSPM;	// дескриптор нашего Static с нажатиями

WPARAM wp;

HMENU hMenu;

short minutes;
short seconds;
UINT uMistakes;	
UINT uProgressbarPosition;

HICON hIcon;
PNOTIFYICONDATA pNID;

COLORREF color;
LOGFONT lf;
HFONT hFont;

HANDLE hMutex;
// Цвета
COLORREF black = RGB(0, 0, 0);	//текущий цвет текста - чёрный
COLORREF green = RGB(124, 252, 0);// цвет текста - зелёный
COLORREF red = RGB(170, 74, 68);  // цвет текста - красный

UINT clicks;

HWND ButtonQ, ButtonW, ButtonE, ButtonR, ButtonT, ButtonY, ButtonU, ButtonI, ButtonO, ButtonP, ButtonSquareBracketLeft, ButtonSquareBracketRight;
HWND ButtonA, ButtonS, ButtonD, ButtonF, ButtonG, ButtonH, ButtonJ, ButtonK, ButtonL;
HWND ButtonZ, ButtonX, ButtonC, ButtonV, ButtonB, ButtonN, ButtonM, ButtonComma, ButtonDot;
HWND ButtonSpace;

AppDlg::AppDlg(void)
{
	ptr = this;
	pNID = new NOTIFYICONDATA;
}

AppDlg::~AppDlg(void)
{
	delete pNID;
	ReleaseMutex(hMutex);
}

BOOL CALLBACK AppDlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	wp = wParam;

	switch (message)
	{
		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
		HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, ptr->Cls_OnCommand);
		HANDLE_MSG(hwnd, WM_SIZE, ptr->Cls_OnSize);
	}


	// Смена цвета текста ошибки на красный
	if (message == WM_CTLCOLORSTATIC)  //WM_CTLCOLORSTATIC вместо WM_CTLCOLOREDIT, ибо ReadOnly
	{
		if ((HWND)lParam == GetDlgItem(hwnd, IDC_EDIT1))
		{
			HDC hdc = (HDC)wParam;
			SetTextColor((HDC)wParam, color);//установим цвет текста
			//SetBkMode(hdc, TRANSPARENT);
			return (int)GetStockObject(NULL_BRUSH);
		}

		if ((HWND)lParam == GetDlgItem(hwnd, IDC_STATIC_MISTAKES))
		{
			HDC hdc = (HDC)wParam;
			//SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, red);
			return (int)GetStockObject(NULL_BRUSH);
		}
	}


	// пользовательское сообщение
	if (message == WM_ICON)
	{
		ptr->OnTrayIcon(wParam, lParam);
		return TRUE;
	}
	return FALSE;
}

BOOL AppDlg::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	TCHAR GUID[] = TEXT("{D99CD3E0-670D-4def-9B74-99FD7E793DFB}");
	hMutex = CreateMutex(NULL, FALSE, GUID);
	DWORD dwAnswer = WaitForSingleObject(hMutex, 0);
	if (dwAnswer == WAIT_TIMEOUT)
	{
		MessageBox(hwnd, TEXT("Нельзя запускать более одной копии приложения!!!"), TEXT("Sting"), MB_OK | MB_ICONINFORMATION);
		EndDialog(hwnd, 0);
	}


	hMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU1));
	// Присоединим меню к главному окну приложения
	SetMenu(hwnd, hMenu);

	// Получим дескриптор экземпляра приложения
	HINSTANCE hInst = GetModuleHandle(NULL);

	hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1)); // загружаем иконку
	SetClassLong(hDialog, GCL_HICON, LONG(hIcon)); // устанавливаем иконку в главном окне приложения
	memset(pNID, 0, sizeof(NOTIFYICONDATA)); //Обнуление структуры
	pNID->cbSize = sizeof(NOTIFYICONDATA); //размер структуры
	pNID->hIcon = hIcon; //загружаем пользовательскую иконку
	pNID->hWnd = hwnd; //дескриптор окна, которое будет получать уведомляющие сообщения,
	pNID->uCallbackMessage = WM_ICON; // Пользовательское сообщение


	pNID->uFlags = NIF_TIP | NIF_ICON | NIF_MESSAGE | NIF_INFO;
	pNID->uID = ID_TRAYICON; // предопределённый идентификатор иконки

	_tcscpy_s(szTextsArr[0], TEXT("We hold these truths to be self evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness."));
	_tcscpy_s(szTextsArr[1], TEXT("Lucas goes to school every day of the week. He has many subjects to go to each school day, English, art, science, mathematics, gym, and history. His mother packs a big backpack full of books and lunch for Lucas. His first class is English, and he likes that teacher very much.His English teacher says that he is a good pupil, which Lucas knows means that she thinks he is a good student."));
	_tcscpy_s(szTextsArr[2], TEXT("When I was a child, I spake as a child, I understood as a child, I thought as a child, but when I became a man, I put away childish things. For now we see through a glass, darkly, but then face to face."));
	_tcscpy_s(szTextsArr[3], TEXT("Move him into the sun Gently its touch awoke him once, At home, whispering of fields unsown. Always it woke him, even in France, Until this morning and this snow. If anything might rouse him now The kind old sun will know."));
	_tcscpy_s(szTextsArr[4], TEXT("If you can keep your head when all about you Are losing theirs and blaming it on you, If you can trust yourself when all men doubt you, But make allowance for their doubting too. If you can wait and not be tired by waiting."));

	
	uProgressbarPosition = 1;
	// Получим дескрипторы элементов управления
	hDialog = hwnd;
	hEdit = GetDlgItem(hwnd, IDC_EDIT1);
	hProgress = GetDlgItem(hwnd, IDC_PROGRESS1);
	hMistakes = GetDlgItem(hwnd, IDC_STATIC_MISTAKES);
	hStaticTime = GetDlgItem(hwnd, IDC_STATIC_TIME);
	hStaticSPM = GetDlgItem(hwnd, IDC_STATIC_SPM);
	uMistakes = 0;


	ButtonQ = GetDlgItem(hDialog, IDC_BUTTON15a);
	ButtonW = GetDlgItem(hDialog, IDC_BUTTON16a);
	ButtonE = GetDlgItem(hDialog, IDC_BUTTON17a);
	ButtonR = GetDlgItem(hDialog, IDC_BUTTON18a);
	ButtonT = GetDlgItem(hDialog, IDC_BUTTON19a);
	ButtonY = GetDlgItem(hDialog, IDC_BUTTON20a);
	ButtonU = GetDlgItem(hDialog, IDC_BUTTON21a);
	ButtonI = GetDlgItem(hDialog, IDC_BUTTON22a);
	ButtonO = GetDlgItem(hDialog, IDC_BUTTON23a);
	ButtonP = GetDlgItem(hDialog, IDC_BUTTON24a);
	ButtonSquareBracketLeft = GetDlgItem(hDialog, IDC_BUTTON25a);
	ButtonSquareBracketRight = GetDlgItem(hDialog, IDC_BUTTON26a);


	ButtonA = GetDlgItem(hDialog, IDC_BUTTON29a);
	ButtonS = GetDlgItem(hDialog, IDC_BUTTON30a);
	ButtonD = GetDlgItem(hDialog, IDC_BUTTON31a);
	ButtonF = GetDlgItem(hDialog, IDC_BUTTON32a);
	ButtonG = GetDlgItem(hDialog, IDC_BUTTON33a);
	ButtonH = GetDlgItem(hDialog, IDC_BUTTON34a);
	ButtonJ = GetDlgItem(hDialog, IDC_BUTTON35a);
	ButtonK = GetDlgItem(hDialog, IDC_BUTTON36a);
	ButtonL = GetDlgItem(hDialog, IDC_BUTTON37a);

	ButtonZ = GetDlgItem(hDialog, IDC_BUTTON42a);
	ButtonX = GetDlgItem(hDialog, IDC_BUTTON43a);
	ButtonC = GetDlgItem(hDialog, IDC_BUTTON44a);
	ButtonV = GetDlgItem(hDialog, IDC_BUTTON45a);
	ButtonB = GetDlgItem(hDialog, IDC_BUTTON46a);
	ButtonN = GetDlgItem(hDialog, IDC_BUTTON47a);
	ButtonM = GetDlgItem(hDialog, IDC_BUTTON48a);
	ButtonComma = GetDlgItem(hDialog, IDC_BUTTON49a);
	ButtonDot = GetDlgItem(hDialog, IDC_BUTTON50a);

	ButtonSpace = GetDlgItem(hDialog, IDC_BUTTON56a);



	hndlTime = CreateThread(NULL, 0, Thread_Timer, hStaticTime, 0, NULL);
	hndlSPM = CreateThread(NULL, 0, Thread_SPM, hStaticSPM, 0, NULL);


	minutes = 3;
	seconds = 0;

	srand(time(NULL));
	_tcscpy_s(textEdit, szTextsArr[(rand() % 5)]);
	//_tcscpy_s(textEdit, TEXT("EZ"));
	_tcscpy_s(currentText, textEdit);
	//_tcscpy_s(textEdit, szTextsArr[2]);
	//_tcscpy_s(textEdit, TEXT("123lkj"));
	int textEditLength = _tcslen(textEdit);
	// Устанавливаю диапазон прогресс бара
	SendMessage(hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, textEditLength));



	////GetWindowText(hEdit, textEdit, TEXT_SIZE);
	SetWindowText(hEdit, textEdit);

	hHook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, NULL /* для собственного потока */, GetCurrentThreadId());

	memset(&lf, 0, sizeof(lf));//обнуляем структуру LOGFONT
	lf.lfHeight = 30; // see PS
	hFont = CreateFontIndirect(&lf);//создаём новый шрифт с характеристиками по умолчанию
	SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

	SetFocus(hEdit);

	return TRUE;
}

DWORD WINAPI Thread_SPM(LPVOID lp)
{
	while (true)
	{
		clicks = 0;
		Sleep(5000);
		spm = clicks * 12;

		wsprintf(textSPM, TEXT("%d"), spm);
		SetWindowText(hStaticSPM, textSPM);
	}
	return 0;
}

DWORD WINAPI Thread_Timer(LPVOID lp)
{
	HWND hStaticTime = HWND(lp);

	TCHAR textMyTime[255];
	TCHAR textSeconds[255];

	while (true)
	{
		seconds--;

		if (minutes == 0 && seconds == -1) break;

		if (seconds == -1)
		{
			minutes--;
			seconds = 59;
		}

		if (minutes < 11) _stprintf_s(textMyTime, TEXT("0%d"), minutes);
		else _stprintf_s(textMyTime, TEXT("%d"), minutes);


		if (seconds < 11) wcscat_s(textMyTime, 255, TEXT(":0"));
		else wcscat_s(textMyTime, 255, TEXT(":"));


		_stprintf_s(textSeconds, TEXT("%d"), seconds);
		wcscat_s(textMyTime, 255, textSeconds);

		SetWindowText(hStaticTime, textMyTime);
		Sleep(980);

		/*float sum = 0;
		for (int i = 0; i < BUFFER_SIZE; ++i) {
			sum += counters[i];
		}

		int spm = sum / BUFFER_SIZE;
		TCHAR textSPM[TEXT_SIZE];
		wsprintf(textSPM, TEXT("0X%X"), spm);

		HWND hSPM = GetDlgItem(hDialog, IDC_STATIC_SPM);
		SetWindowText(hSPM, textSPM);*/


		

		if (minutes == 0 && seconds == 0)
		{
			CLoseDlg dlg;
			MessageBeep(MB_ICONSTOP);
			INT_PTR result = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_LOSE), hDialog, CLoseDlg::DlgProc);

			minutes = 3;
			seconds = 0;
			//ResumeThread(hndlTime);
			srand(time(NULL));

			_tcscpy_s(textEdit, szTextsArr[(rand() % 5)]);



			_tcscpy_s(currentText, textEdit);
			uMistakes = 0;
			swprintf_s(textMistakes, TEXT("%d"), uMistakes);
			SetWindowText(hMistakes, textMistakes);

			textLength = _tcsclen(textEdit);

			uProgressbarPosition = 0;
			SendMessage(hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, textLength));
			SendMessage(hProgress, PBM_SETPOS, uProgressbarPosition, 0);
			SetWindowText(hEdit, textEdit);
		}
	}
	return 0;
}

LRESULT CALLBACK KeyboardProc(int idCode, WPARAM wParam, LPARAM lParam)
{
	/*idCode - если параметр равен HC_ACTION, то процедура должна обработать HOOK,
	в противном случае (если < 0) сообщение нужно передать функции CallNextHookEx
	для дальнейшей обработки, и возвратить значение, полученное от этой функции.
	wParam - код виртуальной клавиши
	lParam - кратность повторения, скэн-код, 31 бит - если нажата, то 1, иначе 0.*/
	
	if (HC_ACTION == idCode)
	{
		TCHAR szMistakes[TEXT_SIZE];
		HWND hWnd = GetFocus();
		static TCHAR buf[40];
		GetClassName(hWnd, buf, 40);

		if (lstrcmpi(buf, TEXT("EDIT")) == 0 && (lParam & 0x80000000)) // клавиша отпущена!!!
		{
			if (wParam <= 32) // непечатаемый символ
			{
				wsprintf(buf, TEXT("0X%X"), wParam);
			}
			else
			{
				if (!(wParam >= 'A' && wParam <= 'Z')) // цифры и некоторые символы
				{
					wsprintf(buf, TEXT("%c"), wParam);
					
				}
					
				else
				{
					short shift = GetKeyState(VK_SHIFT); // если старший бит равен 1, то клавиша нажата
					short caps = GetKeyState(VK_CAPITAL); // если младший бит равен 1, то клавиша переключена
					short space = GetKeyState(VK_SPACE); // если младший бит равен 1, то клавиша переключена
					if ((shift & 0x8000) && !(caps & 1) || !(shift & 0x8000) && (caps & 1))
					{
						wsprintf(buf, TEXT("%c"), wParam);
						
					}
					else
					{
						wsprintf(buf, TEXT("%c"), tolower(wParam));
					}


					
					
				}
				

			}

			//MessageBeep(0xFFFFFFFF);
			//Sleep(500);
			//MessageBeep(MB_ICONEXCLAMATION);
			//Sleep(500);
			//MessageBeep(MB_ICONERROR);
			//Sleep(500);
			//MessageBeep(MB_ICONHAND);
			//Sleep(500);
			//MessageBeep(MB_ICONINFORMATION);
			//Sleep(500);
			//MessageBeep(MB_ICONQUESTION);
			//Sleep(500);
			//MessageBeep(MB_ICONSTOP);
			//Sleep(500);
			//MessageBeep(MB_ICONWARNING);
			//Sleep(500);
			//MessageBeep(MB_OK);


			if (tolower(buf[0]) == TEXT('q')) SendMessage(ButtonQ, WM_LBUTTONDOWN, 0, 0);
			if (tolower(buf[0]) == TEXT('w')) SendMessage(ButtonW, WM_LBUTTONDOWN, 0, 0);
			if (tolower(buf[0]) == TEXT('e')) SendMessage(ButtonE, WM_LBUTTONDOWN, 0, 0);
			if (tolower(buf[0]) == TEXT('r')) SendMessage(ButtonR, WM_LBUTTONDOWN, 0, 0);
			if (tolower(buf[0]) == TEXT('t')) SendMessage(ButtonT, WM_LBUTTONDOWN, 0, 0);
			if (tolower(buf[0]) == TEXT('y')) SendMessage(ButtonY, WM_LBUTTONDOWN, 0, 0);
			if (tolower(buf[0]) == TEXT('u')) SendMessage(ButtonU, WM_LBUTTONDOWN, 0, 0);
			if (tolower(buf[0]) == TEXT('i')) SendMessage(ButtonI, WM_LBUTTONDOWN, 0, 0);
			if (tolower(buf[0]) == TEXT('o')) SendMessage(ButtonO, WM_LBUTTONDOWN, 0, 0);
			if (tolower(buf[0]) == TEXT('p')) SendMessage(ButtonP, WM_LBUTTONDOWN, 0, 0);


			if (tolower(buf[0]) == TEXT('a')) SendMessage(ButtonA, WM_LBUTTONDOWN, 0, 0);
			if (tolower(buf[0]) == TEXT('s')) SendMessage(ButtonS, WM_LBUTTONDOWN, 0, 0);
			if (tolower(buf[0]) == TEXT('d')) SendMessage(ButtonD, WM_LBUTTONDOWN, 0, 0);
			if (tolower(buf[0]) == TEXT('f')) SendMessage(ButtonF, WM_LBUTTONDOWN, 0, 0);
			if (tolower(buf[0]) == TEXT('g')) SendMessage(ButtonG, WM_LBUTTONDOWN, 0, 0);
			if (tolower(buf[0]) == TEXT('h')) SendMessage(ButtonH, WM_LBUTTONDOWN, 0, 0);
			if (tolower(buf[0]) == TEXT('j')) SendMessage(ButtonJ, WM_LBUTTONDOWN, 0, 0);
			if (tolower(buf[0]) == TEXT('k')) SendMessage(ButtonK, WM_LBUTTONDOWN, 0, 0);
			if (tolower(buf[0]) == TEXT('l')) SendMessage(ButtonL, WM_LBUTTONDOWN, 0, 0);

			if (tolower(buf[0]) == TEXT('z')) SendMessage(ButtonZ, WM_LBUTTONDOWN, 0, 0);
			if (tolower(buf[0]) == TEXT('x')) SendMessage(ButtonX, WM_LBUTTONDOWN, 0, 0);
			if (tolower(buf[0]) == TEXT('c')) SendMessage(ButtonC, WM_LBUTTONDOWN, 0, 0);
			if (tolower(buf[0]) == TEXT('v')) SendMessage(ButtonV, WM_LBUTTONDOWN, 0, 0);
			if (tolower(buf[0]) == TEXT('b')) SendMessage(ButtonB, WM_LBUTTONDOWN, 0, 0);
			if (tolower(buf[0]) == TEXT('n')) SendMessage(ButtonN, WM_LBUTTONDOWN, 0, 0);
			if (tolower(buf[0]) == TEXT('m')) SendMessage(ButtonM, WM_LBUTTONDOWN, 0, 0);
			if (buf[0] == TEXT('¼')) SendMessage(ButtonComma, WM_LBUTTONDOWN, 0, 0);
			if (buf[0] == TEXT('¾')) SendMessage(ButtonDot, WM_LBUTTONDOWN, 0, 0);
			if ((buf[0] == TEXT('0') && buf[1] == TEXT('X') && buf[2] == TEXT('2') && buf[3] == TEXT('0'))) SendMessage(ButtonSpace, WM_LBUTTONDOWN, 0, 0);

			//MessageBox(hDialog, buf, buf, MB_OK);

			Sleep(50);
			SendMessage(ButtonQ, WM_LBUTTONUP, 0, 0);
			SendMessage(ButtonW, WM_LBUTTONUP, 0, 0);
			SendMessage(ButtonE, WM_LBUTTONUP, 0, 0);
			SendMessage(ButtonR, WM_LBUTTONUP, 0, 0);
			SendMessage(ButtonT, WM_LBUTTONUP, 0, 0);
			SendMessage(ButtonY, WM_LBUTTONUP, 0, 0);
			SendMessage(ButtonU, WM_LBUTTONUP, 0, 0);
			SendMessage(ButtonI, WM_LBUTTONUP, 0, 0);
			SendMessage(ButtonO, WM_LBUTTONUP, 0, 0);
			SendMessage(ButtonP, WM_LBUTTONUP, 0, 0);


			SendMessage(ButtonA, WM_LBUTTONUP, 0, 0);
			SendMessage(ButtonS, WM_LBUTTONUP, 0, 0);
			SendMessage(ButtonD, WM_LBUTTONUP, 0, 0);
			SendMessage(ButtonF, WM_LBUTTONUP, 0, 0);
			SendMessage(ButtonG, WM_LBUTTONUP, 0, 0);
			SendMessage(ButtonH, WM_LBUTTONUP, 0, 0);
			SendMessage(ButtonJ, WM_LBUTTONUP, 0, 0);
			SendMessage(ButtonK, WM_LBUTTONUP, 0, 0);
			SendMessage(ButtonL, WM_LBUTTONUP, 0, 0);

			SendMessage(ButtonZ, WM_LBUTTONUP, 0, 0);
			SendMessage(ButtonX, WM_LBUTTONUP, 0, 0);
			SendMessage(ButtonC, WM_LBUTTONUP, 0, 0);
			SendMessage(ButtonV, WM_LBUTTONUP, 0, 0);
			SendMessage(ButtonB, WM_LBUTTONUP, 0, 0);
			SendMessage(ButtonN, WM_LBUTTONUP, 0, 0);
			SendMessage(ButtonM, WM_LBUTTONUP, 0, 0);
			SendMessage(ButtonComma, WM_LBUTTONUP, 0, 0);
			SendMessage(ButtonDot, WM_LBUTTONUP, 0, 0);
			SendMessage(ButtonSpace, WM_LBUTTONUP, 0, 0);
			/*SendDlgItemMessage(hDialog, IDC_EDIT1, WM_SETFOCUS, 0, 0);*/
			SetFocus(hEdit);

			++clicks;

			// Меняем код пробела на символ пробела
			if (buf[0] == TEXT('0') && buf[1] == TEXT('X') && buf[2] == TEXT('2') && buf[3] == TEXT('0'))
				buf[0] = TEXT(' ');

			// Меняем код точки на символ точки
			if (buf[0] == TEXT('¾')) buf[0] = TEXT('.');

			// Меняем код запятой на символ запятой
			if (buf[0] == TEXT('¼')) buf[0] = TEXT(',');


			GetWindowText(hWnd, szEditTextKeyboardProc, TEXT_SIZE); // получаем текст

			if (_tcslen(szEditTextKeyboardProc) == 0)
			{
				// Останавливаем таймер
				SuspendThread(hndlTime);

				CWinDlg dlg;
				MessageBeep(MB_ICONINFORMATION);
				INT_PTR result = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_WIN), hDialog, CWinDlg::DlgProc);
				

				minutes = 3;
				seconds = 0;
				ResumeThread(hndlTime);
				srand(time(NULL));

				_tcscpy_s(textEdit, szTextsArr[(rand() % 5)]);



				_tcscpy_s(currentText, textEdit);
				uMistakes = 0;
				swprintf_s(textMistakes, TEXT("%d"), uMistakes);
				SetWindowText(hMistakes, textMistakes);

				textLength = _tcsclen(textEdit);

				uProgressbarPosition = 0;
				SendMessage(hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, textLength));
				SendMessage(hProgress, PBM_SETPOS, uProgressbarPosition, 0);
				SetWindowText(hEdit, textEdit);



				return 1;
			}

			// Проверка текста и отображение
			int textLength = _tcslen(szEditTextKeyboardProc);
			if (buf[0] == szEditTextKeyboardProc[0])
			{
				for (int i = 0; i < textLength - 1; ++i)
					szEditTextKeyboardProc[i] = szEditTextKeyboardProc[i + 1];
				szEditTextKeyboardProc[textLength - 1] = 0;

				// Шаг в прогресс баре
				SendMessage(hProgress, PBM_SETPOS, uProgressbarPosition++, 0);

				SetWindowText(hWnd, szEditTextKeyboardProc);  // Меняем текст
				//color = green;
			}
			else
			{

				if (buf[0] >= TEXT('a') && buf[0] <= TEXT('z') || buf[0] >= TEXT('A') && buf[0] <= TEXT('Z') || buf[0] >= TEXT('1') && buf[0] <= TEXT('9') || buf[0] == TEXT('.') || buf[0] == TEXT(','))
				{
					uMistakes++;
					swprintf_s(szMistakes, TEXT("%d"), uMistakes);
					SetDlgItemText(hDialog, IDC_STATIC_MISTAKES, szMistakes);
					//color = red;

					lstrcat(buf, TEXT(" Ошибка. Клавиша нажата неверно"));
					MessageBox(hWnd, buf, TEXT("Sting"), MB_ICONERROR | MB_OK);
				}
				else if (buf[0] == TEXT(' '))
				{
					MessageBox(hWnd, TEXT("Space Ошибка. Клавиша нажата неверно"), TEXT("Sting"), MB_ICONERROR | MB_OK);
				}
			}
		}
	}
	return CallNextHookEx(hHook, idCode, wParam, lParam);
}

void AppDlg::Cls_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	if (state == SIZE_MINIMIZED)
	{
		ShowWindow(hwnd, SW_HIDE); // Прячем окно
		Shell_NotifyIcon(NIM_ADD, pNID); // Добавляем иконку в трэй
	}
}

// обработчик пользовательского сообщения
void AppDlg::OnTrayIcon(WPARAM wp, LPARAM lp)
{
	// WPARAM - идентификатор иконки
	// LPARAM - сообщение от мыши или клавиатурное сообщение
	if (lp == WM_LBUTTONDBLCLK)
	{
		Shell_NotifyIcon(NIM_DELETE, pNID); // Удаляем иконку из трэя
		ShowWindow(hDialog, SW_NORMAL); // Восстанавливаем окно
		SetForegroundWindow(hDialog); // устанавливаем окно на передний план
	}
}

void AppDlg::OnFont()
{
	char buf[100];
	static CHOOSEFONT cf = { sizeof(CHOOSEFONT) };//модальный диалог "Шрифт" требует заполнения полей данной структуры
	cf.hwndOwner = hDialog;
	cf.lpLogFont = &lf;
	cf.Flags = CF_EFFECTS /* отображать секцию видоизменения  */
		| CF_FORCEFONTEXIST /* предупреждение о попытке выбрать несуществующий шрифт */
		| CF_INITTOLOGFONTSTRUCT /* для инициализации элементов управления диалога Шрифт используется информация  структуры LOGFONT */
		| CF_SCREENFONTS; // отображать в списке только шрифты, поддерживаемые системой
	if (ChooseFont(&cf))//отображаем диалог Шрифт
	{
		color = cf.rgbColors;//запоминаем выбранный цвет шрифта
		DeleteObject(hFont);//удаляем старый шрифт
		hFont = CreateFontIndirect(&lf);//создаём новый шрифт с характеристиками, установленными в диалоге Шрифт
		SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);//устанавливаем для текстового поля новый шрифт	
		SetFocus(hEdit);//устанавливаем фокус ввода для текстового поля
	}
}

void AppDlg::Cls_OnClose(HWND hwnd)
{
	EndDialog(hwnd, 0);
}

void AppDlg::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	if (LOWORD(wp) == ID_EXIT)
	{
		EndDialog(hDialog, 0);
	}
	else if (LOWORD(wp) == ID_LEVEL1 || LOWORD(wp) == ID_LEVEL2 || LOWORD(wp) == ID_LEVEL3 || LOWORD(wp) == ID_LEVEL4 || LOWORD(wp) == ID_LEVEL5 || LOWORD(wp) == ID_LEVEL_RANDOM)
	{
		minutes = 3;
		seconds = 0;
		/*ResumeThread(hndlTime);*/
		srand(time(NULL));
		if (LOWORD(wp) == ID_LEVEL1)
			_tcscpy_s(textEdit, szTextsArr[0]);
		else if (LOWORD(wp) == ID_LEVEL2)
			_tcscpy_s(textEdit, szTextsArr[1]);
		else if (LOWORD(wp) == ID_LEVEL3)
			_tcscpy_s(textEdit, szTextsArr[2]);
		else if (LOWORD(wp) == ID_LEVEL4)
			_tcscpy_s(textEdit, szTextsArr[3]);
		else if (LOWORD(wp) == ID_LEVEL5)
			_tcscpy_s(textEdit, szTextsArr[4]);
		else if (LOWORD(wp) == ID_LEVEL_RANDOM)
			_tcscpy_s(textEdit, szTextsArr[(rand() % 5)]);



		_tcscpy_s(currentText, textEdit);
		uMistakes = 0;
		swprintf_s(textMistakes, TEXT("%d"), uMistakes);
		SetWindowText(hMistakes, textMistakes);
		uProgressbarPosition = 0;
		SendMessage(hProgress, PBM_SETPOS, uProgressbarPosition, 0);
		SetWindowText(hEdit, textEdit);
	}

	else if (LOWORD(wp) == ID_FONT)
	{
		ptr->OnFont();
	}
	
}