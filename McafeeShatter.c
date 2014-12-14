#include <windows.h>

// improved mechanism

#ifndef WH_KEYBOARD_LL
#define WH_KEYBOARD_LL 13
#endif
#ifndef WH_MOUSE_LL
#define WH_MOUSE_LL 14
#endif

HHOOK g_hhk;
HWND g_DlgHwnd;

BOOL CALLBACK DlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    // handles pretty dialogue box
    switch (Message) {
        case WM_INITDIALOG:
            g_DlgHwnd = hwnd;
            return TRUE;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDQUIT:
                    EndDialog(hwnd, 0);
                    break;
            }
            break;
        case WM_CLOSE:
            EndDialog(hwnd, -1);
            break;
        default:
            return FALSE;
    }
    return TRUE;
}

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam) {
    EnableWindow(hwnd, 1);
    return TRUE;
}


// this works only because systemwide LL hooks are different.. somehow :/
// it will NOT work on non LL hooks
// you really do need an external dll for those

LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    static POINT lstPt; // where the last click took place


    // only do this if an action
    if (nCode == HC_ACTION) {
        if (wParam == WM_RBUTTONDOWN) {
            // control key pressed do our stuff
            POINT nowPt;
            HWND hwnd;

            GetCursorPos(&nowPt);
            if (lstPt.x == nowPt.x && lstPt.y == nowPt.y) {
                // doubleclick
                hwnd = WindowFromPoint(nowPt);
                SetWindowText(hwnd, "Read only? ARSE");
                if (hwnd != NULL) {
                    // i did try ChildWindowFromPoint but erh
                    // it sucked so i did this
                    EnumChildWindows(hwnd, EnumChildProc, 0);
                }
            }
            memcpy(&lstPt, &nowPt, sizeof (POINT));
        } else if (wParam == WM_MOUSEMOVE) {
            POINT pt;
            GetCursorPos(&pt);
            SetDlgItemInt(g_DlgHwnd, IDC_EDIT1, pt.x, FALSE);
            SetDlgItemInt(g_DlgHwnd, IDC_EDIT2, pt.y, FALSE);
        }
    }


    return CallNextHookEx(g_hhk, nCode, wParam, lParam);
}

void MoanIfNotAdmin(void) {
    // does what it says on the tin
    HANDLE hFile;

    SetLastError(0);

    hFile = CreateFile("C:/checkabc#", // rude if the file exists but not
            GENERIC_WRITE, // dangerous :)
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_FLAG_DELETE_ON_CLOSE,
            NULL);

    if (GetLastError() == ERROR_ACCESS_DENIED) {
        // we're not admin
        if (MessageBox(NULL,
                "You do not have sufficient privileges to alter mcafee's settings!\nContinue anyways?",
                "Warning",
                MB_ICONWARNING | MB_YESNO) == IDNO) {
            ExitProcess(-1);
        }
    }
    CloseHandle(hFile);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
        LPSTR lpCmdLine, int nShowCmd) {

    MoanIfNotAdmin(); // if you don't care: comment me.

    // again, you need an external dll for other non LL hooks!!
    g_hhk = SetWindowsHookEx(WH_MOUSE_LL,
            HookProc,
            GetModuleHandle(NULL),
            0); //global

    DialogBox(GetModuleHandle(NULL),
            MAKEINTRESOURCE(IDD_DIALOG1),
            NULL, DlgProc);

    UnhookWindowsHookEx(g_hhk);

    return 0;
}
