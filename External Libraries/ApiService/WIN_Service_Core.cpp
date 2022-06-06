#define CURL_STATICLIB
#ifdef _DEBUG
#pragma comment(lib,"libcurld.lib")
#else
#pragma comment(lib,"libcurl.lib")
#endif

#pragma comment(lib,"ws2_32.lib")  // Зависимость от WinSocks2
#pragma comment(lib,"wldap32.lib")  
#include "pch.h"
#include "WIN_Service_Core.h"
#include "SocketAPI.h"
#include <windows.h>
#include <io.h>
#include <sys/types.h>
#include <string>
#include <errno.h>
#include <fstream>
#include "x64/Debug/libcurl/curl.h"


CURL* mainHandle;

SocketAPI socketMain;


WIN_Service_Core::WIN_Service_Core(PWSTR scmServiceName, BOOL fCanStop, BOOL fCanShutdown, BOOL fCanPauseContinue) : WIN_Service_Support(scmServiceName, fCanStop, fCanShutdown, fCanPauseContinue)
{
    m_fStopping = FALSE;

    // Create a manual-reset event that is not signaled at first to indicate 
    // the stopped signal of the service.
    m_hStoppedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (m_hStoppedEvent == NULL)
    {
        throw GetLastError();
    }
}


WIN_Service_Core::~WIN_Service_Core(void)
{
    if (m_hStoppedEvent)
    {
        CloseHandle(m_hStoppedEvent);
        m_hStoppedEvent = NULL;
    }
}


void WIN_Service_Core::OnStart(DWORD dwArgc, LPWSTR* scmArgv)
{
    // Пишем в лог
    WriteEventLogEntry(L"CentrobankAPI in OnStart", EVENTLOG_INFORMATION_TYPE);

    //Создаём сокет (да, да, не удивляйтесь.... Вот бы ещё документацию на MSMQ найти.....)
    int error = 0;
    error = socketMain.OpenSocket(L"CentrobankAPI");
    if(error != 0)
        WriteEventLogEntry(L"CentrobankAPI: Socket open fail", EVENTLOG_INFORMATION_TYPE);
	error = socketMain.BindServer();
	if(error != 0)
	{
        WriteEventLogEntry(L"CentrobankAPI: Socket bind fail", EVENTLOG_INFORMATION_TYPE);
	}

	error = socketMain.ListenServer();
	if(error != 0)
	{
        WriteEventLogEntry(L"CentrobankAPI: Socket start listening fail", EVENTLOG_INFORMATION_TYPE);
	}
}

int getJSONString(char* ptr, size_t size, size_t nmemb, std::string* data)
{
    if (data)
    {
        data->append(ptr, size * nmemb);
        return size * nmemb;
    }
    else
        return 0;  // будет ошибка
}

void WIN_Service_Core::Loop()
{

    WriteEventLogEntry(L"CentrobankAPI: Service start complete", EVENTLOG_INFORMATION_TYPE);
    mainHandle = curl_easy_init();
    std::string dataPOST;
    std::ofstream fout;
    while (true)
    {
        dataPOST.erase();
        fout.open("E:\\temp.txt");
        curl_easy_setopt(mainHandle, CURLOPT_URL, "https://www.cbr-xml-daily.ru/daily_json.js");
        curl_easy_setopt(mainHandle, CURLOPT_HTTPGET, 1);
        curl_easy_setopt(mainHandle, CURLOPT_WRITEFUNCTION, getJSONString);
        curl_easy_setopt(mainHandle, CURLOPT_WRITEDATA, &dataPOST);
        curl_easy_perform(mainHandle);
        fout << dataPOST;
        fout.close();
        //socketMain.SendServer(dataPOST, L"CentrobankAPI");
        WriteEventLogEntry(L"CentrobankAPI: Data catched", EVENTLOG_INFORMATION_TYPE);
        
        std::ifstream file;
        while (true)
        {
            try
            {
                file.open("E:\\temp.txt");
                std::string s;
                file >> s;
                if (s == "Readed")
                    break;

                file.close();
                Sleep(1000);
            }
            catch (const std::exception&)
            {
                file.close();
                break;
            }
            
        }
        Sleep(10000);

        // несколько классов для обслуживания одного HTTP запроса, на этом смысл службы заканчивается
    }
    fout.close();
    
}

void WIN_Service_Core::OnStop()
{
    // Пишем в лог
    WriteEventLogEntry(L"CentrobankAPI in OnStop", EVENTLOG_INFORMATION_TYPE);

    // Закрываем сокет
    socketMain.CloseSocket();
    
    // закрываем curl
    curl_easy_cleanup(mainHandle);

    // Ждём остановки
    m_fStopping = TRUE;
    if (WaitForSingleObject(m_hStoppedEvent, INFINITE) != WAIT_OBJECT_0)
    {
        throw GetLastError();
        // остановка служюы с ошибкй?, ладно, сказали так надо
    }
}