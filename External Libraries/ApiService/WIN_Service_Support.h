#pragma once
class WIN_Service_Support
{
public:

    // Даёт диспетчеру служб команду на запуск
    // После выполняется OnStart()
    // НЕ НАДО ИСПОЛЬЗОВАТЬ ДО ОСТАНОВКИ СЛУЖБЫ
    static BOOL Run(WIN_Service_Support& service);

    // Конструктор класса службы, предназначен для обеспечения работы службы в рамках Windows API
    // Можно указать возможность остановки службы, обработки при выключении, и обработки при приостановке
    WIN_Service_Support(PWSTR scmServiceName, BOOL fCanStop = TRUE, BOOL fCanShutdown = TRUE, BOOL fCanPauseContinue = FALSE);

    // Деструктор класса обеспечения работы службы 
    virtual ~WIN_Service_Support(void);

    // Остановка службы
    void Stop();

protected:

    // Выполняется на старте функции и подготавливает службу к работе: Создаётся очередь MSMQ для передачи json файлов консольному приложению.
    virtual void OnStart(DWORD dwArgc, PWSTR* scmArgv);
    
    // Выполняется при остановке функции, очищает память, закрывает очередь MSMQ.
    virtual void OnStop();

    // Основная функция - тут будет бесконечный цикл
    virtual void Loop();

    // Устанавливает статус службы и сообщает его диспетчеру служб
    void SetServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode = NO_ERROR, DWORD dwWaitHint = 0);

    // Записывает в лог ошибку.
    void WriteErrorLogEntry(PWSTR scmFunction, DWORD dwError = GetLastError());

    void WriteEventLogEntry(PWSTR scmMessage, WORD wType);

private:

    // Входная точка службы
    static void WINAPI ServiceMain(DWORD dwArgc, LPWSTR* lscmArgv);

    // Обработчик исключений WINAPI, используется для управления службой со стороны SCM
    static void WINAPI ServiceCtrlHandler(DWORD dwCtrl);

    void Start(DWORD dwArgc, PWSTR* scmArgv);
    void Shutdown();

    // Инстанс службы, щас конст пачку соберём и пойдём.
    static WIN_Service_Support* s_service;

    // Имя службы
    PWSTR m_name;

    // Статус службы
    SERVICE_STATUS m_status;

    // Хендл статуса
    SERVICE_STATUS_HANDLE m_statusHandle;
};

