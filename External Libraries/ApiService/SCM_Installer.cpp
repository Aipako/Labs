
#include "pch.h"
#include "SCM_Installer.h"
#include <stdio.h>
#include <windows.h>

void InstallService(PWSTR scmServiceName, PWSTR scmDisplayName, DWORD dwStartType, PWSTR scmDependencies)
{
    wchar_t szPath[MAX_PATH];
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService = NULL;

    //Ищем путь к службе
    if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath)) == 0)
    {
        wprintf(L"GetModuleFileName failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    // Получение базы данных диспетчера служб
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE);
    if (schSCManager == NULL)
    {
        wprintf(L"OpenSCManager failed with error 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    // Добавляем службу в SCM. После её можно запустить через StartServiceCtrlDispatcher
    schService = CreateService(
        schSCManager,                   // База данных диспетчера служб
        scmServiceName,                 // Имя службы
        scmDisplayName,                 // Описание службы
        SERVICE_QUERY_STATUS,           // Уровень прав службы
        SERVICE_WIN32_OWN_PROCESS,      // Тип службы
        dwStartType,                    // Тип запуска службы(Автостарт/вручную/выключена, всё остальное для драйверов)
        SERVICE_ERROR_NORMAL,           // Поведение при ошибке службы (Игнорирование/запись в лог, всё остальное не для нас:) )
        szPath,                         // Путь к файлу службы
        NULL,                           // У нас нет нескольких служб для группы, оставляем NULL
        NULL,                           // Тэг для очереди внутри группы, опять не про нас
        scmDependencies,                // Зависимости от других служб (а я сюда ничего не впишу, и всё упадёт без службы для http запросов:) )
        L"NT AUTHORITY\\NetworkService", // Учётная запись службы. В теории это должна быть NetworkService, поэтому пусть будет
        NULL                            // Пароль учётной записи службы. А его нет, у нас NetworkService
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

    // Снова открываем диспетчер служб
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (schSCManager == NULL)
    {
        wprintf(L"OpenSCManager failed with error 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    // Получаем доступ к службе с правами на остановку
    schService = OpenService(schSCManager, scmServiceName, SERVICE_STOP |
        SERVICE_QUERY_STATUS | DELETE);
    if (schService == NULL)
    {
        wprintf(L"OpenService failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    // Останавливаем службу и ждём оставноки.
    if (ControlService(schService, SERVICE_CONTROL_STOP, &ssSvcStatus))
    {
        wprintf(L"Stopping %s.", scmServiceName);
        Sleep(1000);

        while (QueryServiceStatus(schService, &ssSvcStatus))
        {
            if (ssSvcStatus.dwCurrentState == SERVICE_STOP_PENDING)
            {
                //Так вот как это делается
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

    // Служба остановлена, можем удалить её вызвав DeleteService()
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