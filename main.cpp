// SciChart.GpuCapabilityTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "GpuCapabilityTester.h"
#include <iostream>

using namespace std;

int main()
{
	bool _bD3d9SupportOut, _bD3d11SupportOut;
	int _uAdapterIndexOut;
    GpuCapabilityTester tester;
	tester.SetOutputToConsole(true);
	tester.Run(_bD3d9SupportOut, _bD3d11SupportOut, _uAdapterIndexOut);

	cout << "\nPress any key to exit...";
	getchar();
}
