#include "GpuCapabilityTester.h"
#include <cassert>
#include <ostream>
#include <iostream>
#include <map>
#include <d3d9.h>
#include <d3d11.h>

#define SAFE_RELEASE( x ) if ( x ) { x->Release(); x = nullptr; }

using namespace std;

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

void GpuCapabilityTester::Run(bool& _bD3d9SupportOut, bool& _bD3d11SupportOut, unsigned& _uAdapterDeviceId)
{
	map<unsigned, bool> vD3d9Support;
	map<unsigned, bool> vD3d11Support;

	_uAdapterDeviceId = giNoOptimalAdapterFound;
	_bD3d9SupportOut = _bD3d11SupportOut = false;
	IDXGIFactory* pFactory = nullptr;

	// Creating a window, for the later usage in Direct3D9 Device creation
	WNDCLASS windowClass = { 0 };
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.hInstance = nullptr;
	windowClass.lpfnWndProc = WndProc;
	windowClass.lpszClassName = L"A dummy Window"; //needs to be the same name
	//when creating the window as well
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	//also register the class
	if (!RegisterClass(&windowClass))
	{
		LogMessageLine("Could not register a Window class");
	}

	HWND windowHandle = CreateWindowA("A dummy Window",
		nullptr,
		WS_POPUP, //borderless
		0, //x coordinate of window start point
		0, //y start point
		GetSystemMetrics(SM_CXSCREEN), //width of window; this function
		//retrieves the screen resolution.
		GetSystemMetrics(SM_CYSCREEN), //height of the window
		nullptr, //handles and such, not needed
		nullptr,
		nullptr,
		nullptr);
	//ShowWindow(windowHandle, SW_RESTORE);

	// Define DirectX 9 Creation Parameters

	unsigned int uCreationFlags = D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE | D3DCREATE_HARDWARE_VERTEXPROCESSING;

	D3DPRESENT_PARAMETERS presentParams;
	ZeroMemory(&presentParams, sizeof(presentParams));
	presentParams.BackBufferFormat = D3DFMT_A8R8G8B8;
	presentParams.BackBufferCount = 1;
	presentParams.MultiSampleType = D3DMULTISAMPLE_NONE;
	presentParams.MultiSampleQuality = 0;
	presentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParams.hDeviceWindow = windowHandle;
	presentParams.Windowed = TRUE;
	presentParams.EnableAutoDepthStencil = TRUE;
	presentParams.AutoDepthStencilFormat = D3DFMT_D24S8;
	presentParams.Flags = 0;
	presentParams.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	presentParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	presentParams.BackBufferWidth = 0;
	presentParams.BackBufferHeight = 0;

	LogMessageLine("### GPU Capability Test ###");

	LogMessageFormatted("   Is BGRA feature required: %s\n", m_bSupportBgra ? "TRUE" : "FALSE");

	// Create a DXGIFactory object.
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&pFactory))))
	{
		LogMessageLine("\nERROR: Unable to create IDXGI Factory.");
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

		LogMessageFormattedW(L"\nExamining Graphics Adapter: %s\n", adapterDesc.Description);
		LogMessageFormattedW(L"   VRAM: %dMb\n", adapterDesc.DedicatedVideoMemory >> 20);
		LogMessageFormattedW(L"   DeiceId: %d\n", adapterDesc.DeviceId);

		LogMessageLine("\n   Visual Xccelerator Engine Direct3D9 Compatibility");

		size_t rank = 0;
		size_t dx9AdapterIndex = 0;

		LogMessage("      Trying to create Direct3D9 Device... ");
		try
		{
			IDirect3D9* pD3d9 = Direct3DCreate9(D3D_SDK_VERSION);

			IDirect3DDevice9* pD3dDevice9 = nullptr;
			size_t iNumberAdapters = pD3d9->GetAdapterCount();
			for (; dx9AdapterIndex < iNumberAdapters; ++dx9AdapterIndex)
			{
				D3DADAPTER_IDENTIFIER9 ai;
				pD3d9->GetAdapterIdentifier(dx9AdapterIndex, 0, &ai);

				if (ai.DeviceId != adapterDesc.DeviceId)
				{
					continue;
				}

				pD3d9->CreateDevice(dx9AdapterIndex, D3DDEVTYPE_HAL, nullptr, uCreationFlags, &presentParams, &pD3dDevice9);
				break;
			}

			vD3d9Support[adapterDesc.DeviceId] = pD3dDevice9 != nullptr;
			SAFE_RELEASE(pD3dDevice9);
			SAFE_RELEASE(pD3d9);
		}
		catch (...)
		{
			vD3d9Support[adapterDesc.DeviceId] = false;
		}
		if (vD3d9Support[adapterDesc.DeviceId])
		{
			LogMessageLine("SUCCESS");
		}
		else
		{
			LogMessageLine("FAILED");
		}
		rank += 10;

		LogMessageLine("\n   Visual Xccelerator Engine Direct3D11 Compatibility");

		LogMessage("      Trying to create Direct3D9Ex Device (WPF Compatibility)... ");
		bool bD3d9ExSuccess;
		try
		{
			IDirect3D9Ex* pD3d9 = nullptr;
			IDirect3DDevice9Ex* pD3dDevice9 = nullptr;
			bD3d9ExSuccess =
				SUCCEEDED(Direct3DCreate9Ex(D3D_SDK_VERSION, &pD3d9)) &&
				SUCCEEDED(pD3d9->CreateDeviceEx(dx9AdapterIndex, D3DDEVTYPE_HAL, nullptr, uCreationFlags, &presentParams, nullptr, &pD3dDevice9));
			SAFE_RELEASE(pD3dDevice9);
			SAFE_RELEASE(pD3d9);
		}
		catch (...)
		{
			bD3d9ExSuccess = false;
		}
		if (bD3d9ExSuccess)
		{
			LogMessageLine("SUCCESS");
		}
		else
		{
			LogMessageLine("FAILED");
		}

		vD3d11Support[adapterDesc.DeviceId] = bD3d9ExSuccess;

		LogMessage("      Trying to create Direct3D11 Device... ");
		ID3D11Device* pDevice;
		D3D_FEATURE_LEVEL featureLevel;
		bool bD3d11Success;
		try
		{
			bD3d11Success = SUCCEEDED(D3D11CreateDevice(pAdapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr,
				m_bSupportBgra ? D3D11_CREATE_DEVICE_BGRA_SUPPORT : 0, nullptr, 0, 7,
				&pDevice, &featureLevel, nullptr));
		}
		catch (...)
		{
			bD3d11Success = false;
		}
		LogMessageLine(bD3d11Success ? "SUCCESS" : "FAILED");

		vD3d11Support[adapterDesc.DeviceId] &= bD3d11Success;

		if (!bD3d9ExSuccess && bD3d11Success)
		{
			LogMessageLine("\n      NOTE: the adapter is able to create Direct3D11 Device,");
			LogMessageLine("      hovewer for some reason it's unable to create DirectX9Ex Device.");
			LogMessageLine("      It looks like the adapter is not connected to a monitor.");
			LogMessageLine("      If you would like to run the SciChart on this adapter,");
			LogMessageLine("      please make sure that the monitor cable is plugged in.");
		}

		if (vD3d11Support[adapterDesc.DeviceId])
		{
			// Is Features Level sufficient to run Visual Xccelerator Engine using Direct3D11?
			double dMemoryRank = static_cast<double>(adapterDesc.DedicatedVideoMemory) / GetD3d11MinVRam();
			if (dMemoryRank >= 1.0)
			{
				rank += static_cast<size_t>(dMemoryRank * 100.0);
			}
			else
			{
				LogMessageLine("\n      NOTE: the amount of Video Memory (VRAM) isn't sufficient to run");
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
					LogMessageLine("\n      NOTE: the Graphics Adapter does not support Feature Level 10.1");
					break;
				case D3D_FEATURE_LEVEL_11_0:
					LogMessageLine("\n      NOTE: the Graphics Adapter does not support Feature Level 11.0");
					break;
				default:
					LogMessageLine("\n      NOTE: the Graphics Adapter does not support specified Feature Level.");
					break;
				}
				LogMessageLine("               The Visual Xccelerator Engine will use Direct3D 9, if this adapter is being used.");
				LogMessageLine("               This might tend to low performance or visual errors.");
			}
		}

		LogMessageFormatted("\n   Rank: %d Points\n", rank);

		if (rank >= bestRank)
		{
			_uAdapterDeviceId = adapterDesc.DeviceId;
			bestRank = rank;
		}

		SAFE_RELEASE(pDevice);
	}

	if (_uAdapterDeviceId != giNoOptimalAdapterFound)
	{
		_bD3d9SupportOut = vD3d9Support[_uAdapterDeviceId];
		_bD3d11SupportOut = vD3d11Support[_uAdapterDeviceId];

		LogMessageFormatted("\nSelected Graphics Adapter, where DeviceId is: %d\n", _uAdapterDeviceId);
		LogMessageFormatted("   Is Direct3D9 Supported: %s\n", _bD3d9SupportOut ? "TRUE" : "FALSE");
		LogMessageFormatted("   Is Direct3D11 Supported: %s\n", _bD3d11SupportOut ? "TRUE" : "FALSE");
	}

	DeleteObject(windowHandle); //doing it just in case
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wparam, lparam);
	}
}