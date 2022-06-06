#pragma once
#include "WIN_Service_Support.h"
class WIN_Service_Core : public WIN_Service_Support
{
public:

    WIN_Service_Core(PWSTR scmServiceName, BOOL fCanStop = TRUE, BOOL fCanShutdown = TRUE, BOOL fCanPauseContinue = FALSE);
    virtual ~WIN_Service_Core(void);

protected:

    virtual void OnStart(DWORD dwArgc, PWSTR* scmArgv);
    virtual void OnStop();
    virtual void Loop();
    //void ServiceWorkerThread(void);

private:

    BOOL m_fStopping;
    HANDLE m_hStoppedEvent;
};

