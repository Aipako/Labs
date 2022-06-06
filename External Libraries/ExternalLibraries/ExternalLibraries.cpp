// ExternalLibraries.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//


#include <nlohmann/json.hpp>
#include "SocketAPI.h"
#include "Valute_Fault.h"
#include <vector>
#include <iostream>
#include <fstream>
#define HOST_NAME "CentrobankAPI"
#define INTERPROCESS_PATH "E:\\temp.txt"

FILE* inputFile;
std::ifstream fin;
std::ofstream fout;
std::vector<Valute_Fault> Valutes;
bool firstLaunch = true;
nlohmann::json jsonFormat;

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
	if (fdwCtrlType == CTRL_C_EVENT)
	{
		system("cls");
		int i = 0;
		for (auto it : jsonFormat.at("Valute").items())
		{
			Valutes[i].newValueInsert(it.value().at("Value").get<float>() / it.value().at("Nominal").get<int>());

			std::cout << it.key() << ":    Average value:" << Valutes[i].getMiddleValue() << "		Median value:" << Valutes[i].getMedianValue() << std::endl;
			i++;
		}
	}
	return FALSE;
}



void parseJSONFile(nlohmann::json jsonFormat)
{
	system("cls");
	int i = 0;;
	for (auto it : jsonFormat.at("Valute").items())
	{
		Valutes[i].newValueInsert(it.value().at("Value").get<float>() / it.value().at("Nominal").get<int>());
		
		std::cout << it.key() << ":  " << Valutes[i].currentValue;
		if (i % 2 == 0)
		{
			std::cout << "\t\t\t";
		}
		else
			std::cout << "\n";
		i++;
	}

}



int main()
{
	SocketAPI socketMain;
	char hostbuffer[256];
	int Error = 0;
	if (!SetConsoleCtrlHandler(CtrlHandler, TRUE))
	{
		std::cout << "Error while creating exit-event handler. No information after end of work.\n";
	}
	/*Error = socketMain.OpenSocket();
	if (Error != 0)
	{
		std::cout << "Client Open Socket Fail" << std::endl;
		return -1;
	}
	memset(hostbuffer, 0, 255);
	int hostnameRet = gethostname(hostbuffer, sizeof(hostbuffer));
	Error = socketMain.GetHost(hostbuffer);
	if (Error != 0)
	{
		std::cout << "Unable to get host" << std::endl;
		return -1;
	}
	Error = socketMain.ClientConnect();
	if (Error != 0)
	{
		std::cout << "Unable to connect to service" << std::endl;
	}*/
	Valutes.resize(34);
	while (true)
	{
		//socketMain.ClientRead();
		std::string s;
		while (true)
		{

			fin.open(INTERPROCESS_PATH);
			fin >> s;
			if (s != "Readed")
			{
				break;
			}
			fin.close();

		}
		std::string stringJSON;
		fin.close();
		fopen_s(&inputFile,INTERPROCESS_PATH, "r");
		jsonFormat = nlohmann::json::parse(inputFile);
		fin.close();
		fout.open(INTERPROCESS_PATH);
		fout << "Readed";
		fout.close();
		parseJSONFile(jsonFormat);
		Sleep(10000);
	}

	socketMain.CloseSocket();

}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
