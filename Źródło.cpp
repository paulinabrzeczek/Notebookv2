#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <vector>
#include "resource.h"

#pragma comment(lib, "Comctl32.lib")

HFONT selFont = NULL;
HWND hEdit;
HWND hStatusBar;

// Funkcja do zmiany czcionki
void ChangeFont(HWND hwnd) {
    LOGFONT lf;
    CHOOSEFONT cfnt;
    ZeroMemory(&lf, sizeof(lf));
    ZeroMemory(&cfnt, sizeof(cfnt));

    // Pobranie aktualnej czcionki
    selFont = (HFONT)SendMessage(hEdit, WM_GETFONT, 0, 0);
    if (selFont == NULL)
        selFont = (HFONT)GetStockObject(SYSTEM_FONT);

    GetObject(selFont, sizeof(LOGFONT), &lf);

    cfnt.lStructSize = sizeof(cfnt);
    cfnt.hwndOwner = hwnd;
    cfnt.lpLogFont = &lf;
    cfnt.Flags = CF_EFFECTS | CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;

    // Wyœwietlenie okna dialogowego wyboru czcionki
    if (ChooseFont(&cfnt)) {
        HFONT newFont = CreateFontIndirect(&lf);
        SendMessage(hEdit, WM_SETFONT, (WPARAM)newFont, TRUE);
        selFont = newFont;
    }
}

// Funkcja odczytu pliku do pola edycji
BOOL ReadFileToEdit(HWND hEdit, LPCSTR fileName) {
    BOOL success = FALSE;
    HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        DWORD fileSize = GetFileSize(hFile, NULL);
        if (fileSize != INVALID_FILE_SIZE) {
            std::vector<CHAR> buf(fileSize + 1);
            DWORD dwRead;
            if (ReadFile(hFile, buf.data(), fileSize, &dwRead, NULL)) {
                buf[fileSize] = 0;
                SetWindowText(hEdit, buf.data());
                success = TRUE;
            }
        }
        CloseHandle(hFile);
    }
    return success;
}

// Funkcja zapisu pola edycji do pliku
BOOL WriteEditToFile(HWND hEdit, LPCSTR fileName) {
    BOOL success = FALSE;
    HANDLE hFile = CreateFile(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        DWORD textLength = GetWindowTextLength(hEdit);
        if (textLength > 0) {
            std::vector<CHAR> buf(textLength + 1);
            GetWindowText(hEdit, buf.data(), textLength + 1);
            DWORD dwWrite;
            if (WriteFile(hFile, buf.data(), textLength, &dwWrite, NULL)) {
                success = TRUE;
            }
        }
        CloseHandle(hFile);
    }
    return success;
}

// Funkcja okna dialogowego "O Autorze"
INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG:
        return TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        break;
    }
    return FALSE;
}

// Funkcja obs³ugi komunikatów
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: {
        InitCommonControls();  // Dodaj to, aby upewniæ siê, ¿e kontrolki s¹ za³adowane.
        hEdit = CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
            0, 0, 800, 500, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);
        hStatusBar = CreateWindowEx(0, STATUSCLASSNAME, NULL, WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
            0, 0, 0, 0, hwnd, (HMENU)2, GetModuleHandle(NULL), NULL);
        SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)"Gotowy.");
        break;
    }

    case WM_SIZE: {
        RECT rcClient;
        GetClientRect(hwnd, &rcClient);
        SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom - 20, SWP_NOZORDER);
        SendMessage(hStatusBar, WM_SIZE, 0, 0);
        break;
    }

    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case ID_PLIK_OTWORZ: {
            char fileName[MAX_PATH] = "";
            OPENFILENAME ofn = {};
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = fileName;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrFilter = "Text Files\0*.TXT\0All Files\0*.*\0";
            ofn.Flags = OFN_FILEMUSTEXIST;
            if (GetOpenFileName(&ofn)) {
                if (ReadFileToEdit(hEdit, fileName)) {
                    SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)"Plik otwarty pomyœlnie.");
                }
                else {
                    SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)"B³¹d podczas otwierania pliku.");
                }
            }
            break;
        }

        case ID_PLIK_ZAPISZ: {
            char fileName[MAX_PATH] = "";
            OPENFILENAME ofn = {};
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = fileName;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrFilter = "Text Files\0*.TXT\0All Files\0*.*\0";
            ofn.Flags = OFN_OVERWRITEPROMPT;
            if (GetSaveFileName(&ofn)) {
                if (WriteEditToFile(hEdit, fileName)) {
                    SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)"Plik zapisany pomyœlnie.");
                }
                else {
                    SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)"B³¹d podczas zapisywania pliku.");
                }
            }
            break;
        }

        case ID_EDYCJA_ZAZNACZWSZYSTKO:
            SendMessage(hEdit, EM_SETSEL, 0, -1);
            break;

        case ID_EDYCJA_WYCZYSC:
            SetWindowText(hEdit, "");
            break;

        case ID_EDYCJA_ZMIENCZCIONKE:
            ChangeFont(hwnd);
            break;

        case ID_POMOC_OAUTORZE:
            DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), hwnd, AboutDlgProc);
            break;

        case ID_PLIK_ZAKONCZ:
            PostQuitMessage(0);
            break;
        }
        break;
    }

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

// Funkcja g³ówna
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "Notatnik";

    RegisterClass(&wc);

    HWND hwnd = CreateWindow("Notatnik", "Notatnik", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU2)), hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;

    HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!TranslateAccelerator(hwnd, hAccel, &msg)) {  // Dodajemy tê linijkê
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}
