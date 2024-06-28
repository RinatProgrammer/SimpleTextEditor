#include <windows.h>
#include <commdlg.h>

#define IDC_MAIN_EDIT 101

const char g_szClassName[] = "myWindowClass";

HINSTANCE g_hInst = NULL;
HWND g_hEdit;

void DoFileOpen(HWND hwnd)
{
    OPENFILENAME ofn = {0};
    char szFileName[MAX_PATH] = "";

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "txt";

    if (GetOpenFileName(&ofn))
    {
        HANDLE hFile = CreateFile(szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE)
        {
            DWORD dwFileSize = GetFileSize(hFile, NULL);
            if (dwFileSize != INVALID_FILE_SIZE)
            {
                char *pszFileText = (char *)GlobalAlloc(GPTR, dwFileSize + 1);
                DWORD dwRead;
                if (ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL))
                {
                    pszFileText[dwFileSize] = '\0';
                    SetWindowText(g_hEdit, pszFileText);
                }
                GlobalFree(pszFileText);
            }
            CloseHandle(hFile);
        }
    }
}

void DoFileSave(HWND hwnd)
{
    OPENFILENAME ofn = {0};
    char szFileName[MAX_PATH] = "";

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "txt";

    if (GetSaveFileName(&ofn))
    {
        HANDLE hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE)
        {
            DWORD dwTextLength = GetWindowTextLength(g_hEdit);
            char *pszText = (char *)GlobalAlloc(GPTR, dwTextLength + 1);
            if (pszText)
            {
                GetWindowText(g_hEdit, pszText, dwTextLength + 1);
                DWORD dwWritten;
                WriteFile(hFile, pszText, dwTextLength, &dwWritten, NULL);
                GlobalFree(pszText);
            }
            CloseHandle(hFile);
        }
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        g_hEdit = CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                                 0, 0, 100, 100, hwnd, (HMENU)IDC_MAIN_EDIT, g_hInst, NULL);
        break;
    case WM_SIZE:
    {
        RECT rcClient;
        GetClientRect(hwnd, &rcClient);
        SetWindowPos(g_hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
    }
    break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_FILE_OPEN:
            DoFileOpen(hwnd);
            break;
        case ID_FILE_SAVE:
            DoFileSave(hwnd);
            break;
        case ID_FILE_EXIT:
            PostMessage(hwnd, WM_CLOSE, 0, 0);
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
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc = {0};
    HWND hwnd;
    MSG Msg;

    g_hInst = hInstance;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = Wnd