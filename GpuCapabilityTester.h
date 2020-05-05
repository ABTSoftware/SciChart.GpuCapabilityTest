#pragma once

class GpuCapabilityTester
{
public:
	size_t GetD3d11MinFeatureLevel() const { return m_D3d11MinFeatureLevel; }
	void SetD3d11MinFeatureLevel(size_t _uValue) { m_D3d11MinFeatureLevel = _uValue; }

	size_t GetD3d11MinVRam() const { return m_uD3d11MinVRam; }
	void SetD3d11MinVRam(size_t _uValue) { m_uD3d11MinVRam = _uValue; }

	size_t GetSupportBgra() const { return m_bSupportBgra; }
	void SetSupportBgra(bool _bValue) { m_bSupportBgra = _bValue; }

	size_t GetVerbose() const { return m_bVerbose; }
	void SetVerbose(bool _bValue) { m_bVerbose = _bValue; }

	size_t GetOutputToConsole() const { return m_bOutputToConsole; }
	void SetOutputToConsole(bool _bValue) { m_bOutputToConsole = _bValue; }

	void Run(bool& _bD3d9SupportOut, bool& _bD3d11SupportOut, int& _uAdapterIndexOut);

private:
	void LogMessage(const char* _acMsg) const;
	void LogMessageFormatted(const char* _acFormat, ...) const;
	void LogMessageW(const wchar_t* _acMsg) const;
	void LogMessageFormattedW(const wchar_t* _acFormat, ...) const;
	void LogMessageLine(const char* _acMsg) const;
	void LogMessageLineW(const wchar_t* _acMsg) const;

	size_t m_D3d11MinFeatureLevel = 0xa100; // D3D_FEATURE_LEVEL_10_1;
	size_t m_uD3d11MinVRam = 256 << 20; // 256Mb
	bool m_bSupportBgra = true;

	bool m_bVerbose = true;
	bool m_bOutputToConsole = false;
};

