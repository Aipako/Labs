#pragma once
void InstallService(PWSTR scmServiceName, PWSTR scmDisplayName, DWORD dwStartType, PWSTR scmDependencies);
void UninstallService(PWSTR scmServiceName);
