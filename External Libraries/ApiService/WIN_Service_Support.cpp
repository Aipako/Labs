#include "pch.h"
#include "WIN_Service_Support.h"
#include <assert.h>
#include <strsafe.h>

WIN_Service_Support* WIN_Service_Support::s_service = NULL;

BOOL WIN_Service_Support::Run(WIN_Service_Support& service)
{

    s_service = &service;

    // ���������� ���� ������ ���� NULL,NULL - ���� ����� �������
    SERVICE_TABLE_ENTRY serviceTable[] =
    {
        { service.m_name, ServiceMain },
        { NULL, NULL }
    };

    // SCM c����� ������� ������, ����� ���������� �������� ������ ����� ��������� �������. StartServiceCtrlDispatcher ����� ����������� ������ SCM, ����� ������ ���� �������
    return StartServiceCtrlDispatcher(serviceTable);
}


// ���� �������, ��������� ����� ����������� ���� ������.
void WINAPI WIN_Service_Support::ServiceMain(DWORD dwArgc, PWSTR* scmArgv)
{
    assert(s_service != NULL);

    // ������ ������� �������, ��������� �� ����������
    s_service->m_statusHandle = RegisterServiceCtrlHandler(s_service->m_name, ServiceCtrlHandler);
    if (s_service->m_statusHandle == NULL)
    {
        throw GetLastError();
    }

    // ��������� ������
    s_service->Start(dwArgc, scmArgv);

    s_service->Loop();
}

// ��������� ���������� �� ���������� �����
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
    // �������� �� ������������ ������.
    m_name = (scmServiceName == NULL) ? L"" : scmServiceName;

    m_statusHandle = NULL;

    // ��� ������.
    m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;

    // ������ ������ - ������.
    m_status.dwCurrentState = SERVICE_START_PENDING;

    // ��������� ���������� ��������(PAUSE_CONTINUE �� ����������)
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
    // � ���, � �����?
}


// ���������� �������
void WIN_Service_Support::Start(DWORD dwArgc, PWSTR* scmArgv)
{
    try
    {
        // ������ ������ - ������
        SetServiceStatus(SERVICE_START_PENDING);

        
        OnStart(dwArgc, scmArgv);

        // ������ ������ - ���������
        SetServiceStatus(SERVICE_RUNNING);
    }
    catch (DWORD dwError)
    {
        // ������ �� ������, ����� � ���
        WriteErrorLogEntry(L"Service Start", dwError);

        // ������ ������ - ����������� � �������
        SetServiceStatus(SERVICE_STOPPED, dwError);
    }
    catch (...)
    {
        // ������ �� ������, �� �������, ���� ������ ���������� ���
        WriteEventLogEntry(L"Service failed to start.", EVENTLOG_ERROR_TYPE);

        // ������ ������ - �����������
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
        // ������ ������ - ���������
        SetServiceStatus(SERVICE_STOP_PENDING);

        OnStop();

        // ������ ������ - �����������
        SetServiceStatus(SERVICE_STOPPED);
    }
    catch (DWORD dwError)
    {
        // ��������� �� �������, ����� � ���
        WriteErrorLogEntry(L"Service Stop", dwError);

        // ���������� ����������� ������
        SetServiceStatus(dwOriginalState);
    }
    catch (...)
    {
        // ������ �� ������, ����� � ���, ������ ���������� ���� ���
        WriteEventLogEntry(L"Service failed to stop.", EVENTLOG_ERROR_TYPE);

        // ���������� ����������� ������
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

//��������� ������� ������
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
    // �������� � ������� SCM
    ::SetServiceStatus(m_statusHandle, &m_status);
}

// ����� ������ � ���
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