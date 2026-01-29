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
            info.systemMemoryMB = (int)(desc.DedicatedSystemMemory / (1024 * 1024));
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

            gpus.push_back(info);
        }

        adapter->Release();
        adapterIndex++;
    }

    factory->Release();
    return gpus;
}

void PrintInfoGPU(const std::vector<GPUInfo>& info) {

    printf("=========== AVAILABLE GPUs ===========\n");

    for (const GPUInfo& gpu : info) {
        printf("%i : [%s]\n", gpu.index, gpu.description.c_str());
        printf("\tDedicated memory: %i Mb\n", gpu.dedicatedMemoryMB);
        printf("\t   System memory: %i Mb\n", gpu.systemMemoryMB);
        printf("\t    Support DX11: %s\n",  (gpu.supportsDX11 >= 1 ? "True" : "False"));
        printf("\t       Vendor id: %i\n\n", gpu.vendorId);

    }

}