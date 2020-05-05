#include "GpuCapabilityTester.h"
#include <cassert>
#include <ostream>
#include <d3d9.h>
#include <d3d11.h>
#include <iostream>

#define SAFE_RELEASE( x ) if ( x ) { x->Release(); x = NULL; }

using namespace std;

void GpuCapabilityTester::Run(bool& _bD3d9SupportOut, bool& _bD3d11SupportOut, int& _uAdapterIndexOut)
{
	_bD3d9SupportOut = _bD3d11SupportOut = false;
	IDXGIFactory* pFactory = nullptr;

	LogMessageLine("### GPU Capability Test ###");

	// Create a DXGIFactory object.
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&pFactory))))
	{
		LogMessageLineW(L"\nERROR: Unable to create IDXGI Factory.");
	}

	IDXGIAdapter* pAdapter;
	size_t bestRank = 0;
	for (UINT i = 0; pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		DXGI_ADAPTER_DESC adapterDesc;
		pAdapter->GetDesc(&adapterDesc);

		// Skip Microsoft Basic Render Driver
		if (wcscmp(adapterDesc.Description, L"Microsoft Basic Render Driver") == 0)
		{
			continue;
		}

		LogMessageLine("\nExamining Graphics Adapter");
		LogMessageFormattedW(L"\n#%d: %s %dMb\n", i, adapterDesc.Description, adapterDesc.DedicatedVideoMemory >> 20);

		LogMessageLine("\n   Visual Xccelerator Engine Direct3D9 Compatibility");

		size_t rank = 0;

		LogMessage("      Trying to create Direct3D9 Device... ");
		try
		{
			IDirect3D9* pDevice = Direct3DCreate9(D3D_SDK_VERSION);
			_bD3d9SupportOut = pDevice != nullptr;
			SAFE_RELEASE(pDevice);
		}
		catch (...)
		{
			_bD3d9SupportOut = false;
		}
		if (_bD3d9SupportOut)
		{
			LogMessageLine("SUCCESS");
		}
		else
		{
			LogMessageLine("FAILED");
			// Skip
			continue;
		}
		rank += 10;

		LogMessageLine("\n   Visual Xccelerator Engine Direct3D11 Compatibility");

		LogMessageFormatted("      Is BGRA feature required: %s\n", m_bSupportBgra ? "TRUE" : "FALSE");
		LogMessage("      Trying to create Direct3D11 Device... ");
		ID3D11Device* pDevice;
		D3D_FEATURE_LEVEL featureLevel;
		bool bSuccess;
		try
		{
			HRESULT hr = D3D11CreateDevice(pAdapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr,
				m_bSupportBgra ? D3D11_CREATE_DEVICE_BGRA_SUPPORT : 0, nullptr, 0, 7,
				&pDevice, &featureLevel, nullptr);

			bSuccess = SUCCEEDED(hr);
		}
		catch (...)
		{
			bSuccess = false;
		}
		LogMessageLine(bSuccess ? "SUCCESS" : "FAILED");

		if (bSuccess)
		{
			// Is Features Level sufficient to run Visual Xccelerator Engine using Direct3D11?
			double dMemoryRank = static_cast<double>(adapterDesc.DedicatedVideoMemory) / GetD3d11MinVRam();
			if (dMemoryRank >= 1.0)
			{
				rank += static_cast<size_t>(dMemoryRank * 100.0);
			}
			else
			{
				LogMessageLine("      NOTE: the amount of Video Memory (VRAM) isn't sufficient to run");
				LogMessageLine("         the Visual Xccelerator Engine on this adapter using Direct3D 11.");
				LogMessageLine("         It will fallback to DirectX 9, if this adapter is being used.");
				LogMessageLine("         This might tend to low performance or visual errors.");
			}

			// Is Features Level sufficient to run Visual Xccelerator Engine using Direct3D11?
			if (featureLevel >= m_D3d11MinFeatureLevel)
			{
				rank += 1000;
			}
			else
			{
				switch (m_D3d11MinFeatureLevel)
				{
				case D3D_FEATURE_LEVEL_10_1:
					LogMessageLine("      NOTE: the Graphics Adapter does not support Feature Level 10.1");
					break;
				case D3D_FEATURE_LEVEL_11_0:
					LogMessageLine("      NOTE: the Graphics Adapter does not support Feature Level 11.0");
					break;
				default:
					LogMessageLine("      NOTE: the Graphics Adapter does not support specified Feature Level.");
					break;
				}
				LogMessageLine("               The Visual Xccelerator Engine will use Direct3D 9, if this adapter is being used.");
				LogMessageLine("               This might tend to low performance or visual errors.");
			}
		}

		LogMessageFormatted("      Rank: %d Points\n", rank);

		if (rank >= bestRank)
		{
			_bD3d11SupportOut = true;
			_uAdapterIndexOut = i;
		}

		SAFE_RELEASE(pDevice);
	}

	if (_bD3d9SupportOut)
	{
		LogMessageFormatted("\nSelected Graphics Adapter: #%d\n", _uAdapterIndexOut);
	}
	LogMessageFormatted("   Is Direct3D9 Supported: %s\n", _bD3d9SupportOut ? "TRUE" : "FALSE");
	LogMessageFormatted("   Is Direct3D11 Supported: %s\n", _bD3d11SupportOut ? "TRUE" : "FALSE");

	SAFE_RELEASE(pFactory);
}

void GpuCapabilityTester::LogMessage(const char* _acMsg) const
{
	if (m_bVerbose)
	{
		OutputDebugStringA(_acMsg);

		if (m_bOutputToConsole)
		{
			cout << _acMsg;
		}
	}
}

void GpuCapabilityTester::LogMessageFormatted(const char* _acFormat, ...) const
{
	if (m_bVerbose)
	{
		char aBuffer[1024];
		va_list _ArgList;
		__crt_va_start(_ArgList, _acFormat);
#pragma warning(suppress:28719)    // 28719
		vsnprintf(aBuffer, 1024, _acFormat, _ArgList);
		__crt_va_end(_ArgList);

		OutputDebugStringA(aBuffer);

		if (m_bOutputToConsole)
		{
			cout << aBuffer;
		}
	}
}

void GpuCapabilityTester::LogMessageW(const wchar_t* _acMsg) const
{
	if (m_bVerbose)
	{
		OutputDebugStringW(_acMsg);

		if (m_bOutputToConsole)
		{
			wcout << _acMsg;
		}
	}
}

void GpuCapabilityTester::LogMessageFormattedW(const wchar_t* _acFormat, ...) const
{
	if (m_bVerbose)
	{
		wchar_t aBuffer[1024];
		va_list _ArgList;
		__crt_va_start(_ArgList, _acFormat);
#pragma warning(suppress:28719)    // 28719
		vswprintf(aBuffer, 1024, _acFormat, _ArgList);
		__crt_va_end(_ArgList);

		OutputDebugStringW(aBuffer);

		if (m_bOutputToConsole)
		{
			wcout << aBuffer;
		}
	}
}

void GpuCapabilityTester::LogMessageLine(const char* _acMsg) const
{
	if (m_bVerbose)
	{
		OutputDebugStringA(_acMsg);
		OutputDebugStringA("\n");

		if (m_bOutputToConsole)
		{
			wcout << _acMsg << endl;
		}
	}
}

void GpuCapabilityTester::LogMessageLineW(const wchar_t* _acMsg) const
{
	if (m_bVerbose)
	{
		OutputDebugStringW(_acMsg);
		OutputDebugStringW(L"\n");

		if (m_bOutputToConsole)
		{
			wcout << _acMsg << endl;
		}
	}
}
