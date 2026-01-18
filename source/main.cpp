#include <iostream>
#include <windows.h>

#include "Framework/DX11.Framework.h"
#include "Application/Application.h"

#include "Shader/DX11.Shader.h"
#include <DirectXMath.h>


using namespace framework::time;
using namespace framework::directx;
using namespace framework::input;
using namespace framework::window;
using namespace DirectX;


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow) {

    Application app(hInstance, nCmdShow);
    
    /*
    DX11_Shader simple_shader;

    HRESULT hr;
        hr = simple_shader.LoadShadersFromFile(L"resources\\shaders\\shader_triangle.hlsl", "VSMain", "PSMain");
    if (FAILED(hr)) { MessageBoxA(framework::window::GetHandle(), "Failed to compile shaders", "Error", MB_OK); return -1; }

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    simple_shader.CreateInputLayout(layout, ARRAYSIZE(layout));

    struct Vertex { XMFLOAT3 pos; XMFLOAT2 uv; };


    std::vector<Vertex> verts = {
        {{-100,-100,0}, {0,0}},
        {{-100, 100,0}, {0,1}},
        {{ 100, 100,0}, {1,1}},
        {{ 100,-100,0}, {1,0}},
    };
    UINT indices[] = { 0,1,2, 0,2,3 };

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = (UINT)(sizeof(Vertex) * verts.size());
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA init = {};
    init.pSysMem = verts.data();
    ComPtr<ID3D11Buffer> vb;
    framework::directx::GetDevice()->CreateBuffer(&bd, &init, vb.GetAddressOf());


    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(indices);
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    D3D11_SUBRESOURCE_DATA iinit = {};
    iinit.pSysMem = indices;
    ComPtr<ID3D11Buffer> ib;
    framework::directx::GetDevice()->CreateBuffer(&ibd, &iinit, ib.GetAddressOf());


    struct CB { XMFLOAT4X4 wvp; } cbData;
    simple_shader.CreateConstantBuffer("PerObject", 0, sizeof(CB));



    float lastSecond = 0.f;
    int countFrames = 0;
    */


    bool shouldExit = false;
    while (!shouldExit)
    {
        /*
        framework::core::Update();

        auto pos_screen = framework::input::GetPositionScreen();
        auto pos_client = framework::input::GetPositionClient();
        auto delta = framework::input::GetDelta();
        auto wheel = framework::input::GetWheelDelta();

        if (IsMousePressed(MouseButton_LEFT)) {
            ToggleVSync();
            ToggleClipCursor();
            std::cout << "Toggled VSync\n";
        }

        
        if (GetTimeProgram() - lastSecond > 1.f) {

            std::cout << countFrames * 1.f << "  " << GetDeltaTime() << "ms" << std::endl;

            lastSecond = GetTimeProgram();
            countFrames = 0;

        }
        countFrames++;

        float r = sinf(GetTimeProgram()) * 0.5f + 0.5f;
        float g = cosf(GetTimeProgram()) * 0.5f + 0.5f;


        BeginScene(0.f, 0.f, 0.f, 1.f);


        auto size_window = GetSize();


        XMMATRIX wvp = SimpleOrtho(size_window.x, size_window.y);
        XMStoreFloat4x4(&cbData.wvp, XMMatrixTranspose(wvp)); // transpose для HLSL column-major
        simple_shader.UpdateConstantBuffer("PerObject", &cbData, sizeof(cbData));
        simple_shader.BindConstantBufferToVS("PerObject");

        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        GetContext()->IASetVertexBuffers(0, 1, vb.GetAddressOf(), &stride, &offset);
        GetContext()->IASetIndexBuffer(ib.Get(), DXGI_FORMAT_R32_UINT, 0);
        GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


        simple_shader.Bind();
        GetContext()->DrawIndexed(6, 0, 0);


        EndScene();
        */

        app.Update();

        app.Render();


        MSG msg;
        while (!shouldExit && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {

            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
                shouldExit = true;
        }
    }

    /*
    framework::core::Terminate();
    */
    return 0;
}