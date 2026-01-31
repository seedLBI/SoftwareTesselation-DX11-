#include "MonitorInfo.h"



int computeGCD(int a, int b) {
    a = std::abs(a);
    b = std::abs(b);
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

std::pair<int, int> calculateAspectRatio(int width, int height) {
    int gcd = computeGCD(width, height);
    int numerator = width / gcd;
    int denumerator = height / gcd;

    if (numerator == 8 && denumerator == 5)
        return { 16,10 };

    return { numerator,denumerator };
}


float MonitorInfo::DisplayRefreshRate::GetRefreshRate() const {
    return (float)hz.first / (float)hz.second;
}



MonitorInfo::DisplayResolution MonitorInfo::GetHighestResolution() const {
    std::pair<int, int> index_max = {-1, -1};
    std::pair<int, int> max_value = { 0,  0};

    for (size_t i = 0; i < aspects.size(); i++) {
        for (size_t j = 0; j < aspects[i].resolutions.size(); j++) {

            auto& res = aspects[i].resolutions[j];

            if (res.size.first * res.size.second > max_value.first * max_value.second) {
                max_value = aspects[i].resolutions[j].size;
                index_max = {i,j};
            }

        }
    }

    if (index_max.first != -1) {
        return aspects[index_max.first].resolutions[index_max.second];
    }


    return {};
}

MonitorInfo::DisplayResolution MonitorInfo::GetLowestResolution() const {
    std::pair<int, int> index_min = { 32768, 32768 };
    std::pair<int, int> max_value = { 0,  0 };

    for (size_t i = 0; i < aspects.size(); i++) {
        for (size_t j = 0; j < aspects[i].resolutions.size(); j++) {

            auto& res = aspects[i].resolutions[j];

            if (res.size.first * res.size.second < max_value.first * max_value.second) {
                max_value = aspects[i].resolutions[j].size;
                index_min = { i,j };
            }

        }
    }

    if (index_min.first != INT_MAX) {
        return aspects[index_min.first].resolutions[index_min.second];
    }


    return {};
}



bool MonitorInfo::IsPrimary() const {
    return desktopCoordinates.left == 0 && desktopCoordinates.top == 0;
}


std::wstring GetMonitorEDIDName(HMONITOR hMonitor) {
    std::wstring result;

    MONITORINFOEX monitorInfo;
    monitorInfo.cbSize = sizeof(monitorInfo);

    if (!GetMonitorInfo(hMonitor, &monitorInfo)) {
        return result;
    }

    DISPLAY_DEVICE displayDevice;
    ZeroMemory(&displayDevice, sizeof(displayDevice));
    displayDevice.cb = sizeof(displayDevice);

    for (DWORD devIndex = 0;
        EnumDisplayDevices(monitorInfo.szDevice, devIndex, &displayDevice, 0);
        devIndex++) {

        if (displayDevice.StateFlags & DISPLAY_DEVICE_ACTIVE &&
            displayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) {

            HDEVINFO devInfo = SetupDiGetClassDevs(
                &GUID_DEVCLASS_MONITOR,
                nullptr,
                nullptr,
                DIGCF_PRESENT | DIGCF_PROFILE
            );

            if (devInfo != INVALID_HANDLE_VALUE) {
                SP_DEVINFO_DATA devInfoData;
                devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

                for (DWORD i = 0;
                    SetupDiEnumDeviceInfo(devInfo, i, &devInfoData);
                    i++) {

                    HKEY hKey = SetupDiOpenDevRegKey(
                        devInfo,
                        &devInfoData,
                        DICS_FLAG_GLOBAL,
                        0,
                        DIREG_DEV,
                        KEY_READ
                    );

                    if (hKey != INVALID_HANDLE_VALUE) {
                        DWORD edidSize = 0;
                        DWORD type = 0;

                        if (RegQueryValueEx(hKey, L"EDID", nullptr, &type,
                            nullptr, &edidSize) == ERROR_SUCCESS) {

                            std::vector<BYTE> edidData(edidSize);
                            if (RegQueryValueEx(hKey, L"EDID", nullptr, &type,
                                edidData.data(), &edidSize) == ERROR_SUCCESS) {

                                if (edidSize >= 128) {
                                    for (int descOffset = 54; descOffset <= 125; descOffset += 18) {
                                        if (edidData[descOffset] == 0x00 &&
                                            edidData[descOffset + 1] == 0x00 &&
                                            edidData[descOffset + 2] == 0x00 &&
                                            edidData[descOffset + 3] == 0xFC) {

                                            std::wstring monitorName;
                                            for (int j = 0; j < 13; j++) {
                                                BYTE c = edidData[descOffset + 5 + j];
                                                if (c == 0x0A) break;
                                                if (c >= 32 && c <= 126) {
                                                    monitorName += static_cast<wchar_t>(c);
                                                }
                                            }

                                            while (!monitorName.empty() &&
                                                monitorName.back() == ' ') {
                                                monitorName.pop_back();
                                            }

                                            if (!monitorName.empty()) {
                                                result = monitorName;
                                            }
                                            break;
                                        }
                                    }

                                    if (result.empty()) {
                                        std::wstring manufacturer;
                                        BYTE id1 = ((edidData[8] >> 2) & 0x1F) + '@';
                                        BYTE id2 = (((edidData[8] & 0x03) << 3) | ((edidData[9] >> 5) & 0x07)) + '@';
                                        BYTE id3 = (edidData[9] & 0x1F) + '@';

                                        manufacturer += static_cast<wchar_t>(id1);
                                        manufacturer += static_cast<wchar_t>(id2);
                                        manufacturer += static_cast<wchar_t>(id3);

                                        WORD productId = (edidData[10] << 8) | edidData[11];

                                        DWORD serial = (edidData[12] << 24) | (edidData[13] << 16) |
                                            (edidData[14] << 8) | edidData[15];

                                        wchar_t buffer[256];
                                        swprintf_s(buffer, L"%s-%04X-%08X",
                                            manufacturer.c_str(), productId, serial);
                                        result = buffer;
                                    }
                                }
                            }
                        }

                        RegCloseKey(hKey);

                        if (!result.empty()) {
                            break;
                        }
                    }
                }

                SetupDiDestroyDeviceInfoList(devInfo);
            }
        }

        if (!result.empty()) {
            break;
        }
    }

    return result;
}

std::vector<MonitorInfo> GetAvailableMonitor() {

    std::vector<MonitorInfo> availableMonitors;

    HRESULT hr;
    IDXGIFactory* dxgiFactory = nullptr;

    hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
    if (FAILED(hr)) {
        return availableMonitors;
    }

    IDXGIAdapter* adapter = nullptr;
    int adapterIndex = 0;

    while (dxgiFactory->EnumAdapters(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND) {
        DXGI_ADAPTER_DESC adapterDesc;
        adapter->GetDesc(&adapterDesc);

        IDXGIOutput* output = nullptr;
        int outputIndex = 0;

        while (adapter->EnumOutputs(outputIndex, &output) != DXGI_ERROR_NOT_FOUND) {
            MonitorInfo monitorInfo;
            monitorInfo.adapterIndex = adapterIndex;
            monitorInfo.outputIndex = outputIndex;

            monitorInfo.adapterName = adapterDesc.Description;

            DXGI_OUTPUT_DESC outputDesc;
            output->GetDesc(&outputDesc);
            monitorInfo.name = outputDesc.DeviceName;
            monitorInfo.handle = outputDesc.Monitor;
            monitorInfo.desktopCoordinates = outputDesc.DesktopCoordinates;

            monitorInfo.friendlyName = GetMonitorEDIDName(outputDesc.Monitor);

            DXGI_OUTPUT_DESC1 outputDesc1;
            if (SUCCEEDED(output->QueryInterface(__uuidof(IDXGIOutput1), (void**)&output))) {

            }

            UINT numModes = 0;
            DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

            hr = output->GetDisplayModeList(format, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr);
            if (SUCCEEDED(hr) && numModes > 0) {
                DXGI_MODE_DESC* displayModes = new DXGI_MODE_DESC[numModes];
                hr = output->GetDisplayModeList(format, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModes);

                if (SUCCEEDED(hr)) {
                    for (UINT i = 0; i < numModes; i++) {

                        std::pair<int, int> size = { displayModes[i].Width, displayModes[i].Height };
                        std::pair<int, int> aspect = calculateAspectRatio(displayModes[i].Width, displayModes[i].Height);
                        std::pair<int, int> rate = { displayModes[i].RefreshRate.Numerator, displayModes[i].RefreshRate.Denominator };

                        MonitorInfo::DisplayRefreshRate refresh_rate;
                        refresh_rate.index = i;
                        refresh_rate.hz = rate;

                        MonitorInfo::DisplayResolution resolution;
                        resolution.size = size;
                        resolution.format = displayModes[i].Format;

                        MonitorInfo::DisplayAspect aspect_ratio;
                        aspect_ratio.aspect_ratio = aspect;


                        auto& aspects = monitorInfo.aspects;

                        if (aspects.empty()) {
                            resolution.refresh_rates.push_back(refresh_rate);
                            aspect_ratio.resolutions.push_back(resolution);
                            aspects.push_back(aspect_ratio);
                        }
                        else {

                            bool aspect_finded = false;

                            for (auto& obj_aspect : aspects) {

                                if (obj_aspect.aspect_ratio == aspect) {
                                    aspect_finded = true;


                                    bool resolution_finded = false;

                                    for (auto& res : obj_aspect.resolutions) {
                                        if (res.size == size) {
                                            resolution_finded = true;

                                            bool hz_finded = false;
                                            for (auto& hz : res.refresh_rates) {
                                                if (hz.hz == rate) {
                                                    hz_finded = true;
                                                    break;
                                                }
                                            }

                                            if (hz_finded == false) {
                                                res.refresh_rates.push_back(refresh_rate);
                                            }

                                        }

                                    }

                                    if (resolution_finded == false) {
                                        resolution.refresh_rates.push_back(refresh_rate);
                                        obj_aspect.resolutions.push_back(resolution);
                                    }
                                }

                            }

                            if (aspect_finded == false) {
                                resolution.refresh_rates.push_back(refresh_rate);
                                aspect_ratio.resolutions.push_back(resolution);
                                aspects.push_back(aspect_ratio);
                            }
                        }

                    }
                }

                delete[] displayModes;
            }

            availableMonitors.push_back(monitorInfo);

            output->Release();
            outputIndex++;
        }

        adapter->Release();
        adapterIndex++;
    }

    dxgiFactory->Release();
    dxgiFactory = nullptr;


    return availableMonitors;
}

MonitorInfo GetPrimary(const std::vector<MonitorInfo>& monitors) {
    for (size_t i = 0; i < monitors.size(); i++) {
        if (monitors[i].IsPrimary()) {
            return monitors[i];
            break;
        }
    }
    return {};
}

void PrintAvailableMonitors(const std::vector<MonitorInfo>& monitors) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    const WORD text_red = FOREGROUND_RED | FOREGROUND_INTENSITY;
    const WORD text_green = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    const WORD text_blue = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    const WORD text_yellow = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
    const WORD text_gray = FOREGROUND_INTENSITY;
    const WORD text_white = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;

    const WORD back_white = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY;
    const WORD back_yellow = BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY;



    const int width_box = 66;

    SetConsoleTextAttribute(hConsole, back_white);
    printf("%*s\n", width_box, "");
    printf("%*s->[", width_box / 2 - 3 - 19 / 2, "");
    SetConsoleTextAttribute(hConsole, text_blue | back_white);
    printf("AVAILABLE MONITORs");
    SetConsoleTextAttribute(hConsole, back_white);
    printf("]<-%*s\n", width_box / 2 - 3 - 19 / 2, "");
    printf("%*s\n", width_box, "");
    SetConsoleTextAttribute(hConsole, text_white);


    int primaryIndex = -1;
    for (size_t i = 0; i < monitors.size(); i++) {
        if (monitors[i].IsPrimary()) {
            primaryIndex = static_cast<int>(i);
            break;
        }
    }

    for (size_t i = 0; i < monitors.size(); i++) {
        const auto& monitor = monitors[i];

        printf("\n%zu: ", i);

        if (primaryIndex == static_cast<int>(i))
            SetConsoleTextAttribute(hConsole, back_yellow);
        else
            SetConsoleTextAttribute(hConsole, text_white);

        std::wstring displayName = monitor.friendlyName.empty() ? monitor.name : monitor.friendlyName;
        wprintf(L"[%s]", displayName.c_str());

        if (primaryIndex == static_cast<int>(i)) {
            SetConsoleTextAttribute(hConsole, text_green );
            printf(" [PRIMARY]");
        }
        printf("\n");

        SetConsoleTextAttribute(hConsole, text_gray);
        printf("               Name: ");
        SetConsoleTextAttribute(hConsole, text_white);
        wprintf(L"%s\n", monitor.name.c_str());

        SetConsoleTextAttribute(hConsole, text_gray);
        printf("            Adapter: ");
        SetConsoleTextAttribute(hConsole, text_white);
        wprintf(L"%s\n", monitor.adapterName.c_str());

        SetConsoleTextAttribute(hConsole, text_gray);
        printf("           Position: ");
        SetConsoleTextAttribute(hConsole, text_white);
        printf("[%d, %d, %d, %d]\n",
            monitor.desktopCoordinates.left,
            monitor.desktopCoordinates.top,
            monitor.desktopCoordinates.right,
            monitor.desktopCoordinates.bottom);



        SetConsoleTextAttribute(hConsole, text_gray);
        printf("             Handle: ");
        SetConsoleTextAttribute(hConsole, text_white);
        printf("0x%p\n", monitor.handle);


        SetConsoleTextAttribute(hConsole, text_gray);
        printf("       Current Size: ");
        SetConsoleTextAttribute(hConsole, text_yellow);

        int current_width = monitor.desktopCoordinates.right - monitor.desktopCoordinates.left;
        int current_height = monitor.desktopCoordinates.bottom - monitor.desktopCoordinates.top;

        printf("%dx%d\n", current_width, current_height);



        SetConsoleTextAttribute(hConsole, text_gray);
        printf("       Aspect ratio: \n");


        const auto& aspects = monitors[i].aspects;

        for (size_t i = 0; i < aspects.size(); i++) {
            printf("\t%i : %i \n", aspects[i].aspect_ratio.first, aspects[i].aspect_ratio.second);



            for (size_t j = 0; j < aspects[i].resolutions.size(); j++)
            {


                SetConsoleTextAttribute(hConsole, text_gray);

                if (current_width == aspects[i].resolutions[j].size.first &&
                    current_height == aspects[i].resolutions[j].size.second)
                {

                    SetConsoleTextAttribute(hConsole, text_yellow);

                }
                

                printf("\t\t%i x %i ", aspects[i].resolutions[j].size.first, aspects[i].resolutions[j].size.second);

                if (aspects[i].resolutions[j].refresh_rates.empty() == false) {

                    printf("(");

                    for (size_t k = 0; k < aspects[i].resolutions[j].refresh_rates.size(); k++)
                    {
                        int hz_value = (aspects[i].resolutions[j].refresh_rates[k].hz.first) / (aspects[i].resolutions[j].refresh_rates[k].hz.second);

                        printf("%i", hz_value);

                        if (k != aspects[i].resolutions[j].refresh_rates.size() - 1) {
                            printf(", ");
                        }
                    }

                    printf(")");
                }

                printf("\n");

                SetConsoleTextAttribute(hConsole, text_gray);
            }


        }




        if (i < monitors.size() - 1) {
            SetConsoleTextAttribute(hConsole, text_gray);
            printf("    --------------------\n");
        }
    }

    SetConsoleTextAttribute(hConsole, text_white);
    printf("\n===================================================\n\n");
}