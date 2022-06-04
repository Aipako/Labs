// Custom allocator.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
//#define CUSTOM_ALLOCATOR_DEBUG

#include <iostream>
#include <vector>
#include "CustomAllocator.h"
#include <ctime>
using namespace std;



int main()
{
	clock_t stdAllocatorTimeStart, customAllocatorTimeStart;
	clock_t stdAllocatorTimeEnd, customAllocatorTimeEnd;

	customAllocatorTimeStart = clock();
    vector<int, CustomAllocator<int>> vecCustom;
	customAllocatorTimeEnd = clock();

	stdAllocatorTimeStart = clock();
	vector<int> vecSTD;
	stdAllocatorTimeEnd = clock();

	cout << "Allocator construction:  STD-" << difftime(stdAllocatorTimeEnd, stdAllocatorTimeStart) << "	Custom-" << difftime(customAllocatorTimeEnd, customAllocatorTimeStart) << endl;
	int c = 1;
	while (c != 0)
	{
		cin >> c;
		stdAllocatorTimeStart = clock();
		vecSTD.push_back(c);
		stdAllocatorTimeEnd = clock();

		customAllocatorTimeStart = clock();
		vecCustom.push_back(c);
		customAllocatorTimeEnd = clock();
		cout << "New ellement added:  STD-" << difftime(stdAllocatorTimeEnd, stdAllocatorTimeStart) << "	Custom-" << difftime(customAllocatorTimeEnd, customAllocatorTimeStart) << endl;
	}
	for (auto &v : vecSTD)
	{
		cout << v << endl;
	}
    std::cout << "\n";
}
