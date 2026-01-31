#ifndef MONITOR_INFO_H
#define MONITOR_INFO_H

#include <d3d11.h>
#include <dxgi1_6.h>
#include <setupapi.h>
#include <devguid.h>
#include <cfgmgr32.h>

#include <string>
#include <vector>
#include <map>
#include <unordered_map>

#pragma comment(lib, "setupapi.lib")


int computeGCD(int a, int b);
std::pair<int, int> calculateAspectRatio(int width, int height);

struct MonitorInfo {
	std::wstring name;
	std::wstring friendlyName;

	std::wstring adapterName;

	HMONITOR handle;

	int adapterIndex;
	int outputIndex;

	RECT desktopCoordinates;
	int currentWidth;
	int currentHeight;
	int currentRefreshRate;


	struct DisplayRefreshRate {
		std::pair<int, int> hz;
		float GetRefreshRate() const;
		int index;
	};

	struct DisplayResolution {
		std::pair<int, int> size;
		DXGI_FORMAT format;
		std::vector<DisplayRefreshRate> refresh_rates;
	};

	struct DisplayAspect {
		std::pair<int, int> aspect_ratio;
		std::vector<DisplayResolution> resolutions;
	};

	std::vector<DisplayAspect> aspects;


	DisplayResolution GetHighestResolution() const;
	DisplayResolution GetLowestResolution() const;
	bool IsPrimary() const;

};


std::wstring GetMonitorEDIDName(HMONITOR hMonitor);




std::vector<MonitorInfo> GetAvailableMonitor();
MonitorInfo GetPrimary(const std::vector<MonitorInfo>& monitors);

void PrintAvailableMonitors(const std::vector<MonitorInfo>& monitors);

#endif