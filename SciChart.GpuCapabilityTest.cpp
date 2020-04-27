// SciChart.GpuCapabilityTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <d3d9.h>
#include <d3d11.h>

#define SAFE_RELEASE( x ) if ( x ) { x->Release(); x = NULL; }

int main()
{
    std::cout << "### GPU Capability Test ###\n";

	bool bSuccess;
	
	/// Direct3D9 Compatibility
	
	std::cout << "\n   Visual Xccelerator Engine Direct3D9 Compatibility\n";
	
	try
    {
		std::cout << "      Trying to create Direct3D9 Device... ";

		IDirect3D9* pID3D = Direct3DCreate9(D3D_SDK_VERSION);
	    bSuccess = (pID3D != nullptr);
	    SAFE_RELEASE(pID3D);
    }
    catch (...)
    {
		bSuccess = false;
    }
	std::cout << (bSuccess ? "SUCCESS" : "FAILED") << std::endl;
	
	/// Direct3D11 Compatibility
	
	std::cout << "\n   Visual Xccelerator Engine Direct3D11 Compatibility\n";

	ID3D11Device* pDevice = nullptr;
	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr;
	try
    {
		std::cout << "      Trying to create Direct3D11 Device... ";
    	
	    hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 
			D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG,
			nullptr, 0, 7, &pDevice, &featureLevel, nullptr);
	    bSuccess = SUCCEEDED(hr);
    }
    catch (...)
    {
		bSuccess = false;
    }
	std::cout << (bSuccess ? "SUCCESS" : "FAILED") << std::endl;

	if (bSuccess)
	{
		// Is Features Level sufficient to run Visual Xccelerator Engine using Direct3D11?
		if (featureLevel < D3D_FEATURE_LEVEL_10_0)
		{
			std::cout << "      NOTE: the Graphics Adapter does not support Feature Level 10.0.\n";
			std::cout << "            Visual Xccelerator Engine will fallback to Direct3D 9.\n";
			std::cout << "            This might tend to low performance or visual errors.\n";
		}
		// Is Feature Level 11.0 supported?
		if (featureLevel < D3D_FEATURE_LEVEL_11_0)
		{
			std::cout << "      NOTE: the Graphics Adapter does not support Feature Level 11.0.\n";
			std::cout << "            This might tend to low performance or visual errors.\n";
		}

		const int ciSufficientVRamSizeMb = 256;
		std::cout << "      Determining whether the Graphics Adapter has sufficient size of VRAM (" << ciSufficientVRamSizeMb << "Mb)... ";
		IDXGIDevice* pDXGIDevice;
		hr = pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);
		IDXGIAdapter* pDXGIAdapter;
		pDXGIDevice->GetAdapter(&pDXGIAdapter);
		DXGI_ADAPTER_DESC adapterDesc;
		pDXGIAdapter->GetDesc(&adapterDesc);
		int iAdapterVRamMb = adapterDesc.DedicatedVideoMemory / 1024 / 1024;
		bSuccess = iAdapterVRamMb >= ciSufficientVRamSizeMb;
		SAFE_RELEASE(pDXGIAdapter);
		std::cout << iAdapterVRamMb << "Mb " << (bSuccess ? "SUCCESS" : "FAILED") << std::endl;
	}

	SAFE_RELEASE(pDevice);
}
