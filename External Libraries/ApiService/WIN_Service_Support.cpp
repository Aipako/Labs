#include "pch.h"
#include "WIN_Service_Support.h"
#include <assert.h>
#include <strsafe.h>

WIN_Service_Support* WIN_Service_Support::s_service = NULL;

BOOL WIN_Service_Support::Run(WIN_Service_Support& service)
{

    s_service = &service;

    // Замыкающая пара должна быть NULL,NULL - знак конца таблицы
    SERVICE_TABLE_ENTRY serviceTable[] =
    {
        { service.m_name, ServiceMain },
        { NULL, NULL }
    };

    // SCM cоздаёт процесс службы, метод возвращает значение ТОЛЬКО после остановки функции. StartServiceCtrlDispatcher может применяться ТОЛЬКО SCM, иначе ошибка прав доступа
    return StartServiceCtrlDispatcher(serviceTable);
}


// Маин функция, запускает метод внутреннего кода службы.
void WINAPI WIN_Service_Support::ServiceMain(DWORD dwArgc, PWSTR* scmArgv)
{
    assert(s_service != NULL);

    // Создаём хендлер статуса, проверяем на успешность
    s_service->m_statusHandle = RegisterServiceCtrlHandler(s_service->m_name, ServiceCtrlHandler);
    if (s_service->m_statusHandle == NULL)
    {
        throw GetLastError();
    }

    // Запускаем службу
    s_service->Start(dwArgc, scmArgv);

    s_service->Loop();
}

// Обработка исключений от диспетчера служб
void WINAPI WIN_Service_Support::ServiceCtrlHandler(DWORD dwCtrl)
{
    switch (dwCtrl)
    {
    case SERVICE_CONTROL_STOP: 
        s_service->Stop(); 
        break;
    case SERVICE_CONTROL_SHUTDOWN: 
        s_service->Shutdown(); 
        break;
    case SERVICE_CONTROL_INTERROGATE: 
        break;
    default: 
        break;
    }
}

WIN_Service_Support::WIN_Service_Support(PWSTR scmServiceName, BOOL fCanStop, BOOL fCanShutdown, BOOL fCanPauseContinue)
{
    // Проверка на корректность строки.
    m_name = (scmServiceName == NULL) ? L"" : scmServiceName;

    m_statusHandle = NULL;

    // Тип службы.
    m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;

    // Статус службы - запуск.
    m_status.dwCurrentState = SERVICE_START_PENDING;

    // Настройка управления фукнцией(PAUSE_CONTINUE не реализован)
    DWORD dwControlsAccepted = 0;
    if (fCanStop)
        dwControlsAccepted |= SERVICE_ACCEPT_STOP;
    if (fCanShutdown)
        dwControlsAccepted |= SERVICE_ACCEPT_SHUTDOWN;
    if (fCanPauseContinue)
        dwControlsAccepted |= SERVICE_ACCEPT_PAUSE_CONTINUE;
    m_status.dwControlsAccepted = dwControlsAccepted;

    m_status.dwWin32ExitCode = NO_ERROR;
    m_status.dwServiceSpecificExitCode = 0;
    m_status.dwCheckPoint = 0;
    m_status.dwWaitHint = 0;
}

WIN_Service_Support::~WIN_Service_Support(void)
{
    // А что, а зачем?
}


// Управление службой
void WIN_Service_Support::Start(DWORD dwArgc, PWSTR* scmArgv)
{
    try
    {
        // Статус службы - запуск
        SetServiceStatus(SERVICE_START_PENDING);

        
        OnStart(dwArgc, scmArgv);

        // Статус службы - запущенна
        SetServiceStatus(SERVICE_RUNNING);
    }
    catch (DWORD dwError)
    {
        // Запуск не удался, пишем в лог
        WriteErrorLogEntry(L"Service Start", dwError);

        // Статус службы - остановлена с ошибкой
        SetServiceStatus(SERVICE_STOPPED, dwError);
    }
    catch (...)
    {
        // Запуск не удался, всё пропало, даже ошибки нормальной нет
        WriteEventLogEntry(L"Service failed to start.", EVENTLOG_ERROR_TYPE);

        // Статус службы - остановлена
        SetServiceStatus(SERVICE_STOPPED);
    }
}

void WIN_Service_Support::OnStart(DWORD dwArgc, PWSTR* scmArgv)
{

}

void WIN_Service_Support::Loop()
{

}

void WIN_Service_Support::Stop()
{
    DWORD dwOriginalState = m_status.dwCurrentState;
    try
    {
        // Статус службы - остановка
        SetServiceStatus(SERVICE_STOP_PENDING);

        OnStop();

        // Статус службы - остановлена
        SetServiceStatus(SERVICE_STOPPED);
    }
    catch (DWORD dwError)
    {
        // Остановка не удалась, пишем в лог
        WriteErrorLogEntry(L"Service Stop", dwError);

        // Возвращаем изначальный статус
        SetServiceStatus(dwOriginalState);
    }
    catch (...)
    {
        // Запуск не удался, пишем в лог, ошибки нормальной тоже нет
        WriteEventLogEntry(L"Service failed to stop.", EVENTLOG_ERROR_TYPE);

        // Возвращаем изначальный статус
        SetServiceStatus(dwOriginalState);
    }
}

void WIN_Service_Support::OnStop()
{

}


void WIN_Service_Support::Shutdown()
{
    Stop();
}

//Установка статуса службы
void WIN_Service_Support::SetServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;


    m_status.dwCurrentState = dwCurrentState;
    m_status.dwWin32ExitCode = dwWin32ExitCode;
    m_status.dwWaitHint = dwWaitHint;

    m_status.dwCheckPoint =
        ((dwCurrentState == SERVICE_RUNNING) ||
            (dwCurrentState == SERVICE_STOPPED)) ?
        0 : dwCheckPoint++;
    // Сообщаем о статусе SCM
    ::SetServiceStatus(m_statusHandle, &m_status);
}

// Пишем ошибки в лог
void WIN_Service_Support::WriteErrorLogEntry(PWSTR scmFunction, DWORD dwError)
{
    wchar_t szMessage[260];
    StringCchPrintf(szMessage, ARRAYSIZE(szMessage), L"%s failed with error 0x%08lx", scmFunction, dwError);
    WriteEventLogEntry(szMessage, EVENTLOG_ERROR_TYPE);
}

void WIN_Service_Support::WriteEventLogEntry(PWSTR scmMessage, WORD wType)
{
    HANDLE hEventSource = NULL;
    LPCWSTR lscmStrings[2] = { NULL, NULL };

    hEventSource = RegisterEventSource(NULL, m_name);
    if (hEventSource)
    {
        lscmStrings[0] = m_name;
        lscmStrings[1] = scmMessage;

        ReportEvent(hEventSource,  
            wType,                 
            0,                     
            0,                     
            NULL,                  
            2,                     
            0,                     
            lscmStrings,           
            NULL                   
        );

        DeregisterEventSource(hEventSource);
    }
}