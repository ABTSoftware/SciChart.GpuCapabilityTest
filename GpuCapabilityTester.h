#pragma once

#include <d3d9.h>
#include <d3d11.h>
#include <vector>
#include <string>
#include <map>

#define SAFE_RELEASE( x ) if ( x ) { x->Release(); x = NULL; }

// Min amount of VRAM required by Visual Xccelerator Engine
static const size_t gcVxD3d11MinVRam = 256 << 20; // 256Mb

class GpuCapabilityTester
{
public:
	GpuCapabilityTester();
	~GpuCapabilityTester();

	bool TryCreateDirect3D9Device();
	size_t GetNumberDxgiAdapters() const;
	bool TryCreateDirect3D11Device(size_t _uAdapterIndex, bool _bSupportBgra);
	bool CheckDirect3D11FeatureLevel10(size_t _uAdapterIndex) const;
	bool CheckDirect3D11FeatureLevel11(size_t _uAdapterIndex) const;
	size_t RankDxgiAdapter(size_t _uAdapterIndex);
	size_t GetDxgiAdapterVRam(size_t _uAdapterIndex) const;
	std::wstring GetDxgiAdapterName(size_t _uAdapterIndex) const;
	bool CheckDirect3D11IsPreferred(size_t _uAdapterIndex, bool _bUseLowerFeatureLevel) const;

private:
	IDirect3D9* m_pD3d9Device = nullptr;
	std::vector<IDXGIAdapter*> m_vDxgiAdapters{};
	std::map<size_t, std::tuple<ID3D11Device*, D3D_FEATURE_LEVEL>> m_mapD3d11DevicesAndFeatures{};
};

