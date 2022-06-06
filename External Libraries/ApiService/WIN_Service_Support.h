#pragma once
class WIN_Service_Support
{
public:

    // ��� ���������� ����� ������� �� ������
    // ����� ����������� OnStart()
    // �� ���� ������������ �� ��������� ������
    static BOOL Run(WIN_Service_Support& service);

    // ����������� ������ ������, ������������ ��� ����������� ������ ������ � ������ Windows API
    // ����� ������� ����������� ��������� ������, ��������� ��� ����������, � ��������� ��� ������������
    WIN_Service_Support(PWSTR scmServiceName, BOOL fCanStop = TRUE, BOOL fCanShutdown = TRUE, BOOL fCanPauseContinue = FALSE);

    // ���������� ������ ����������� ������ ������ 
    virtual ~WIN_Service_Support(void);

    // ��������� ������
    void Stop();

protected:

    // ����������� �� ������ ������� � �������������� ������ � ������: �������� ������� MSMQ ��� �������� json ������ ����������� ����������.
    virtual void OnStart(DWORD dwArgc, PWSTR* scmArgv);
    
    // ����������� ��� ��������� �������, ������� ������, ��������� ������� MSMQ.
    virtual void OnStop();

    // �������� ������� - ��� ����� ����������� ����
    virtual void Loop();

    // ������������� ������ ������ � �������� ��� ���������� �����
    void SetServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode = NO_ERROR, DWORD dwWaitHint = 0);

    // ���������� � ��� ������.
    void WriteErrorLogEntry(PWSTR scmFunction, DWORD dwError = GetLastError());

    void WriteEventLogEntry(PWSTR scmMessage, WORD wType);

private:

    // ������� ����� ������
    static void WINAPI ServiceMain(DWORD dwArgc, LPWSTR* lscmArgv);

    // ���������� ���������� WINAPI, ������������ ��� ���������� ������� �� ������� SCM
    static void WINAPI ServiceCtrlHandler(DWORD dwCtrl);

    void Start(DWORD dwArgc, PWSTR* scmArgv);
    void Shutdown();

    // ������� ������, ��� ����� ����� ������ � �����.
    static WIN_Service_Support* s_service;

    // ��� ������
    PWSTR m_name;

    // ������ ������
    SERVICE_STATUS m_status;

    // ����� �������
    SERVICE_STATUS_HANDLE m_statusHandle;
};

