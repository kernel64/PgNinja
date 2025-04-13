#include <windows.h>
#include <shellapi.h>
#include <winsvc.h>
#include <tchar.h>
#include <string>
#include <vector>
#include "resource.h"

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_ABOUT 1005
#define ID_TRAY_START 1001
#define ID_TRAY_STOP  1002
#define ID_TRAY_RESTART 1003
#define ID_TRAY_EXIT  1004

NOTIFYICONDATA nid = {};
HMENU hMenu;

bool IsRunningAsAdmin() {
    BOOL isAdmin = FALSE;
    PSID adminGroup = NULL;
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;

    if (AllocateAndInitializeSid(&NtAuthority, 2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &adminGroup)) {
        CheckTokenMembership(NULL, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }

    return isAdmin;
}

void UpdateIcon(HINSTANCE hInstance, int iconId) {
    nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(iconId));
    Shell_NotifyIcon(NIM_MODIFY, &nid);
}

std::string FindPostgreSQLService() {
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    if (!hSCManager) return "";

    DWORD bytesNeeded = 0, servicesReturned = 0, resumeHandle = 0;
    EnumServicesStatusExA(hSCManager, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL,
        NULL, 0, &bytesNeeded, &servicesReturned, &resumeHandle, NULL);

    std::vector<BYTE> buffer(bytesNeeded);
    LPENUM_SERVICE_STATUS_PROCESSA services = reinterpret_cast<LPENUM_SERVICE_STATUS_PROCESSA>(buffer.data());

    if (!EnumServicesStatusExA(hSCManager, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL,
        buffer.data(), bytesNeeded, &bytesNeeded, &servicesReturned, &resumeHandle, NULL)) {
        CloseServiceHandle(hSCManager);
        return "";
    }

    std::string serviceName = "";
    for (DWORD i = 0; i < servicesReturned; ++i) {
        std::string currentName = services[i].lpServiceName;
        if (currentName.find("postgresql") != std::string::npos) {
            serviceName = currentName;
            break;
        }
    }

    CloseServiceHandle(hSCManager);
    return serviceName;
}

void ControlService(const char* command, const std::string& serviceName) {
    std::string cmdLine = "sc ";
    cmdLine += command;
    cmdLine += " ";
    cmdLine += serviceName;

    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    char cmdBuffer[256];
    strcpy_s(cmdBuffer, cmdLine.c_str());

    if (CreateProcessA(
        NULL,
        cmdBuffer,
        NULL, NULL,
        FALSE,
        CREATE_NO_WINDOW,
        NULL,
        NULL,
        &si, &pi
    )) {
        // Attendre la fin du processus lancé
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else {
        MessageBoxA(NULL, "Failed to control PostgreSQL service.", "PgNinja", MB_ICONERROR);
    }
}

bool IsServiceRunning() {
    std::string serviceName = FindPostgreSQLService();
    if (serviceName.empty()) return false;

    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (!hSCManager) return false;

    SC_HANDLE hService = OpenServiceA(hSCManager, serviceName.c_str(), SERVICE_QUERY_STATUS);
    if (!hService) {
        CloseServiceHandle(hSCManager);
        return false;
    }

    SERVICE_STATUS_PROCESS ssStatus = {};
    DWORD bytesNeeded;
    bool isRunning = false;

    if (QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, reinterpret_cast<LPBYTE>(&ssStatus),
        sizeof(SERVICE_STATUS_PROCESS), &bytesNeeded)) {
        isRunning = (ssStatus.dwCurrentState == SERVICE_RUNNING);
    }

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
    return isRunning;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    static std::string serviceName = FindPostgreSQLService();
    if (serviceName.empty()) {
        MessageBoxA(NULL, "PostgreSQL server not found on this machine.", "PgNinja", MB_ICONERROR | MB_OK);
        return 0;
    }

    switch (msg) {
    case WM_TRAYICON:
        if (lParam == WM_RBUTTONUP || lParam == WM_LBUTTONUP) {
            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(hwnd);
            TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL);
        }
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_TRAY_START:
            ControlService("start", serviceName);
            UpdateIcon(GetModuleHandle(NULL), IDI_ICON_GREEN);
            break;
        case ID_TRAY_STOP:
            ControlService("stop", serviceName);
            UpdateIcon(GetModuleHandle(NULL), IDI_ICON_RED);
            break;
        case ID_TRAY_RESTART:
            ControlService("stop", serviceName);
            UpdateIcon(GetModuleHandle(NULL), IDI_ICON_RED);
            Sleep(2000);
            UpdateIcon(GetModuleHandle(NULL), IDI_ICON_YELLOW);
            ControlService("start", serviceName);
            UpdateIcon(GetModuleHandle(NULL), IDI_ICON_GREEN);
            break;
		case ID_TRAY_ABOUT:
            MessageBox(
                NULL,
                "PgNinja — PostgreSQL Tray Controller\n\n"
                "Version 1.0.0\n"
                "Developed by Mohamed Aymen\n"
                "MIT Licensed\n\n"
                "https://github.com/kernel64/PgNinja",
                "About PgNinja",
                MB_ICONINFORMATION | MB_OK
            );
            break;
        case ID_TRAY_EXIT:
            Shell_NotifyIcon(NIM_DELETE, &nid);
            PostQuitMessage(0);
            break;
        }
        break;
    case WM_DESTROY:
        Shell_NotifyIcon(NIM_DELETE, &nid);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {

    if (!IsRunningAsAdmin()) {
        
        char path[MAX_PATH];
        GetModuleFileNameA(NULL, path, MAX_PATH);

        SHELLEXECUTEINFOA sei = { sizeof(sei) };
        sei.lpVerb = "runas";
        sei.lpFile = path;
        sei.hwnd = NULL;
        sei.nShow = SW_NORMAL;

        if (!ShellExecuteExA(&sei)) {
            MessageBoxA(NULL, "Elevation failed. PgNinja requires administrator rights.", "PgNinja", MB_ICONERROR);
        }

        return 0; 
    }


    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "PgNinjaClass";
    RegisterClass(&wc);

    HWND hwnd = CreateWindow("PgNinjaClass", "PgNinja", WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);

    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    strcpy_s(nid.szTip, "PgNinja - PostgreSQL Controller");

    if (IsServiceRunning()) {
        nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_GREEN));
    }
    else {
        nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_RED));
    }

    Shell_NotifyIcon(NIM_ADD, &nid);

    hMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING, ID_TRAY_ABOUT, "About!");
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenu, MF_STRING, ID_TRAY_START, "Start");
    AppendMenu(hMenu, MF_STRING, ID_TRAY_STOP, "Stop");
    AppendMenu(hMenu, MF_STRING, ID_TRAY_RESTART, "Restart");
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, "Exit");

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
