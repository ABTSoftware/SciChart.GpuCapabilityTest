// SciChart.GpuCapabilityTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "GpuCapabilityTester.h"
#include <iostream>

using namespace std;

int main()
{
	GpuCapabilityTester tester;
	tester.SetOutputToConsole(true);
	tester.SetOutputToFile(true);
	GpuRequirements reqs{};
	auto caps = tester.FindOptimalAdapter( reqs );

	cout << "\nPress any key to exit...";
	getchar();
}
