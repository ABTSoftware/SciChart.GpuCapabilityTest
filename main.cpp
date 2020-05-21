// SciChart.GpuCapabilityTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "GpuCapabilityTester.h"
#include <iostream>

using namespace std;

int main()
{
	GpuCapabilityTester tester;
	tester.SetOutputToFile(true);
	GpuRequirements reqs{};
	auto caps = tester.FindOptimalAdapter( reqs );
	wcout << caps.m_srtLogMessages;
	wcout << L"\nPress any key to exit...";
	getchar();
}
