#include <windows.h>
#include <commctrl.h>
#include "resource.h"

HWND hEdit;
HWND hStatusBar;
HINSTANCE hInst;

// Funkcja zwrotna dla okna dialogowego "O Autorze"
INT_PTR CALLBACK AboutDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hwndDlg, (INT_PTR)TRUE);
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// Funkcja obs³ugi okna g³ównego
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: {
        // Tworzenie pola edycyjnego
        hEdit = CreateWindowEx(0, TEXT("EDIT"), NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
            0, 0, CW_USEDEFAULT, CW_USEDEFAULT,
            hwnd, (HMENU)1, hInst, NULL);

        // Tworzenie paska stanu
        INITCOMMONCONTROLSEX icc;
        icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
        icc.dwICC = ICC_BAR_CLASSES;
        InitCommonControlsEx(&icc);

        hStatusBar = CreateWindowEx(0, STATUSCLASSNAME, NULL,
            WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
            0, 0, 0, 0,
            hwnd, (HMENU)ID_STATUSBAR, hInst, NULL);

        SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)TEXT("Aplikacja uruchomiona"));
    }
                  break;

    case WM_SIZE: {
        RECT rcClient;
        GetClientRect(hwnd, &rcClient);

        // Dopasowanie pola edycyjnego do rozmiaru okna
        SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom - 20, SWP_NOZORDER);

        // Dopasowanie paska stanu
        SendMessage(hStatusBar, WM_SIZE, 0, 0);
    }
                break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_PLIK_OTWORZ:
            MessageBox(hwnd, TEXT("Otwórz wybrano"), TEXT("Informacja"), MB_OK);
            break;

        case ID_PLIK_ZAPISZ:
            MessageBox(hwnd, TEXT("Zapisz wybrano"), TEXT("Informacja"), MB_OK);
            SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)TEXT("Zapisano zawartoœæ"));
            break;

        case ID_PLIK_ZAKONCZ:
            DestroyWindow(hwnd);
            break;

        case ID_EDYCJA_ZAZNACZWSZYSTKO:
            SendMessage(hEdit, EM_SETSEL, 0, -1);
            break;

        case ID_EDYCJA_WYCZYSC:
            SetWindowText(hEdit, TEXT(""));
            break;

        case ID_EDYCJA_ZMIENCZCIONKE:
            MessageBox(hwnd, TEXT("Zmiana czcionki niezaimplementowana"), TEXT("Informacja"), MB_OK);
            break;

        case ID_POMOC_OAUTORZE:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, AboutDlgProc);
            break;
        }
        break;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

// Funkcja WinMain
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;

    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WndProc,
        0, 0, hInstance, NULL, LoadCursor(NULL, IDC_ARROW),
        (HBRUSH)(COLOR_WINDOW + 1), NULL, TEXT("Notatnik"), NULL };

    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(0, TEXT("Notatnik"), TEXT("Notatnik"),
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU2)), hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!TranslateAccelerator(hwnd, hAccel, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}
