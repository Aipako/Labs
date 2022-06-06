// ApiService.cpp : Определяет точку входа для приложения.
//
#include <windows.h>
#include "pch.h"
#include "framework.h"
#include "ApiService.h"

// ApiSyncingService.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include <cstdio>
#include <stdio.h>
#include <windows.h>
#include "SCM_Installer.h"
#include "WIN_Service_Support.h"
#include "WIN_Service_Core.h"

// Имя службы
#define SERVICE_NAME             L"CentrobankAPI"

// Описание службы
#define SERVICE_DISPLAY_NAME     L"CentrobankAPI Service"

// Тип старта(ручной)
#define SERVICE_START_TYPE       SERVICE_DEMAND_START

// Список зависимостей
#define SERVICE_DEPENDENCIES     L""

int wmain(int argc, wchar_t* argv[])
{
    if ((argc > 1) && (*argv[1] == L'-' ))
    {
        if (_wcsicmp(L"install", argv[1] + 1) == 0)
        {
            // Установка службы
            InstallService(SERVICE_NAME, SERVICE_DISPLAY_NAME, SERVICE_START_TYPE, SERVICE_DEPENDENCIES);
        }
        else if (_wcsicmp(L"remove", argv[1] + 1) == 0)
        {
            // удаление службы
            UninstallService(SERVICE_NAME);
        }
    }
    else
    {
        wprintf(L"Parameters:\n");
        wprintf(L" -install  to install the service.\n");
        wprintf(L" -remove   to remove the service.\n");

        WIN_Service_Core service(SERVICE_NAME);
        if (!WIN_Service_Core::Run(service))
        {
            wprintf(L"Service failed to run with error 0x%08lx\n", GetLastError());
        }
    }

    return 0;
}