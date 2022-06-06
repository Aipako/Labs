
#include "pch.h"
#include "SCM_Installer.h"
#include <stdio.h>
#include <windows.h>

void InstallService(PWSTR scmServiceName, PWSTR scmDisplayName, DWORD dwStartType, PWSTR scmDependencies)
{
    wchar_t szPath[MAX_PATH];
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService = NULL;

    //���� ���� � ������
    if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath)) == 0)
    {
        wprintf(L"GetModuleFileName failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    // ��������� ���� ������ ���������� �����
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE);
    if (schSCManager == NULL)
    {
        wprintf(L"OpenSCManager failed with error 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    // ��������� ������ � SCM. ����� � ����� ��������� ����� StartServiceCtrlDispatcher
    schService = CreateService(
        schSCManager,                   // ���� ������ ���������� �����
        scmServiceName,                 // ��� ������
        scmDisplayName,                 // �������� ������
        SERVICE_QUERY_STATUS,           // ������� ���� ������
        SERVICE_WIN32_OWN_PROCESS,      // ��� ������
        dwStartType,                    // ��� ������� ������(���������/�������/���������, �� ��������� ��� ���������)
        SERVICE_ERROR_NORMAL,           // ��������� ��� ������ ������ (�������������/������ � ���, �� ��������� �� ��� ���:) )
        szPath,                         // ���� � ����� ������
        NULL,                           // � ��� ��� ���������� ����� ��� ������, ��������� NULL
        NULL,                           // ��� ��� ������� ������ ������, ����� �� ��� ���
        scmDependencies,                // ����������� �� ������ ����� (� � ���� ������ �� �����, � �� ����� ��� ������ ��� http ��������:) )
        L"NT AUTHORITY\\NetworkService", // ������� ������ ������. � ������ ��� ������ ���� NetworkService, ������� ����� �����
        NULL                            // ������ ������� ������ ������. � ��� ���, � ��� NetworkService
    );
    if (schService == NULL)
    {
        wprintf(L"CreateService failed with error 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    wprintf(L"%s is installed.\n", scmServiceName);

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (schSCManager)
    {
        CloseServiceHandle(schSCManager);
        schSCManager = NULL;
    }
    if (schService)
    {
        CloseServiceHandle(schService);
        schService = NULL;
    }
}





void UninstallService(PWSTR scmServiceName)
{
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService = NULL;
    SERVICE_STATUS ssSvcStatus = {};

    // ����� ��������� ��������� �����
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (schSCManager == NULL)
    {
        wprintf(L"OpenSCManager failed with error 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    // �������� ������ � ������ � ������� �� ���������
    schService = OpenService(schSCManager, scmServiceName, SERVICE_STOP |
        SERVICE_QUERY_STATUS | DELETE);
    if (schService == NULL)
    {
        wprintf(L"OpenService failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    // ������������� ������ � ��� ���������.
    if (ControlService(schService, SERVICE_CONTROL_STOP, &ssSvcStatus))
    {
        wprintf(L"Stopping %s.", scmServiceName);
        Sleep(1000);

        while (QueryServiceStatus(schService, &ssSvcStatus))
        {
            if (ssSvcStatus.dwCurrentState == SERVICE_STOP_PENDING)
            {
                //��� ��� ��� ��� ��������
                wprintf(L".");
                Sleep(1000);
            }
            else break;
        }

        if (ssSvcStatus.dwCurrentState == SERVICE_STOPPED)
        {
            wprintf(L"\n%s is stopped.\n", scmServiceName);
        }
        else
        {
            wprintf(L"\n%s failed to stop.\n", scmServiceName);
        }
    }

    // ������ �����������, ����� ������� � ������ DeleteService()
    if (!DeleteService(schService))
    {
        wprintf(L"DeleteService failed with error 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    wprintf(L"%s is removed.\n", scmServiceName);

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (schSCManager)
    {
        CloseServiceHandle(schSCManager);
        schSCManager = NULL;
    }
    if (schService)
    {
        CloseServiceHandle(schService);
        schService = NULL;
    }
}