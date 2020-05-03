#include "GpuCapabilityTester.h"
#include <cassert>
#include <tuple>

#define SAFE_RELEASE( x ) if ( x ) { x->Release(); x = NULL; }

using namespace std;

// Helper functions
bool TryFindDevice(const std::map<size_t, std::tuple<ID3D11Device*, D3D_FEATURE_LEVEL>>& _map, size_t _uAdapterIndex, ID3D11Device*& _pDeviceOut);
bool TryFindFeature(const std::map<size_t, std::tuple<ID3D11Device*, D3D_FEATURE_LEVEL>>& _map, size_t _uAdapterIndex, D3D_FEATURE_LEVEL& _FeatureOut);

GpuCapabilityTester::GpuCapabilityTester()
{
	IDXGIAdapter* pAdapter;
	IDXGIFactory* pFactory = nullptr;
	
	// Create a DXGIFactory object.
	if (SUCCEEDED(CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&pFactory))))
	{
		for (UINT i = 0; pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			m_vDxgiAdapters.push_back(pAdapter);
		}

		SAFE_RELEASE(pFactory);
	}
}

GpuCapabilityTester::~GpuCapabilityTester()
{
	SAFE_RELEASE(m_pD3d9Device);
	for(auto& pair : m_mapD3d11DevicesAndFeatures)
	{
		get<0>(pair.second)->Release();
	}
	m_mapD3d11DevicesAndFeatures.clear();
}

bool GpuCapabilityTester::TryCreateDirect3D9Device()
{
	try
	{
		SAFE_RELEASE(m_pD3d9Device);
		m_pD3d9Device = Direct3DCreate9(D3D_SDK_VERSION);
		return m_pD3d9Device != nullptr;
	}
	catch (...)
	{
		return false;
	}
}

size_t GpuCapabilityTester::GetNumberDxgiAdapters() const
{
	return m_vDxgiAdapters.size();
}

bool GpuCapabilityTester::TryCreateDirect3D11Device(size_t _uAdapterIndex, bool _bSupportBgra)
{
	assert(_uAdapterIndex < m_vDxgiAdapters.size() && "Argument _uAdapterIndex is out of range!");
	
	try
	{
		// Remove previously created Device for the specified adapter, if any
		ID3D11Device* pDevice;
		if (TryFindDevice(m_mapD3d11DevicesAndFeatures, _uAdapterIndex, pDevice))
		{
			pDevice->Release();
			m_mapD3d11DevicesAndFeatures.erase(_uAdapterIndex);
		}
		
		D3D_FEATURE_LEVEL featureLevel;
		HRESULT hr = D3D11CreateDevice(/*m_vDxgiAdapters[_uAdapterIndex]*/nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
			 _bSupportBgra ? D3D11_CREATE_DEVICE_BGRA_SUPPORT : 0, nullptr, 0, 7,
			&pDevice, &featureLevel, nullptr);

		if (SUCCEEDED(hr))
		{
			m_mapD3d11DevicesAndFeatures.insert({ _uAdapterIndex, { pDevice , featureLevel } });
			
			return true;
		}

		return false;
	}
	catch (...)
	{
		return false;
	}
}

bool GpuCapabilityTester::CheckDirect3D11FeatureLevel10(size_t _uAdapterIndex) const
{
	D3D_FEATURE_LEVEL featureLevel;
	if (!TryFindFeature(m_mapD3d11DevicesAndFeatures, _uAdapterIndex, featureLevel))
	{
		assert(0 && "First, call TryCreateDirect3D11Device() with the same adapter index and make sure it succeedes!");
	}

	return featureLevel >= D3D_FEATURE_LEVEL_10_0;
}

bool GpuCapabilityTester::CheckDirect3D11FeatureLevel11(size_t _uAdapterIndex) const
{
	D3D_FEATURE_LEVEL featureLevel;
	if (!TryFindFeature(m_mapD3d11DevicesAndFeatures, _uAdapterIndex, featureLevel))
	{
		assert(0 && "First, call TryCreateDirect3D11Device() with the same adapter index and make sure it succeedes!");
	}

	return featureLevel >= D3D_FEATURE_LEVEL_11_0;
}

size_t GpuCapabilityTester::RankDxgiAdapter(size_t _uAdapterIndex)
{
	size_t rank = 0;

	if (TryCreateDirect3D9Device())
	{
		rank += 10;

		if (TryCreateDirect3D11Device(_uAdapterIndex, true) &&
			CheckDirect3D11FeatureLevel10(_uAdapterIndex) &&
			GetDxgiAdapterVRam(_uAdapterIndex) >= gcVxD3d11MinVRam)
		{
			rank += 100 * GetDxgiAdapterVRam(_uAdapterIndex) / gcVxD3d11MinVRam;

			if (CheckDirect3D11FeatureLevel11(_uAdapterIndex))
			{
				rank += 100;
			}
		}
	}
	
	return rank;
}

bool GpuCapabilityTester::IsMicrosoftBasicRenderDriver(size_t _uAdapterIndex) const
{
	assert(_uAdapterIndex < m_vDxgiAdapters.size() && "Argument _uAdapterIndex is out of range!");

	DXGI_ADAPTER_DESC adapterDesc;
	m_vDxgiAdapters[_uAdapterIndex]->GetDesc(&adapterDesc);
	return wcscmp(adapterDesc.Description, L"Microsoft Basic Render Driver") == 0;
}

size_t GpuCapabilityTester::GetDxgiAdapterVRam(size_t _uAdapterIndex) const
{
	assert(_uAdapterIndex < m_vDxgiAdapters.size() && "Argument _uAdapterIndex is out of range!");

	DXGI_ADAPTER_DESC adapterDesc;
	m_vDxgiAdapters[_uAdapterIndex]->GetDesc(&adapterDesc);
	
	return adapterDesc.DedicatedVideoMemory;
}

wstring GpuCapabilityTester::GetDxgiAdapterName(size_t _uAdapterIndex) const
{
	assert(_uAdapterIndex < m_vDxgiAdapters.size() && "Argument _uAdapterIndex is out of range!");

	DXGI_ADAPTER_DESC adapterDesc;
	m_vDxgiAdapters[_uAdapterIndex]->GetDesc(&adapterDesc);
	wstring ret{ adapterDesc.Description };
	
	return ret;
}

bool GpuCapabilityTester::CheckDirect3D11IsPreferred(size_t _uAdapterIndex, bool _bUseLowerFeatureLevel) const
{
	return CheckDirect3D11FeatureLevel10(_uAdapterIndex) &&
		(!_bUseLowerFeatureLevel || CheckDirect3D11FeatureLevel11(_uAdapterIndex)) &&
		GetDxgiAdapterVRam(_uAdapterIndex) >= gcVxD3d11MinVRam;
}

size_t GpuCapabilityTester::SuggestDxgiAdapterIndex()
{
	size_t uBestRank = 0;
	size_t uBestAdapterIndex = (std::numeric_limits<size_t>::max)();
	for (size_t i = 0; i < GetNumberDxgiAdapters(); ++i)
	{
		if (IsMicrosoftBasicRenderDriver(i))
		{
			continue;
		}

		size_t curRank = RankDxgiAdapter(i);
		if (curRank > uBestRank)
		{
			uBestRank = curRank;
			uBestAdapterIndex = i;
		}
	}

	return uBestAdapterIndex;
}

bool TryFindDevice(const std::map<size_t, std::tuple<ID3D11Device*, D3D_FEATURE_LEVEL>>& _map, size_t _uAdapterIndex, ID3D11Device*& _pDeviceOut)
{
	const auto itrDeviceAndFeature = _map.find(_uAdapterIndex);
	if (itrDeviceAndFeature == _map.end())
	{
		return false;
	}

	_pDeviceOut = get<0>(itrDeviceAndFeature->second);
	return true;
}

bool TryFindFeature(const std::map<size_t, std::tuple<ID3D11Device*, D3D_FEATURE_LEVEL>>& _map, size_t _uAdapterIndex, D3D_FEATURE_LEVEL& _FeatureOut)
{
	const auto itrDeviceAndFeature = _map.find(_uAdapterIndex);
	if (itrDeviceAndFeature == _map.end())
	{
		return false;
	}

	_FeatureOut = get<1>(itrDeviceAndFeature->second);
	return true;
}
