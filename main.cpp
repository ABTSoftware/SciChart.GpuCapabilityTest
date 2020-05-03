// SciChart.GpuCapabilityTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "GpuCapabilityTester.h"
#include <iostream>

using namespace std;

int main()
{
    cout << "### GPU Capability Test ###\n";

	GpuCapabilityTester tester;
	
	/// Direct3D9 Compatibility
	
	cout << "\n   Visual Xccelerator Engine Direct3D9 Compatibility\n";
	
	cout << "      Trying to create Direct3D9 Device... ";
	cout << (tester.TryCreateDirect3D9Device() ? "SUCCESS" : "FAILED") << endl;
	
	/// Direct3D11 Compatibility
	
	cout << "\n   Visual Xccelerator Engine Direct3D11 Compatibility\n";

	for (size_t i = 0; i < tester.GetNumberDxgiAdapters(); ++i)
    {
		wcout << L"      Examining Graphics Adapter " << tester.GetDxgiAdapterName(i) << L" ... " << endl;

		cout << "         Trying to create Direct3D11 Device... ";
		if (!tester.TryCreateDirect3D11Device(i, false))
		{
			cout << "FAILED" << endl;
			continue;
		}
	    cout << "SUCCESS" << endl;

		cout << "         Trying to create Direct3D11 Device with BGRA support... ";
		if (!tester.TryCreateDirect3D11Device(i, true))
		{
			cout << "FAILED" << endl;
			continue;
		}
	    cout << "SUCCESS" << endl;

		cout << "         VRAM: " << (tester.GetDxgiAdapterVRam(i) >> 20) << "Mb" << endl;
		cout << "         Rank: " << tester.RankDxgiAdapter(i) << " points" << endl;
		
		// Is Features Level sufficient to run Visual Xccelerator Engine using Direct3D11?
	    if (tester.GetDxgiAdapterVRam(i) < gcVxD3d11MinVRam)
	    {
		    cout << "         NOTE: the amount of Video Memory (VRAM) isn't sufficient to run.\n";
		    cout << "               the Visual Xccelerator Engine on this adapter using Direct3D 11.\n";
		    cout << "               It will fallback to DirectX 9, if this adapter is used.\n";
			cout << "               This might tend to low performance or visual errors.\n";
	    }
		
		// Is Features Level sufficient to run Visual Xccelerator Engine using Direct3D11?
	    if (!tester.CheckDirect3D11FeatureLevel10(i))
	    {
		    cout << "         NOTE: the Graphics Adapter does not support Feature Level 10.0.\n";
		    cout << "               If Visual Xccelerator Engine would use Direct3D 9, if the adapter used.\n";
		    cout << "               This might tend to low performance or visual errors.\n";
	    }
		
	    // Is Feature Level 11.0 supported?
	    if (!tester.CheckDirect3D11FeatureLevel11(i))
	    {
		    cout << "         NOTE: the Graphics Adapter does not support Feature Level 11.0.\n";
			cout << "               If Visual Xccelerator Engine would use Direct3D 9, if the adapter used and low feature level is avoided.\n";
		    cout << "               This might tend to low performance or visual errors.\n";
	    }
    }
}
