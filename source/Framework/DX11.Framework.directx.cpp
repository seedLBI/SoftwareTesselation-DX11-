#include "DX11.Framework.h"

namespace framework {
	namespace core {

		namespace VARS {

			namespace directx {
				int m_videoCardMemory;
				char m_videoCardDescription[128];
				IDXGISwapChain* m_swapChain;
				ID3D11Device* m_device;
				ID3D11DeviceContext* m_deviceContext;
				ID3D11RenderTargetView* m_renderTargetView;
				ID3D11Texture2D* m_depthStencilBuffer;
				ID3D11DepthStencilState* m_depthStencilState;
				ID3D11DepthStencilView* m_depthStencilView;
				ID3D11RasterizerState* m_rasterState;
				D3D11_VIEWPORT m_viewport;

				bool isVSyncEnabled = true;
			}

			void InitDirectX() {
				using namespace framework::core::VARS::directx;

				HRESULT result;
				IDXGIFactory* factory;
				IDXGIAdapter* adapter;
				IDXGIOutput* adapterOutput;
				unsigned int numModes, i, numerator, denominator;
				unsigned long long stringLength;
				DXGI_MODE_DESC* displayModeList;
				DXGI_ADAPTER_DESC adapterDesc;
				int error;
				DXGI_SWAP_CHAIN_DESC swapChainDesc;
				D3D_FEATURE_LEVEL featureLevel;
				ID3D11Texture2D* backBufferPtr;
				D3D11_TEXTURE2D_DESC depthBufferDesc;
				D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
				D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
				D3D11_RASTERIZER_DESC rasterDesc;
				float fieldOfView, screenAspect;



				result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
				if (FAILED(result))
					assert(1);

				result = factory->EnumAdapters(0, &adapter);
				if (FAILED(result))
					assert(1);

				result = adapter->EnumOutputs(0, &adapterOutput);
				if (FAILED(result))
					assert(1);

				result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
				if (FAILED(result))
					assert(1);

				displayModeList = new DXGI_MODE_DESC[numModes];
				if (!displayModeList)
					assert(1);

				result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
				if (FAILED(result))
					assert(1);


				printf("Available modes display: \n");

				for (int i = 0; i < numModes; i++) {

					printf("\t[%i]: %ux%u (%f fps)\n",
						i, 
						displayModeList[i].Width, 
						displayModeList[i].Height,
						(float)displayModeList[i].RefreshRate.Numerator / (float)displayModeList[i].RefreshRate.Denominator);


					if (displayModeList[i].Width == (unsigned int)core::VARS::window::mScreenWidth)
					{
						if (displayModeList[i].Height == (unsigned int)core::VARS::window::mScreenHeight)
						{
							numerator = displayModeList[i].RefreshRate.Numerator;
							denominator = displayModeList[i].RefreshRate.Denominator;
						}
					}
				}

				printf("\n");

				auto gpus = GetAvailableGPU();
				PrintInfoGPU(gpus);


				delete[] displayModeList;
				displayModeList = 0;
				adapterOutput->Release();
				adapterOutput = 0;
				adapter->Release();
				adapter = 0;
				factory->Release();
				factory = 0;

				ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

				swapChainDesc.BufferCount = 1;

				swapChainDesc.BufferDesc.Width  = core::VARS::window::mScreenWidth;
				swapChainDesc.BufferDesc.Height = core::VARS::window::mScreenHeight;

				swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

				if (0)
				{
					swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
					swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
				}
				else
				{
					swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
					swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
				}

				swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

				swapChainDesc.OutputWindow = framework::window::GetHandle();

				// Turn multisampling off.
				swapChainDesc.SampleDesc.Count = 1;
				swapChainDesc.SampleDesc.Quality = 0;

				if (0)
				{
					swapChainDesc.Windowed = false;
				}
				else
				{
					swapChainDesc.Windowed = true;
				}

				swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
				swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

				swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

				swapChainDesc.Flags = 0;

				featureLevel = D3D_FEATURE_LEVEL_11_0;

				result = D3D11CreateDeviceAndSwapChain(
					NULL, 
					D3D_DRIVER_TYPE_HARDWARE, 
					NULL, 
					0, 
					&featureLevel, 
					1,
					D3D11_SDK_VERSION, 
					&swapChainDesc, 
					&m_swapChain, 
					&m_device, 
					NULL, 
					&m_deviceContext);

				if (FAILED(result))
					assert(1);


				// Get the pointer to the back buffer.
				result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
				if (FAILED(result))
					assert(1);

				// Create the render target view with the back buffer pointer.
				result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
				if (FAILED(result))
					assert(1);

				// Release pointer to the back buffer as we no longer need it.
				backBufferPtr->Release();
				backBufferPtr = 0;

				// Initialize the description of the depth buffer.
				ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

				// Set up the description of the depth buffer.
				depthBufferDesc.Width = core::VARS::window::mScreenWidth;
				depthBufferDesc.Height = core::VARS::window::mScreenHeight;
				depthBufferDesc.MipLevels = 1;
				depthBufferDesc.ArraySize = 1;
				depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				depthBufferDesc.SampleDesc.Count = 1;
				depthBufferDesc.SampleDesc.Quality = 0;
				depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
				depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
				depthBufferDesc.CPUAccessFlags = 0;
				depthBufferDesc.MiscFlags = 0;

				result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
				if (FAILED(result))
					assert(1);


				// Initialize the description of the stencil state.
				ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

				// Set up the description of the stencil state.
				depthStencilDesc.DepthEnable = true;
				depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
				depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

				depthStencilDesc.StencilEnable = true;
				depthStencilDesc.StencilReadMask = 0xFF;
				depthStencilDesc.StencilWriteMask = 0xFF;

				// Stencil operations if pixel is front-facing.
				depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
				depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

				// Stencil operations if pixel is back-facing.
				depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
				depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;


				// Create the depth stencil state.
				result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
				if (FAILED(result))
					assert(1);

				// Set the depth stencil state.
				m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

				// Initialize the depth stencil view.
				ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

				// Set up the depth stencil view description.
				depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
				depthStencilViewDesc.Texture2D.MipSlice = 0;

				// Create the depth stencil view.
				result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
				if (FAILED(result))
					assert(1);

				// Bind the render target view and depth stencil buffer to the output render pipeline.
				m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

				// Setup the raster description which will determine how and what polygons will be drawn.
				rasterDesc.AntialiasedLineEnable = false;
				rasterDesc.CullMode = D3D11_CULL_NONE;
				rasterDesc.DepthBias = 0;
				rasterDesc.DepthBiasClamp = 0.0f;
				rasterDesc.DepthClipEnable = true;
				rasterDesc.FillMode = D3D11_FILL_SOLID;
				rasterDesc.FrontCounterClockwise = false;
				rasterDesc.MultisampleEnable = false;
				rasterDesc.ScissorEnable = false;
				rasterDesc.SlopeScaledDepthBias = 0.0f;

				// Create the rasterizer state from the description we just filled out.
				result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
				if (FAILED(result))
					assert(1);

				// Now set the rasterizer state.
				m_deviceContext->RSSetState(m_rasterState);

				// Setup the viewport for rendering.
				m_viewport.Width = (float)core::VARS::window::mScreenWidth;
				m_viewport.Height = (float)core::VARS::window::mScreenHeight;
				m_viewport.MinDepth = 0.0f;
				m_viewport.MaxDepth = 1.0f;
				m_viewport.TopLeftX = 0.0f;
				m_viewport.TopLeftY = 0.0f;

				// Create the viewport.
				m_deviceContext->RSSetViewports(1, &m_viewport);

				// Setup the projection matrix.
				fieldOfView = 3.141592654f / 4.0f;
				screenAspect = (float)core::VARS::window::mScreenWidth / (float)core::VARS::window::mScreenHeight;




				ID3D11BlendState* m_alphaEnableBlendingState = nullptr;
				D3D11_BLEND_DESC blendDesc;
				ZeroMemory(&blendDesc, sizeof(blendDesc));
				blendDesc.RenderTarget[0].BlendEnable = TRUE;
				blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE ;
				blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

				result = m_device->CreateBlendState(&blendDesc, &m_alphaEnableBlendingState);

				float blendFactor[4] = { 0,0,0,0 }; // обычно 0
				// чтоб применять ко всем маскам: sampleMask = 0xffffffff
				framework::directx::GetContext()->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);

			}
			void TerminateDirectX() {
				using namespace framework::core::VARS::directx;

				if (m_swapChain) {
					m_swapChain->SetFullscreenState(false, NULL);
				}

				if (m_rasterState) {
					m_rasterState->Release();
					m_rasterState = 0;
				}

				if (m_depthStencilView) {
					m_depthStencilView->Release();
					m_depthStencilView = 0;
				}

				if (m_depthStencilState) {
					m_depthStencilState->Release();
					m_depthStencilState = 0;
				}

				if (m_depthStencilBuffer) {
					m_depthStencilBuffer->Release();
					m_depthStencilBuffer = 0;
				}

				if (m_renderTargetView) {
					m_renderTargetView->Release();
					m_renderTargetView = 0;
				}

				if (m_deviceContext) {
					m_deviceContext->Release();
					m_deviceContext = 0;
				}

				if (m_device) {
					m_device->Release();
					m_device = 0;
				}

				if (m_swapChain) {
					m_swapChain->Release();
					m_swapChain = 0;
				}

				return;
			}
		}


		
	}

	namespace directx {
		using namespace framework::core::VARS::directx;

		ID3D11Device* GetDevice() {
			return m_device;
		}
		ID3D11DeviceContext* GetContext() {
			return m_deviceContext;
		}

		void BeginScene(const float& red, const float& green, const float& blue, const float& alpha) {
			

			float color[4];

			color[0] = red;
			color[1] = green;
			color[2] = blue;
			color[3] = alpha;

			m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);
			m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);


		}
		void BeginScene(const DirectX::XMFLOAT4& color) {
			using namespace framework::core::VARS::directx;

			float color_array[4];

			color_array[0] = color.x;
			color_array[1] = color.y;
			color_array[2] = color.z;
			color_array[3] = color.w;

			m_deviceContext->ClearRenderTargetView(m_renderTargetView, color_array);
			m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		}
		void EndScene() {
			using namespace framework::core::VARS::directx;

			if (isVSyncEnabled)
				m_swapChain->Present(1, 0);
			else
				m_swapChain->Present(0, 0);
		}

		void SetBackBufferRenderTarget() {
			m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
		}

		void ResetViewport() {
			if (!m_deviceContext)
				return;

			m_viewport.Width = window::GetSize().x;
			m_viewport.Height = window::GetSize().y;
			m_deviceContext->RSSetViewports(1, &m_viewport);
			

		}


		void EnableVSync() {
			isVSyncEnabled = true;
		}
		void DisableVSync() {
			isVSyncEnabled = false;
		}
		void ToggleVSync() {
			isVSyncEnabled = !isVSyncEnabled;
		}


	}
}