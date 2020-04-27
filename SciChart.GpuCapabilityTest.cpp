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
		bSuccess = true;
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

		/// Try create D32f ([SC-5383] Fails on Intel HD 4600)
		try
		{
			std::cout << "      Trying to create D32_FLOAT Texture 2D... ";
			
			D3D11_TEXTURE2D_DESC textureDesc;
			textureDesc.ArraySize = 1;
			textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.Format = DXGI_FORMAT_D32_FLOAT;
			textureDesc.Height = 1;
			textureDesc.Width = 1;
			textureDesc.MipLevels = 1;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.MiscFlags = 0;
			ID3D11Texture2D* pTexture2D;
			hr = pDevice->CreateTexture2D(&textureDesc, nullptr, &pTexture2D);
			if (FAILED(hr))
			{
				bSuccess = false;
			}
			SAFE_RELEASE(pTexture2D);
		}
		catch (...)
		{
			bSuccess = false;
		}
		std::cout << (bSuccess ? "SUCCESS" : "FAILED") << std::endl;
		
		// Try create B8G8R8A8 ([SC-5383] Fails on Intel HD 4600)
		try
		{
			std::cout << "      Trying to create B8G8R8A8_UNORM Texture 2D... ";

			D3D11_TEXTURE2D_DESC textureDesc;
			textureDesc.ArraySize = 1;
			textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			textureDesc.Height = 1;
			textureDesc.Width = 1;
			textureDesc.MipLevels = 1;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.MiscFlags = 0;
			ID3D11Texture2D* pTexture2D;
			hr = pDevice->CreateTexture2D(&textureDesc, nullptr, &pTexture2D);
			if (FAILED(hr))
			{
				bSuccess = false;
			}
			SAFE_RELEASE(pTexture2D);
		}
		catch (...)
		{
			bSuccess = false;
		}
		std::cout << (bSuccess ? "SUCCESS" : "FAILED") << std::endl;
	}
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
