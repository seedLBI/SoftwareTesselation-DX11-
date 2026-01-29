#ifndef GPUINFO_H
#define GPUINFO_H


#include <d3d11.h>
#include <iostream>
#include <string>
#include <vector>

struct GPUInfo {
    int index;
    std::string description;
    int dedicatedMemoryMB;
    int systemMemoryMB;
    int vendorId;
    bool supportsDX11;
};

std::vector<GPUInfo> GetAvailableGPU();
void PrintInfoGPU(const std::vector<GPUInfo>& info);

#endif