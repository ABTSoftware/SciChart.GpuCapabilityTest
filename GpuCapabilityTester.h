#pragma once

#include <string>
#include <sstream>

struct GpuRequirements
{
	size_t m_D3d11MinFeatureLevel = 0xa100; // D3D_FEATURE_LEVEL_10_1;
	size_t m_uLowVRamThreshold = 256 << 20; // 256Mb
	bool m_bSupportBgra = true;
	std::wstring m_srtBlacklist;
	wchar_t m_cBlDelimiter;
};

struct GpuCapabilities
{
	bool m_bD3d9Support;
	bool m_bD3d11Support;
	unsigned m_uAdapterDeviceId;
	bool m_bLowVRam;
	bool m_bBlacklisted;
	std::wstring m_srtLogMessages;
};

class GpuCapabilityTester
{
public:
	bool GetOutputToDebug() const { return m_bOutputToDebug; }
	void SetOutputToDebug(bool _bValue) { m_bOutputToDebug = _bValue; }

	bool GetOutputToConsole() const { return m_bOutputToConsole; }
	void SetOutputToConsole(bool _bValue) { m_bOutputToConsole = _bValue; }

	bool GetOutputToFile() const { return m_bOutputToFile; }
	void SetOutputToFile(bool _bValue) { m_bOutputToFile = _bValue; }

	bool GetOutputToString() const { return m_bOutputToString; }
	void SetOutputToString(bool _bValue) { m_bOutputToString = _bValue; }

	GpuCapabilities FindOptimalAdapter(const GpuRequirements& _Reqs);

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

	bool m_bOutputToDebug = false;
	bool m_bOutputToConsole = false;
	bool m_bOutputToFile = false;
	bool m_bOutputToString = true;

	bool m_bOutputFileReady = false;
	std::wstringstream m_StringStream;
};

