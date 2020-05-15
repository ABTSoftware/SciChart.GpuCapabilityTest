#pragma once

#include <limits>

struct GpuRequirements
{
	size_t m_D3d11MinFeatureLevel = 0xa100; // D3D_FEATURE_LEVEL_10_1;
	size_t m_uLowVRamThreshold = 256 << 20; // 256Mb
	bool m_bSupportBgra = true;
};

struct GpuCapabilities
{
	bool m_bD3d9Support;
	bool m_bD3d11Support;
	unsigned m_uAdapterDeviceId;
	bool m_bLowVRam;
};

class GpuCapabilityTester
{
public:
	size_t GetOutputToDebug() const { return m_bOutputToDebug; }
	void SetOutputToDebug(bool _bValue) { m_bOutputToDebug = _bValue; }

	size_t GetOutputToConsole() const { return m_bOutputToConsole; }
	void SetOutputToConsole(bool _bValue) { m_bOutputToConsole = _bValue; }

	size_t GetOutputToFile() const { return m_bOutputToFile; }
	void SetOutputToFile(bool _bValue) { m_bOutputToFile = _bValue; }

	GpuCapabilities FindOptimalAdapter( const GpuRequirements& _Reqs );

private:
	void LogMessage(const char* _acMsg);
	void LogMessageFormatted(const char* _acFormat, ...);
	void LogMessageW(const wchar_t* _acMsg);
	void LogMessageFormattedW(const wchar_t* _acFormat, ...);
	void LogMessageLine(const char* _acMsg);
	void LogMessageLineW(const wchar_t* _acMsg);
	void PrepareOutputFile();
	void OutputToFile(const char* _acMsg);
	void OutputToFileW(const wchar_t* _acMsg);

	bool m_bOutputToDebug = true;
	bool m_bOutputToConsole = false;
	bool m_bOutputToFile = false;
	bool m_bOutputFileReady = false;
};

