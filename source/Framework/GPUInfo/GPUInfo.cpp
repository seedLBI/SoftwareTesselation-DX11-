#include "GPUInfo.h"

std::vector<GPUInfo> GetAvailableGPU() {
    std::vector<GPUInfo> gpus;

    IDXGIFactory* factory = nullptr;
    HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
    if (FAILED(result)) return gpus;

    IDXGIAdapter* adapter = nullptr;
    UINT adapterIndex = 0;

    while (factory->EnumAdapters(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND) {
        DXGI_ADAPTER_DESC desc;
        result = adapter->GetDesc(&desc);

        if (SUCCEEDED(result)) {
            GPUInfo info;
            info.index = adapterIndex;

            char description[128];
            size_t convertedChars = 0;
            wcstombs_s(&convertedChars, description, desc.Description, 128);
            info.description = std::string(description,128);


            info.dedicatedMemoryMB = (int)(desc.DedicatedVideoMemory / (1024 * 1024));

            info.vendorId = desc.VendorId;

            D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
            D3D_FEATURE_LEVEL supportedLevel;

            result = D3D11CreateDevice(
                adapter,
                D3D_DRIVER_TYPE_UNKNOWN,
                NULL,
                0,
                featureLevels,
                1,
                D3D11_SDK_VERSION,
                NULL,
                &supportedLevel,
                NULL
            );

            info.supportsDX11 = SUCCEEDED(result);

            if (info.dedicatedMemoryMB != 0)
                gpus.push_back(info);

            
        }

        adapter->Release();
        adapterIndex++;
    }

    factory->Release();
    return gpus;
}

int GetIndexBestGPU(const std::vector<GPUInfo>& gpu_s) {
    if (gpu_s.empty()) {
        return -1;
    }

    int bestIndex = -1;
    float bestScore = -1.0f;

    for (size_t i = 0; i < gpu_s.size(); i++) {
        const GPUInfo& gpu = gpu_s[i];
        float score = 0.0f;

        if (gpu.supportsDX11) score += 100.0f;

        float memoryScore = (std::min)(60.0f,
            static_cast<float>(gpu.dedicatedMemoryMB) / 16384.0f * 60.0f);
        score += memoryScore;

        switch (gpu.vendorId) {
        case 0x10DE: // NVIDIA
            score += 30.0f;
            if (gpu.description.find("RTX") != std::string::npos ||
                gpu.description.find("GTX") != std::string::npos) {
                score += 20.0f;
            }
            break;
        case 0x1002: // AMD
            score += 25.0f;
            if (gpu.description.find("Radeon") != std::string::npos ||
                gpu.description.find("RX") != std::string::npos) {
                score += 15.0f;
            }
            break;
        case 0x8086: // Intel
            score += 10.0f;
            if (gpu.description.find("Arc") != std::string::npos) {
                score += 20.0f;
            }
            break;
        default:
            score += 5.0f;
        }

        if (gpu.dedicatedMemoryMB < 1024 ||
            gpu.description.find("Intel") != std::string::npos ||
            gpu.description.find("UHD") != std::string::npos ||
            gpu.description.find("HD Graphics") != std::string::npos) {
            score -= 50.0f;
        }

        if (score > bestScore) {
            bestScore = score;
            bestIndex = static_cast<int>(i);
        }
    }

    return bestIndex;
}



#include <Windows.h>

void PrintInfoGPU(const std::vector<GPUInfo>& gpus) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    const WORD text_red = FOREGROUND_RED | FOREGROUND_INTENSITY;
    const WORD text_green = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    const WORD text_blue = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    const WORD text_yellow = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
    const WORD text_gray = FOREGROUND_INTENSITY;
    const WORD text_white = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    

    const WORD back_white = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY;
    const WORD back_yellow = BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY;

    SetConsoleTextAttribute(hConsole, back_white);


    const int width_box = 66;

    printf("%*s\n", width_box, "");
    printf("%*s->[", width_box/2 - 3 - 15/2,"");
    SetConsoleTextAttribute(hConsole, text_red | back_white);
    printf("AVAILABLE GPUs");
    SetConsoleTextAttribute(hConsole, back_white);
    printf("]<-%*s\n", width_box / 2 - 3 - 15 / 2,"");
    printf("%*s\n", width_box, "");
    SetConsoleTextAttribute(hConsole, text_white);


    int index_best = GetIndexBestGPU(gpus);



    for (const GPUInfo& gpu : gpus) {

        printf("\n%i: ", gpu.index);

        if (index_best == gpu.index) {
            SetConsoleTextAttribute(hConsole, back_yellow);
            printf("[%s]", gpu.description.c_str());

            SetConsoleTextAttribute(hConsole, text_yellow);
            printf(" [BEST]\n");
        }
        else {
            SetConsoleTextAttribute(hConsole, text_white);
            printf("[%s]\n", gpu.description.c_str());
        }



        SetConsoleTextAttribute(hConsole, text_gray);
        std::cout << "\t    Support DX11: ";

        if (gpu.supportsDX11)
            SetConsoleTextAttribute(hConsole, text_green);
        else
            SetConsoleTextAttribute(hConsole, text_red);
        printf("%s\n", (gpu.supportsDX11 >= 1 ? "True" : "False"));

        SetConsoleTextAttribute(hConsole, text_gray);
        std::cout << "\tDedicated memory: ";
        SetConsoleTextAttribute(hConsole, text_white);
        printf("%i Mb\n", gpu.dedicatedMemoryMB);


        SetConsoleTextAttribute(hConsole, text_gray);
        std::cout << "\t       Vendor id: ";
        SetConsoleTextAttribute(hConsole, text_white);
        printf("%i\n", gpu.vendorId);

    }
    printf("===================================================\n\n");

}